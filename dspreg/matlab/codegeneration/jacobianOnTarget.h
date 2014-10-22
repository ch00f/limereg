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
 * jacobianOnTarget.h
 *
 * Code generation for function 'jacobianOnTarget'
 *
 * C source code generated on: Mon Jul 02 10:11:21 2012
 *
 */

#ifndef __JACOBIANONTARGET_H__
#define __JACOBIANONTARGET_H__
/* Include files */
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"

#include "rtwtypes.h"
#include "dspreg_types.h"

/* Type Definitions */

/* Named Constants */

/* Variable Declarations */

/* Variable Definitions */

/* Function Declarations */
extern void jacobianOnTarget(uint32_T DSPIdx, real32_T *SSD, real32_T JD[3], real32_T JD2[9]);
#endif
/* End of code generation (jacobianOnTarget.h) */
