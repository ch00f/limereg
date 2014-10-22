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
 * dstr_jacobian.cpp
 *
 * Code generation for function 'dstr_jacobian'
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
#include "dstr_jacobian.h"
#include "dspreg_emxutil.h"

/* Custom Source Code */
#include "pseudo_stdafx.h"             //precompiled header not possible because of include position of matlab

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */
void dstr_jacobian(uint32_T DSPCount, const real32_T w[3], uint32_T Level, real32_T *SSD,
                   real32_T JD[3], real32_T JD2[9])
{
  emxArray_real32_T *SSDp;
  uint32_T aHeight;
  uint32_T aWidth;
  int32_T k;
  int32_T vlen;
  emxArray_real32_T *JDp;
  emxArray_real32_T *JD2p;
  uint32_T i;
  uint32_T x;
  uint32_T y;
  real32_T fv0[9];
  real32_T fv1[3];
  real32_T b_JD;
  emxInit_real32_T(&SSDp, 1);

  /* Global definition has only effect in matlab simulation (coder.extrinsic forbids global use here in matlab coder) */
  /* coder.extrinsic('global');  %this line unfortunately doesn't work */
  /* global GlobTParts GlobTPartsPtrs GlobRParts GlobRPartsPtrs GlobDSPResponsibilityBox GlobBoundBox GlobImgDimension; */
  /* Calc layout of DSP grid regarding the Reference image */
  calcDSPLayout(DSPCount, &aWidth, &aHeight);
  k = SSDp->size[0];
  SSDp->size[0] = (int32_T)DSPCount;
  emxEnsureCapacity((emxArray__common *)SSDp, k, (int32_T)sizeof(real32_T));
  vlen = (int32_T)DSPCount - 1;
  for (k = 0; k <= vlen; k++) {
    SSDp->data[k] = 0.0F;
  }

  c_emxInit_real32_T(&JDp, 3);
  k = JDp->size[0] * JDp->size[1] * JDp->size[2];
  JDp->size[0] = 1;
  JDp->size[1] = 3;
  JDp->size[2] = (int32_T)DSPCount;
  emxEnsureCapacity((emxArray__common *)JDp, k, (int32_T)sizeof(real32_T));
  vlen = 3 * (int32_T)DSPCount - 1;
  for (k = 0; k <= vlen; k++) {
    JDp->data[k] = 0.0F;
  }

  c_emxInit_real32_T(&JD2p, 3);
  k = JD2p->size[0] * JD2p->size[1] * JD2p->size[2];
  JD2p->size[0] = 3;
  JD2p->size[1] = 3;
  JD2p->size[2] = (int32_T)DSPCount;
  emxEnsureCapacity((emxArray__common *)JD2p, k, (int32_T)sizeof(real32_T));
  vlen = 9 * (int32_T)DSPCount - 1;
  for (k = 0; k <= vlen; k++) {
    JD2p->data[k] = 0.0F;
  }

  /* Matlab coder compilation: Start calculation nonblocking on all DSPs in */
  /* parallel, then the results can be taken below in a blocking way. */
  i = 1U;
  for (x = 1U; x <= aWidth; x++) {
    for (y = 1U; y <= aHeight; y++) {
      start_jacobianOnTarget(i, w, Level);
      i++;
    }
  }

  i = 1U;
  for (x = 1U; x <= aWidth; x++) {
    for (y = 1U; y <= aHeight; y++) {
      /* calculate distributed (happens in parallel on the DSPs, but serialized in matlab) */
      /* Matlab coder compilation: Fetch calculation result from DSP */
      /* (Blocking is ok here as the calculation was allready started */
      /* above in a nonblocking way and all DSPs spend equal time for */
      /* calculation and just fetching the data needs not much time.) */
      jacobianOnTarget(i, &SSDp->data[(int32_T)i - 1], fv1, fv0);
      for (k = 0; k < 3; k++) {
        for (vlen = 0; vlen < 3; vlen++) {
          JD2p->data[(vlen + JD2p->size[0] * k) + JD2p->size[0] * JD2p->size[1] *
            ((int32_T)i - 1)] = fv0[vlen + 3 * k];
        }
      }

      for (k = 0; k < 3; k++) {
        JDp->data[JDp->size[0] * k + JDp->size[0] * JDp->size[1] * ((int32_T)i -
          1)] = fv1[k];
      }

      i++;
    }
  }

  /* Sum up all partial results */
  if (SSDp->size[0] == 0) {
    *SSD = 0.0F;
  } else {
    vlen = SSDp->size[0];
    *SSD = SSDp->data[0];
    for (k = 2; k <= vlen; k++) {
      *SSD += SSDp->data[k - 1];
    }
  }

  emxFree_real32_T(&SSDp);
  for (k = 0; k < 3; k++) {
    JD[k] = 0.0F;
  }

  for (k = 0; k < 9; k++) {
    JD2[k] = 0.0F;
  }

  for (aWidth = 1U; aWidth <= DSPCount; aWidth++) {
    for (k = 0; k < 3; k++) {
      b_JD = JD[k] + JDp->data[JDp->size[0] * k + JDp->size[0] * JDp->size[1] *
        ((int32_T)aWidth - 1)];
      JD[k] = b_JD;
    }

    for (k = 0; k < 3; k++) {
      for (vlen = 0; vlen < 3; vlen++) {
        JD2[vlen + 3 * k] += JD2p->data[(vlen + JD2p->size[0] * k) + JD2p->size
          [0] * JD2p->size[1] * ((int32_T)aWidth - 1)];
      }
    }
  }

  emxFree_real32_T(&JD2p);
  emxFree_real32_T(&JDp);
}

/* End of code generation (dstr_jacobian.cpp) */
