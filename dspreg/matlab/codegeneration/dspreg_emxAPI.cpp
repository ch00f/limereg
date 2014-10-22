/* =====================================
=== LIMEREG - Lightweight Image Registration ===
========================================

Forked from the project FIMREG, which was written for a distributed calculation on the PCIe card DSPC-8681 of Advantech. LIMEREG does not use DSPs and can
be run on an ordinary PC without special hardware. FIMREG was originally developed by by Roelof Berg, Berg Solutions (rberg@berg-solutions.de) with support
from Lars Koenig, Fraunhofer MEVIS (lars.koenig@mevis.fraunhofer.de) and Jan Ruehaak, Fraunhofer MEVIS (jan.ruehaak@mevis.fraunhofer.de).

THIS IS A LIMITED RESEARCH PROTOTYPE. Documentation: www.berg-solutions.de/limereg.html

------------------------------------------------------------------------------

Copyright (c) 2014, Roelof Berg
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
 * dspreg_emxAPI.cpp
 *
 * CODE GENERATED BY MATLAB CODER (THE HUMAN READABILITY IS THEREFORE LIMITED)
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
#include "dspreg_emxAPI.h"
#include "dspreg_emxutil.h"

/* Custom Source Code */
#include "pseudo_stdafx.h"             //precompiled header not possible because of include position of matlab

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */
emxArray_char_T *emxCreateND_char_T(int32_T numDimensions, int32_T *size)
{
  emxArray_char_T *emx;
  int32_T numEl;
  int32_T loop_ub;
  int32_T i;
  emxInit_char_T(&emx, numDimensions);
  numEl = 1;
  loop_ub = numDimensions - 1;
  for (i = 0; i <= loop_ub; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = (char_T *)calloc((uint32_T)numEl, sizeof(char_T));
  emx->numDimensions = numDimensions;
  emx->allocatedSize = numEl;
  return emx;
}

emxArray_real32_T *emxCreateND_real32_T(int32_T numDimensions, int32_T *size)
{
  emxArray_real32_T *emx;
  int32_T numEl;
  int32_T loop_ub;
  int32_T i;
  d_emxInit_real32_T(&emx, numDimensions);
  numEl = 1;
  loop_ub = numDimensions - 1;
  for (i = 0; i <= loop_ub; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = (real32_T *)calloc((uint32_T)numEl, sizeof(real32_T));
  emx->numDimensions = numDimensions;
  emx->allocatedSize = numEl;
  return emx;
}

emxArray_uint32_T *emxCreateND_uint32_T(int32_T numDimensions, int32_T *size)
{
  emxArray_uint32_T *emx;
  int32_T numEl;
  int32_T loop_ub;
  int32_T i;
  c_emxInit_uint32_T(&emx, numDimensions);
  numEl = 1;
  loop_ub = numDimensions - 1;
  for (i = 0; i <= loop_ub; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = (uint32_T *)calloc((uint32_T)numEl, sizeof(uint32_T));
  emx->numDimensions = numDimensions;
  emx->allocatedSize = numEl;
  return emx;
}

emxArray_uint8_T *emxCreateND_uint8_T(int32_T numDimensions, int32_T *size)
{
  emxArray_uint8_T *emx;
  int32_T numEl;
  int32_T loop_ub;
  int32_T i;
  c_emxInit_uint8_T(&emx, numDimensions);
  numEl = 1;
  loop_ub = numDimensions - 1;
  for (i = 0; i <= loop_ub; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = (uint8_T *)calloc((uint32_T)numEl, sizeof(uint8_T));
  emx->numDimensions = numDimensions;
  emx->allocatedSize = numEl;
  return emx;
}

emxArray_char_T *emxCreateWrapperND_char_T(char_T *data, int32_T numDimensions,
  int32_T *size)
{
  emxArray_char_T *emx;
  int32_T numEl;
  int32_T loop_ub;
  int32_T i;
  emxInit_char_T(&emx, numDimensions);
  numEl = 1;
  loop_ub = numDimensions - 1;
  for (i = 0; i <= loop_ub; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = data;
  emx->numDimensions = numDimensions;
  emx->allocatedSize = numEl;
  emx->canFreeData = FALSE;
  return emx;
}

emxArray_real32_T *emxCreateWrapperND_real32_T(real32_T *data, int32_T
  numDimensions, int32_T *size)
{
  emxArray_real32_T *emx;
  int32_T numEl;
  int32_T loop_ub;
  int32_T i;
  d_emxInit_real32_T(&emx, numDimensions);
  numEl = 1;
  loop_ub = numDimensions - 1;
  for (i = 0; i <= loop_ub; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = data;
  emx->numDimensions = numDimensions;
  emx->allocatedSize = numEl;
  emx->canFreeData = FALSE;
  return emx;
}

emxArray_uint32_T *emxCreateWrapperND_uint32_T(uint32_T *data, int32_T
  numDimensions, int32_T *size)
{
  emxArray_uint32_T *emx;
  int32_T numEl;
  int32_T loop_ub;
  int32_T i;
  c_emxInit_uint32_T(&emx, numDimensions);
  numEl = 1;
  loop_ub = numDimensions - 1;
  for (i = 0; i <= loop_ub; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = data;
  emx->numDimensions = numDimensions;
  emx->allocatedSize = numEl;
  emx->canFreeData = FALSE;
  return emx;
}

emxArray_uint8_T *emxCreateWrapperND_uint8_T(uint8_T *data, int32_T
  numDimensions, int32_T *size)
{
  emxArray_uint8_T *emx;
  int32_T numEl;
  int32_T loop_ub;
  int32_T i;
  c_emxInit_uint8_T(&emx, numDimensions);
  numEl = 1;
  loop_ub = numDimensions - 1;
  for (i = 0; i <= loop_ub; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = data;
  emx->numDimensions = numDimensions;
  emx->allocatedSize = numEl;
  emx->canFreeData = FALSE;
  return emx;
}

emxArray_char_T *emxCreateWrapper_char_T(char_T *data, int32_T rows, int32_T
  cols)
{
  emxArray_char_T *emx;
  int32_T size[2];
  int32_T numEl;
  int32_T i;
  size[0] = rows;
  size[1] = cols;
  emxInit_char_T(&emx, 2);
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = data;
  emx->numDimensions = 2;
  emx->allocatedSize = numEl;
  emx->canFreeData = FALSE;
  return emx;
}

emxArray_real32_T *emxCreateWrapper_real32_T(real32_T *data, int32_T rows,
  int32_T cols)
{
  emxArray_real32_T *emx;
  int32_T size[2];
  int32_T numEl;
  int32_T i;
  size[0] = rows;
  size[1] = cols;
  d_emxInit_real32_T(&emx, 2);
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = data;
  emx->numDimensions = 2;
  emx->allocatedSize = numEl;
  emx->canFreeData = FALSE;
  return emx;
}

emxArray_uint32_T *emxCreateWrapper_uint32_T(uint32_T *data, int32_T rows,
  int32_T cols)
{
  emxArray_uint32_T *emx;
  int32_T size[2];
  int32_T numEl;
  int32_T i;
  size[0] = rows;
  size[1] = cols;
  c_emxInit_uint32_T(&emx, 2);
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = data;
  emx->numDimensions = 2;
  emx->allocatedSize = numEl;
  emx->canFreeData = FALSE;
  return emx;
}

emxArray_uint8_T *emxCreateWrapper_uint8_T(uint8_T *data, int32_T rows, int32_T
  cols)
{
  emxArray_uint8_T *emx;
  int32_T size[2];
  int32_T numEl;
  int32_T i;
  size[0] = rows;
  size[1] = cols;
  c_emxInit_uint8_T(&emx, 2);
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = data;
  emx->numDimensions = 2;
  emx->allocatedSize = numEl;
  emx->canFreeData = FALSE;
  return emx;
}

emxArray_char_T *emxCreate_char_T(int32_T rows, int32_T cols)
{
  emxArray_char_T *emx;
  int32_T size[2];
  int32_T numEl;
  int32_T i;
  size[0] = rows;
  size[1] = cols;
  emxInit_char_T(&emx, 2);
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = (char_T *)calloc((uint32_T)numEl, sizeof(char_T));
  emx->numDimensions = 2;
  emx->allocatedSize = numEl;
  return emx;
}

emxArray_real32_T *emxCreate_real32_T(int32_T rows, int32_T cols)
{
  emxArray_real32_T *emx;
  int32_T size[2];
  int32_T numEl;
  int32_T i;
  size[0] = rows;
  size[1] = cols;
  d_emxInit_real32_T(&emx, 2);
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = (real32_T *)calloc((uint32_T)numEl, sizeof(real32_T));
  emx->numDimensions = 2;
  emx->allocatedSize = numEl;
  return emx;
}

emxArray_uint32_T *emxCreate_uint32_T(int32_T rows, int32_T cols)
{
  emxArray_uint32_T *emx;
  int32_T size[2];
  int32_T numEl;
  int32_T i;
  size[0] = rows;
  size[1] = cols;
  c_emxInit_uint32_T(&emx, 2);
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = (uint32_T *)calloc((uint32_T)numEl, sizeof(uint32_T));
  emx->numDimensions = 2;
  emx->allocatedSize = numEl;
  return emx;
}

emxArray_uint8_T *emxCreate_uint8_T(int32_T rows, int32_T cols)
{
  emxArray_uint8_T *emx;
  int32_T size[2];
  int32_T numEl;
  int32_T i;
  size[0] = rows;
  size[1] = cols;
  c_emxInit_uint8_T(&emx, 2);
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = (uint8_T *)calloc((uint32_T)numEl, sizeof(uint8_T));
  emx->numDimensions = 2;
  emx->allocatedSize = numEl;
  return emx;
}

void emxDestroyArray_char_T(emxArray_char_T *emxArray)
{
  emxFree_char_T(&emxArray);
}

void emxDestroyArray_real32_T(emxArray_real32_T *emxArray)
{
  emxFree_real32_T(&emxArray);
}

void emxDestroyArray_uint32_T(emxArray_uint32_T *emxArray)
{
  emxFree_uint32_T(&emxArray);
}

void emxDestroyArray_uint8_T(emxArray_uint8_T *emxArray)
{
  emxFree_uint8_T(&emxArray);
}

/* End of code generation (dspreg_emxAPI.cpp) */
