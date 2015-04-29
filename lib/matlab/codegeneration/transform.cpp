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
 * transform.cpp
 *
 * CODE GENERATED BY MATLAB CODER (THE HUMAN READABILITY IS THEREFORE LIMITED)
 *
 */

#include "../../stdafx.h"

/* Include files */
#include "rt_nonfinite.h"
#include "diffimg.h"
#include "gaussnewton.h"
#include "generatePyramidPC.h"
#include "jacobian.h"
#include "ssd.h"
#include "transform.h"
#include "limereg_emxutil.h"
#include "limereg_rtwutil.h"

/* Custom Source Code */
#include "../pseudo_stdafx.h"             //precompiled header not possible because of include position of matlab
namespace Limereg {


/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */

/* Function Definitions */
void transform(const real64_T w[3], const emxArray_uint8_T *Tvec, uint32_T dx, uint32_T dy,
               emxArray_uint8_T *FTvec)
{
  uint32_T mn;
  real64_T y;
  real64_T dmax;
  real64_T b_y;
  real64_T FP_idx_0;
  real64_T FP_idx_1;
  real64_T FP_idx_3;
  real64_T FP_idx_4;
  int32_T i33;
  int32_T loop_ub;
  real64_T X_mni;
  int32_T i34;
  int32_T X_i;
  real64_T FA_mni;
  real64_T FA_i;
  uint32_T Ax;
  uint32_T Ay;
  int32_T k11;
  int32_T k12;
  int32_T k21;
  int32_T k22;

/* RBE MOVE ON HERE FOR X, Y SPLIT UP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
  uint32_T d = dx;

  mn = d * d;
  y = (real64_T)d / 2.0F;
  dmax = (real64_T)d / 2.0F - 0.5F;
  b_y = (real64_T)d / 2.0F;

  /* Shifting, um negative Koordinaten in Matrixbereich zu bringen */
  /* ======================================================================== */
  /* = Registerweise statt matrizenbasiert */
  /* ======================================================================== */
  FP_idx_0 = (real64_T)cos(w[0]);
  FP_idx_1 = -(real64_T)sin(w[0]);
  FP_idx_3 = (real64_T)sin(w[0]);
  FP_idx_4 = (real64_T)cos(w[0]);
  i33 = FTvec->size[0];
  FTvec->size[0] = (int32_T)mn;
  emxEnsureCapacity((emxArray__common *)FTvec, i33, (int32_T)sizeof(uint8_T));
  loop_ub = (int32_T)mn - 1;
  for (i33 = 0; i33 <= loop_ub; i33++) {
    FTvec->data[i33] = 0;
  }

  mn = 1U;

  /* Folgende beiden For-Loops laufen 1..mn mal durch das pixelmittige Koordinatengitter */
  /* Dabei zeigt i auf die Position von 1..mn */
  i33 = (int32_T)((y - 0.5F) + (1.0F - (-dmax)));
  for (loop_ub = 0; loop_ub <= i33 - 1; loop_ub++) {
    X_mni = -dmax + (real64_T)loop_ub;
    i34 = (int32_T)((y - 0.5F) + (1.0F - (-dmax)));
    for (X_i = 0; X_i <= i34 - 1; X_i++) {
      FA_mni = -dmax + (real64_T)X_i;

      /* Die Zeilenvektoren FP(1)*X_i und FP(4)*X_i k�nnten vorberechnet */
      /* werden (mit FP(3) und FP(6), sowie +s schon mit drin ... */
      FA_i = ((FP_idx_0 * FA_mni + FP_idx_1 * X_mni) + w[1]) + (b_y + 0.5F);
      FA_mni = ((FP_idx_3 * FA_mni + FP_idx_4 * X_mni) + w[2]) + (b_y + 0.5F);

      /* +s weil Drehpunkt in Bildmitte */
      Ax = (uint32_T)rt_roundf_snf((real64_T)floor(FA_i));
      Ay = (uint32_T)rt_roundf_snf((real64_T)floor(FA_mni));

      /* Fetch picture values (how can we cache-optimize this ? Intelligent routing ?) */
      k11 = 0;
      k12 = 0;
      k21 = 0;
      k22 = 0;
      if ((Ax >= 1U) && (Ax < d) && (Ay >= 1U) && (Ay < d)) {
        k11 = Tvec->data[(int32_T)((Ay - 1U) * d + Ax) - 1];
        k12 = Tvec->data[(int32_T)((Ay - 1U) * d + Ax)];
        k21 = Tvec->data[(int32_T)(Ay * d + Ax) - 1];
        k22 = Tvec->data[(int32_T)(Ay * d + Ax)];
      }

      /* Interpolation */
      FA_i -= (real64_T)floor(FA_i);
      FA_mni -= (real64_T)floor(FA_mni);
      FTvec->data[(int32_T)mn - 1] = (uint8_T)rt_roundf_snf((((real64_T)k11 *
        (1.0F - FA_i) * (1.0F - FA_mni) + (real64_T)k12 * FA_i * (1.0F - FA_mni))
        + (real64_T)k21 * (1.0F - FA_i) * FA_mni) + (real64_T)k22 * FA_i *
        FA_mni);
      mn++;
    }
  }
}

}
/* End of code generation (transform.cpp) */
