/* =====================================
=== FIMREG - Fast Image Registration ===
========================================

Written by Roelof Berg, Berg Solutions (rberg@berg-solutions.de) with support from
Lars Koenig, Fraunhofer MEVIS (lars.koenig@mevis.fraunhofer.de) Jan Ruehaak, Fraunhofer
MEVIS (jan.ruehaak@mevis.fraunhofer.de).

THIS IS A LIMITED RESEARCH PROTOTYPE. Documentation: www.berg-solutions.de/fimreg.html

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

#pragma once

#include "dspreg_common.h"
class CHPRPCConnection;
class CBufferedWriter;

/**
* Abstract baseclass for a HPRPC command
*/
class CHPRPCCommand
{
public:
	CHPRPCCommand(CHPRPCConnection& HPRPCConnection);
	virtual ~CHPRPCCommand(void);

	void StartSendToOtherPeer(
		#if defined(_TRACE_HPRPC_)
		const string& CommandLogString
		#endif
		);
	void ReceiveResponse(uint8_t*& Buffer, uint32_t& Bufferlen
		#if defined(_TRACE_HPRPC_)
		,const string& CommandLogString
		#endif
		);
	virtual uint16_t GetCallIndex()=0;

	CHPRPCConnection& GetHPRPCConnection();

protected:
	/** Abstract interface method. For documentation see baseclass implementations. */
	/*
	uint8_t* AllocateBuffer(const uint32_t BufferLen);
	void FreeBuffers();
	*/
	uint8_t* GetPayloadHeaderPtr();
	void SetPayloadHeaderPtr(uint8_t* PayloadHeaderPtr);
	CBufferedWriter& GetBufferedWriter();

private:
	uint8_t* mpPayloadHeader;
	CHPRPCConnection& m_HPRPCConnection;
};
