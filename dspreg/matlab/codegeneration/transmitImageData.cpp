/* =====================================
=== LIMEREG - Lightweight Image Registration ===
========================================

Forked by Roelof Berg, Berg Solutions (rberg@berg-solutions.de) from the project
FIMREG. FIMREG was written for a distributed calculation on the PCIe card DSPC-8681
of Advantech. LIMEREG does not use DSPs and can be run without special hardware.

THIS IS A LIMITED RESEARCH PROTOTYPE. Documentation: www.berg-solutions.de/limereg.html

------------------------------------------------------------------------------

Copyright (c) 2014, RoelofBerg
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of the owner nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------------*/

/*
 * transmitImageData.cpp
 *
 * Code generation for function 'transmitImageData'
 *
 * C source code generated on: Mon Jul 02 10:11:21 2012
 *
 */

#include "stdafx.h"

/* Include files */
#include "rt_nonfinite.h"
#include "calcDSPLayout.h"
#include "diffimg.h"
#include "dstr_ssd.h"
#include "gaussnewton.h"
#include "gen_example_data.h"
#include "generatePyramidPC.h"
#include "get_current_time_in_sec.h"
#include "jacobian.h"
#include "jacobianOnTarget.h"
#include "myprintf.h"
#include "notiifyFinishedOnTarget.h"
#include "sendToTarget.h"
#include "ssd.h"
#include "ssdOnTarget.h"
#include "start_jacobianOnTarget.h"
#include "start_ssdOnTarget.h"
#include "transform.h"
#include "transmitImageData.h"
#include "waitUntilTargetReady.h"
#include "extract.h"
#include "dspreg_emxutil.h"
#include "dspreg_rtwutil.h"

/* Custom Source Code */
#include "pseudo_stdafx.h"             //precompiled header not possible because of include position of matlab

#include "CHPRPCRequestStoreImg.h"
#include "CHPRPCConnection.h"
#include "CBufferedWriter.h"

#include "CImgTransTimestamps.h"
#include "CThreadPool.h"
extern CThreadPool* gpThreadPool;	//This is global because it is used from the c - style matlab generated code.

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

//I hacked this in during the very last two hours of the project to parallelize the image extraction a bit. I didn't care so much about code quality as I have to finish the whole code (yes!) today.
struct TThreadData
{
	//uint8_T* TSinglePart;
	uint8_T* Tvec;
	uint32_T BoundBox[4];
	uint32_T d;
	//uint8_T* RSinglePart;
	uint8_T* Rvec;
	uint32_T b_DSPResponsibilityBox[4];
	uint32_T b_i;
	CHPRPCRequestStoreImg* pRequest;
	double ExtractFinishedWCTime;	//out parameter
	double ExtractFinishedCPUTime;	//out parameter
	double SndStartdWCTime;	//out parameter
	double SndStartdCPUTime;	//out parameter
};
TThreadData g_ThreadData[4];	//Allocate memory (pointers will be used below, not the index).
volatile TThreadData* g_lastImageExtractor = NULL;  //Remember the thread that was the last to finish image extraction
volatile TThreadData* g_lastPCIeSender = NULL;  //Remember the thread that was the last to start PCIe sendingc


DWORD WINAPI ExtractAndSendThreadEntry(LPVOID lpParam) 
{
	TThreadData* pTDat = (TThreadData*)lpParam;
	//uint8_T* TSinglePart = pTDat->TSinglePart;
	uint8_T* Tvec = pTDat->Tvec;
	uint32_T* BoundBox = pTDat->BoundBox;
	const uint32_T d = pTDat->d;
	//uint8_T* RSinglePart = pTDat->RSinglePart;
	uint8_T* Rvec = pTDat->Rvec;
	uint32_T* b_DSPResponsibilityBox = pTDat->b_DSPResponsibilityBox;
	uint32_T b_i = pTDat->b_i;
	CHPRPCRequestStoreImg* pRequest = pTDat->pRequest;

	//Extract partial image (places the extracted lines into the HPRPC command buffer. In case of a PCIe transfer directly to the buffers of the non-paged kernel memory used for DMA.)
	CBufferedWriter& BufferedWriter=pRequest->GetHPRPCConnection().GetBufferedWriter();
	extract(Tvec, BoundBox, d, BufferedWriter);
	//Note: We could initiate the sending and start the pyramid generation here allready.
	extract(Rvec, b_DSPResponsibilityBox, d, BufferedWriter);

	//record duration
	pTDat->ExtractFinishedWCTime = get_wall_time();
	pTDat->ExtractFinishedCPUTime = get_cpu_time();
	g_lastImageExtractor = pTDat;	//last thread wins

    //Start sending the data buffer to the target DSP (async, header and image data)
	matlab_c_sendToTarget(b_i, pRequest);

	//Remember completion time (for performance analysis)
	pTDat->SndStartdWCTime = get_wall_time();
	pTDat->SndStartdCPUTime = get_cpu_time();
	g_lastPCIeSender = pTDat;	//last thread wins
	
	return 0;
}

void SpawnExtractAndSendThread(TThreadData* pThreadData, uint32_T CoreNo)
{
	//The threadpool is global because it is used from the c - style matlab generated code.
	gpThreadPool->Execute(CoreNo, ExtractAndSendThreadEntry, pThreadData);
}

/* Function Definitions */
void c_transmitImageData(uint32_T DSPCount, real32_T w[3], real32_T da, real32_T
  dtr, const emxArray_uint8_T *Tvec, const emxArray_uint8_T *Rvec, uint32_T d, uint32_T levels, CImgTransTimestamps& Timestamps)
{
  real32_T y;
  real32_T dmax;
  real32_T b_y;
  real32_T s;
  uint32_T aHeight;
  uint32_T aWidth;
  real32_T rr;
  real32_T angles[6];
  int32_T i;
  uint32_T b_i;
  real32_T rl;
  int32_T angle_size;
  int32_T c_y;
  int32_T Box_size_idx_0;
  int32_T ixstart;
  real32_T Box_data[48];
  real32_T uBox[8];
  int32_T x;
  //uint8_t *TSinglePart;
  //uint8_t *RSinglePart;
  real32_T rt;
  real32_T rb;
  real32_T DSPResponsibilityBox[4];
  int32_T ix;
  boolean_T exitg4;
  uint32_T marginL;
  boolean_T exitg3;
  uint32_T marginR;
  boolean_T exitg2;
  uint32_T marginO;
  boolean_T exitg1;
  uint32_T marginU;
  uint32_T BoundBox[4];
  uint32_T b_DSPResponsibilityBox[4];
  uint32_T MarginAddon[3];

  Timestamps.BeginXtrct.measureWallAndCPUTime();

  /* Global definition has only effect in matlab simulation (coder.extrinsic forbids global use here in matlab coder) */
  /* coder.extrinsic('global');  %this line unfortunately doesn't work */
  /* global GlobTParts GlobTPartsPtrs GlobRParts GlobRPartsPtrs GlobDSPResponsibilityBox GlobBoundBox GlobImgDimension; */
  y = (real32_T)d / 2.0F;
  dmax = (real32_T)d / 2.0F - 0.5F;
  b_y = (real32_T)d / 2.0F;
  s = (real32_T)d / 2.0F + 0.5F;

  /* Shifting, um negative Koordinaten in Matrixbereich zu bringen */
  /* ======================================================================== */
  /* = Relevante Bildteile berechnen, Bilder �bertragen */
  /* ======================================================================== */
  /* Calc layout of DSP grid regarding the Reference image */
  calcDSPLayout(DSPCount, &aWidth, &aHeight);

  /* Winkelst�tzstellen berechnen. */
  /* Einerseits die erlaubten Maximalwinkel w-da, w+da */
  /* Ferner alle Extrema zwischen den St�tzstelen ((x-45) mod 90 = 0) */
  rr = w[0] / 6.28318548F;
  rr = (rr - (real32_T)floor(rr)) * 6.28318548F;
  w[0] = rr;

  /* ensure angle to be in range 0 ... 360 deg */
  for (i = 0; i < 6; i++) {
    angles[i] = 0.0F;
  }

  angles[0] = w[0] - da;
  angles[1] = w[0] + da;
  b_i = 2U;

  /* Maximum diagonal extend is at 45deg, (90+45)deg, (190+45)deg etc. */
  /* wMax=single(0); */
  for (i = 0; i < 12; i++) {
    rl = -5.497787F + (real32_T)i * 1.57079637F;

    /* because dt is allowed to be 0 ... 360 deg */
    if ((angles[0] < rl) && (angles[1] > rl)) {
      angles[(int32_T)b_i] = rl;
      b_i++;
    }
  }

  angle_size = (int32_T)b_i;

  /* DSPResponsibilityBox=zeros(4,'single'); */
  /* BoundBox=zeros(4,'uint32'); */
  c_y = (int32_T)(((uint32_T)(int32_T)b_i - 1U) << 2);
  Box_size_idx_0 = (int32_T)((uint32_T)c_y + 4U);
  i = ((int32_T)((uint32_T)c_y + 4U) << 1) - 1;
  for (ixstart = 0; ixstart <= i; ixstart++) {
    Box_data[ixstart] = 0.0F;
  }

  for (ixstart = 0; ixstart < 8; ixstart++) {
    uBox[ixstart] = 0.0F;
  }

  b_i = 1U;
  x = 0;
  while (x <= (int32_T)(real32_T)aWidth - 1) {
    for (c_y = 0; c_y <= (int32_T)(real32_T)aHeight - 1; c_y++) {

      /*  Untransformed x,y edges per DSP ------------------- */
      rl = (((1.0F + (real32_T)x) - 1.0F) / (real32_T)aWidth * (real32_T)d -
            (real32_T)d / 2.0F) + 0.5F;

      /* -s for matching coordinate system to registration algorithm */
      rr = ((1.0F + (real32_T)x) / (real32_T)aWidth * (real32_T)d - (real32_T)d /
            2.0F) - 0.5F;
      rt = (((1.0F + (real32_T)c_y) - 1.0F) / (real32_T)aHeight * (real32_T)d -
            (real32_T)d / 2.0F) + 0.5F;
      rb = ((1.0F + (real32_T)c_y) / (real32_T)aHeight * (real32_T)d - (real32_T)
            d / 2.0F) - 0.5F;
      DSPResponsibilityBox[0] = rl;
      DSPResponsibilityBox[1] = rr;
      DSPResponsibilityBox[2] = rt;
      DSPResponsibilityBox[3] = rb;
      uBox[0] = rl;

      /* -s for matching coordinate system to registration algorithm */
      uBox[4] = rt;
      uBox[1] = rr;
      uBox[5] = rt;
      uBox[2] = rl;
      uBox[6] = rb;
      uBox[3] = rr;
      uBox[7] = rb;

      /*  Transform edge coordinates -------------------------- */
      for (i = 1; (uint32_T)i <= (uint32_T)angle_size; i = (int32_T)((uint32_T)i
            + 1U)) {
        for (ixstart = 0; ixstart < 4; ixstart++) {
          Box_data[(int32_T)(((uint32_T)i - 1U) << 2) + ixstart] = ((real32_T)
            cos(angles[i - 1]) * uBox[ixstart] - (real32_T)sin(angles[i - 1]) *
            uBox[4 + ixstart]) + w[1];
          Box_data[((int32_T)(((uint32_T)i - 1U) << 2) + ixstart) +
            Box_size_idx_0] = ((real32_T)sin(angles[i - 1]) * uBox[ixstart] +
                               (real32_T)cos(angles[i - 1]) * uBox[4 + ixstart])
            + w[2];
        }
      }

      /*  X Bounding Box ------------------------------- */
      i = (int32_T)((uint32_T)angle_size << 2);
      ixstart = 1;
      rl = Box_data[0];
      if (rtIsNaNF(Box_data[0])) {
        ix = 2;
        exitg4 = FALSE;
        while ((exitg4 == 0U) && (ix <= i)) {
          ixstart = ix;
          if (!rtIsNaNF(Box_data[ix - 1])) {
            rl = Box_data[ix - 1];
            exitg4 = TRUE;
          } else {
            ix++;
          }
        }
      }

      if (ixstart < i) {
        while (ixstart + 1 <= i) {
          if (Box_data[ixstart] < rl) {
            rl = Box_data[ixstart];
          }

          ixstart++;
        }
      }

      rt = (rl - 1.0F) - dtr;

      /* 1 more pixel for bilinear filtering             */
      marginL = 0U;
      if (rt < -dmax) {
        /* clipping */
        marginL = (uint32_T)rt_roundf_snf((-dmax - rt) + 1.0F);

        /* +1 = ceil */
        rt = -dmax;
      }

      ixstart = 1;
      rl = Box_data[0];
      if (rtIsNaNF(Box_data[0])) {
        ix = 2;
        exitg3 = FALSE;
        while ((exitg3 == 0U) && (ix <= i)) {
          ixstart = ix;
          if (!rtIsNaNF(Box_data[ix - 1])) {
            rl = Box_data[ix - 1];
            exitg3 = TRUE;
          } else {
            ix++;
          }
        }
      }

      if (ixstart < i) {
        while (ixstart + 1 <= i) {
          if (Box_data[ixstart] > rl) {
            rl = Box_data[ixstart];
          }

          ixstart++;
        }
      }

      rr = (rl + 1.0F) + dtr;

      /* 1 more pixel for bilinear filtering */
      marginR = 0U;
      if (rr > y - 0.5F) {
        /* clipping */
        marginR = (uint32_T)rt_roundf_snf((rr - (y - 0.5F)) + 1.0F);

        /* +1 = ceil */
        rr = y - 0.5F;
      }

      /*  Y Bounding Box ------------------------------- */
      i = (int32_T)((uint32_T)angle_size << 2);
      ixstart = 1;
      rl = Box_data[Box_size_idx_0];
      if (rtIsNaNF(Box_data[Box_size_idx_0])) {
        ix = 2;
        exitg2 = FALSE;
        while ((exitg2 == 0U) && (ix <= i)) {
          ixstart = ix;
          if (!rtIsNaNF(Box_data[(ix + Box_size_idx_0) - 1])) {
            rl = Box_data[(ix + Box_size_idx_0) - 1];
            exitg2 = TRUE;
          } else {
            ix++;
          }
        }
      }

      if (ixstart < i) {
        while (ixstart + 1 <= i) {
          if (Box_data[ixstart + Box_size_idx_0] < rl) {
            rl = Box_data[ixstart + Box_size_idx_0];
          }

          ixstart++;
        }
      }

      rb = (rl - 1.0F) - dtr;

      /* 1 more pixel for bilinear filtering; */
      marginO = 0U;
      if (rb < -dmax) {
        /* clipping */
        marginO = (uint32_T)rt_roundf_snf((-dmax - rb) + 1.0F);

        /* +1 = ceil */
        rb = -dmax;
      }

      ixstart = 1;
      rl = Box_data[Box_size_idx_0];
      if (rtIsNaNF(Box_data[Box_size_idx_0])) {
        ix = 2;
        exitg1 = FALSE;
        while ((exitg1 == 0U) && (ix <= i)) {
          ixstart = ix;
          if (!rtIsNaNF(Box_data[(ix + Box_size_idx_0) - 1])) {
            rl = Box_data[(ix + Box_size_idx_0) - 1];
            exitg1 = TRUE;
          } else {
            ix++;
          }
        }
      }

      if (ixstart < i) {
        while (ixstart + 1 <= i) {
          if (Box_data[ixstart + Box_size_idx_0] > rl) {
            rl = Box_data[ixstart + Box_size_idx_0];
          }

          ixstart++;
        }
      }

      rl = (rl + 1.0F) + dtr;

      /* 1 more pixel for bilinear interpolation; */
      marginU = 0U;
      if (rl > y - 0.5F) {
        /* clipping */
        marginU = (uint32_T)rt_roundf_snf((rl - (y - 0.5F)) + 1.0F);

        /* +1 = ceil */
        rl = y - 0.5F;
      }

      /*  Extract img -------------------------------              */
      BoundBox[0] = (uint32_T)rt_roundf_snf((real32_T)floor(rt + (b_y + 0.5F)));
      BoundBox[1] = (uint32_T)rt_roundf_snf((real32_T)ceil(rr + (b_y + 0.5F)));
      BoundBox[2] = (uint32_T)rt_roundf_snf((real32_T)floor(rb + (b_y + 0.5F)));
      BoundBox[3] = (uint32_T)rt_roundf_snf((real32_T)ceil(rl + (b_y + 0.5F)));

      /* Keep only the biggest margin of left and right. (We share both */
      /* margins on the right with one exception, we add one times the left */
      /* margin to the space between the top margin and the data start.) */
      if (marginL > marginR) {
        marginR = marginL;
      }

      /* Return R */
      for (ixstart = 0; ixstart < 4; ixstart++) {
        b_DSPResponsibilityBox[ixstart] = (uint32_T)rt_roundf_snf
          (DSPResponsibilityBox[ixstart] + s);
      }

	  /* Keep only the biggest margin of left and right. (We share both */
	  /* margins on the right with one exception, we add one times the left */
	  /* margin to the space between the top margin and the data start.) */
	  if (marginL > marginR) {
		marginR = marginL;
	  }

	  MarginAddon[0] = marginR;
	  MarginAddon[1] = marginO + 1U;
	  MarginAddon[2] = marginU;

	  /* We add 1 to the upper margin because of the reusal of the right and */
	  /* left margin. It would be sufficient to add marginL padding bytes to */
	  /* the top, however adding a whole row is more convenient as the */
	  /* resulting data buffer remains a square picture and the loss of a few */
	  /* KB ram (in a cache-uninteresting area) is neglectible. */

	  /* Matlab coder compilation: Start sendingall data to target needed */
      /* for being able to call function 'jacobian' on the target. */
      /* (Sending will be done async/nonblocking in the background). */

	  //Prepare HPRPC store image command header.
	  uint32_T TSinglePartLen = getImagePartSize(BoundBox);
	  uint32_T RSinglePartLen = getImagePartSize(b_DSPResponsibilityBox);

	  CHPRPCRequestStoreImg *pRequest = matlab_c_prepareSendToTarget(
					 b_i, BoundBox, MarginAddon, DSPResponsibilityBox,
					 /*TSinglePart,*/ TSinglePartLen, /*RSinglePart,*/
					 RSinglePartLen, d, levels
					 );

	  double dBeforeExtraction = get_wall_time();

	  //Start 1 thread per DSP for image extraction (directly into the send buffers which might allready be in non-paged kernel memory)
	  TThreadData* pThreadData = &g_ThreadData[b_i-1];
	  //pThreadData->TSinglePart = TSinglePart;
	  pThreadData->Tvec = &Tvec->data[0];
	  memcpy(pThreadData->BoundBox, BoundBox, 4*sizeof(uint32_T));
	  pThreadData->d = d;
	  //pThreadData->RSinglePart = RSinglePart;
	  pThreadData->Rvec = &Rvec->data[0];
	  memcpy(pThreadData->b_DSPResponsibilityBox, b_DSPResponsibilityBox, 4*sizeof(uint32_T));
	  pThreadData->b_i = b_i;
	  pThreadData->pRequest = pRequest;

	  SpawnExtractAndSendThread(pThreadData, (b_i-1));
	
      b_i++;
    }

    x++;
  }

  double TimeForAddMarginsTotal=0;
  double TimeForPyramidTotal=0;
  b_i = 1U;
  for (x = 0; x <= (int32_T)(real32_T)aWidth - 1; x++) {
    for (c_y = 0; c_y <= (int32_T)(real32_T)aHeight - 1; c_y++) {
      /* Matlab coder compilation: Wait until every DSP confirmed data */
      /* transmission. */
	  double TimeForAddMargins, TimeForPyramid;
      waitUntilTargetReady(b_i, TimeForAddMargins, TimeForPyramid);
	  TimeForAddMarginsTotal+=TimeForAddMargins;
	  TimeForPyramidTotal+=TimeForPyramid;

	  b_i++;
    }
  }

  //Performance measurement:

  //Time when the DSPs responded about having received (and processed) the image data and are ready for calculation
  Timestamps.FinishedXtrct.measureWallAndCPUTime();

  //Time when the CPU finished extracting the partial image data
  Timestamps.XtrctPartFinished.setWallClockTime(g_lastImageExtractor->ExtractFinishedWCTime);
  Timestamps.XtrctPartFinished.setCPUTime(g_lastImageExtractor->ExtractFinishedCPUTime);

  //Time when the CPU finished regarding the data sending (below the latest value is determined)
  Timestamps.SendingStartedSystemNowIdle.setWallClockTime(g_lastPCIeSender->SndStartdWCTime);
  Timestamps.SendingStartedSystemNowIdle.setCPUTime(g_lastPCIeSender->SndStartdCPUTime);	

  //Average times from target
  Timestamps.TimeForAddMargins = TimeForAddMarginsTotal / DSPCount;
  Timestamps.TimeForPyramidTotal = TimeForPyramidTotal / DSPCount;

  /* Datenreduktion etwa auf folgenden Wert (ist / max) */
  /* dbgSizDat_Transmitted_Data = dbgSizDat / (d*d*4) */
}

/* End of code generation (transmitImageData.cpp) */
