/*	$Id: ctrl_trans.c,v 1.16 2004/03/22 14:22:26 monaka Exp $	*/

/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"
#include "cpu.h"
#include "ia32.mcr"
#include "ctrlxfer.h"

#include "ctrl_trans.h"


/*
 * JMP
 */
void
JMP_Jb(void)
{
	UINT32 ip;

	CPU_WORKCLOCK(7);
	GET_PCBYTESD(ip);
	ADD_EIP(ip);
	CPU_PREFETCH_CLEAR();
}

void
JMP_Jw(void)
{
	UINT32 ip;

	CPU_WORKCLOCK(7);
	GET_PCWORDS(ip);
	ADD_EIP(ip);
	CPU_PREFETCH_CLEAR();
}

void
JMP_Jd(void)
{
	UINT32 ip;

	CPU_WORKCLOCK(7);
	GET_PCDWORD(ip);
	ADD_EIP(ip);
	CPU_PREFETCH_CLEAR();
}

void
JMP_Ew(UINT32 op)
{
	UINT32 madr;
	UINT32 new_ip;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(7);
		new_ip = *(reg16_b20[op]);
		SET_EIP(new_ip);
	} else {
		CPU_WORKCLOCK(11);
		madr = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		SET_EIP(new_ip);
	}
}

void
JMP_Ed(UINT32 op)
{
	UINT32 madr;
	UINT32 new_ip;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(7);
		new_ip = *(reg32_b20[op]);
		SET_EIP(new_ip);
	} else {
		CPU_WORKCLOCK(11);
		madr = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		SET_EIP(new_ip);
	}
}

void
JMP16_Ap(void)
{
	UINT32 new_ip;
	UINT16 new_cs;

	CPU_WORKCLOCK(11);
	GET_PCWORD(new_ip);
	GET_PCWORD(new_cs);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		JMPfar_pm(new_cs, new_ip);
	}
}

void
JMP32_Ap(void)
{
	UINT32 new_ip;
	UINT16 new_cs;

	CPU_WORKCLOCK(11);
	GET_PCDWORD(new_ip);
	GET_PCWORD(new_cs);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		JMPfar_pm(new_cs, new_ip);
	}
}

void
JMP16_Ep(UINT32 op)
{
	UINT32 madr;
	UINT32 new_ip;
	UINT16 new_cs;

	CPU_WORKCLOCK(11);
	if (op < 0xc0) {
		madr = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		new_cs = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr + 2);
		if (!CPU_STAT_PM || CPU_STAT_VM86) {
			/* Real mode or VM86 mode */
			CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
			SET_EIP(new_ip);
		} else {
			/* Protected mode */
			JMPfar_pm(new_cs, new_ip);
		}
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
JMP32_Ep(UINT32 op)
{
	UINT32 madr;
	UINT32 new_ip;
	UINT16 new_cs;

	CPU_WORKCLOCK(11);
	if (op < 0xc0) {
		madr = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		new_cs = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr + 4);
		if (!CPU_STAT_PM || CPU_STAT_VM86) {
			/* Real mode or VM86 mode */
			CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
			SET_EIP(new_ip);
		} else {
			/* Protected mode */
			JMPfar_pm(new_cs, new_ip);
		}
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

/* jo */
void
JO_Jb(void)
{

	if (CC_NO) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JO_Jw(void)
{

	if (CC_NO) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JO_Jd(void)
{

	if (CC_NO) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jno */
void
JNO_Jb(void)
{

	if (CC_O) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNO_Jw(void)
{

	if (CC_O) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNO_Jd(void)
{

	if (CC_O) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jc */
void
JC_Jb(void)
{

	if (CC_NC) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JC_Jw(void)
{

	if (CC_NC) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JC_Jd(void)
{

	if (CC_NC) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jnc */
void
JNC_Jb(void)
{

	if (CC_C) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}
void
JNC_Jw(void)
{

	if (CC_C) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}
void
JNC_Jd(void)
{

	if (CC_C) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jz */
void
JZ_Jb(void)
{

	if (CC_NZ) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JZ_Jw(void)
{

	if (CC_NZ) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JZ_Jd(void)
{

	if (CC_NZ) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jnz */
void
JNZ_Jb(void)
{

	if (CC_Z) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNZ_Jw(void)
{

	if (CC_Z) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNZ_Jd(void)
{

	if (CC_Z) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jna */
void
JNA_Jb(void)
{

	if (CC_A) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNA_Jw(void)
{

	if (CC_A) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNA_Jd(void)
{

	if (CC_A) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* ja */
void
JA_Jb(void)
{

	if (CC_NA) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JA_Jw(void)
{

	if (CC_NA) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JA_Jd(void)
{

	if (CC_NA) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* js */
void
JS_Jb(void)
{

	if (CC_NS) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JS_Jw(void)
{

	if (CC_NS) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JS_Jd(void)
{

	if (CC_NS) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jns */
void
JNS_Jb(void)
{

	if (CC_S) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNS_Jw(void)
{

	if (CC_S) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNS_Jd(void)
{

	if (CC_S) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jp */
void
JP_Jb(void)
{

	if (CC_NP) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JP_Jw(void)
{

	if (CC_NP) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JP_Jd(void)
{

	if (CC_NP) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jnp */
void
JNP_Jb(void)
{

	if (CC_P) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNP_Jw(void)
{

	if (CC_P) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNP_Jd(void)
{

	if (CC_P) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jl */
void
JL_Jb(void)
{

	if (CC_NL) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JL_Jw(void)
{

	if (CC_NL) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JL_Jd(void)
{

	if (CC_NL) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jnl */
void
JNL_Jb(void)
{

	if (CC_L) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNL_Jw(void)
{

	if (CC_L) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNL_Jd(void)
{

	if (CC_L) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jle */
void
JLE_Jb(void)
{

	if (CC_NLE) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JLE_Jw(void)
{

	if (CC_NLE) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JLE_Jd(void)
{

	if (CC_NLE) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jnle */
void
JNLE_Jb(void)
{

	if (CC_LE) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNLE_Jw(void)
{

	if (CC_LE) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNLE_Jd(void)
{

	if (CC_LE) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jcxz */
void
JeCXZ_Jb(void)
{

	if (!CPU_INST_AS32) {
		if (CPU_CX) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	} else {
		if (CPU_ECX) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	}
}

/*
 * LOOPcc
 */
/* loopne */
void
LOOPNE_Jb(void)
{

	if (!CPU_INST_AS32) {
		CPU_CX--;
		if (CPU_CX == 0 || (CPU_FLAGL & Z_FLAG)) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	} else {
		CPU_ECX--;
		if (CPU_ECX == 0 || (CPU_FLAGL & Z_FLAG)) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	}
}

/* loope */
void
LOOPE_Jb(void)
{

	if (!CPU_INST_AS32) {
		CPU_CX--;
		if (CPU_CX == 0 || !(CPU_FLAGL & Z_FLAG)) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	} else {
		CPU_ECX--;
		if (CPU_ECX == 0 || !(CPU_FLAGL & Z_FLAG)) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	}
}

/* loop */
void
LOOP_Jb(void)
{

	if (!CPU_INST_AS32) {
		CPU_CX--;
		if (CPU_CX == 0) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	} else {
		CPU_ECX--;
		if (CPU_ECX == 0) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	}
}

/*
 * CALL
 */
void
CALL_Aw(void)
{
	SINT32 ip;

	CPU_WORKCLOCK(7);
	GET_PCWORDS(ip);
	PUSH0_16(CPU_IP);
	ADD_EIP(ip);
	CPU_PREFETCH_CLEAR();
}

void
CALL_Ad(void)
{
	UINT32 ip;

	CPU_WORKCLOCK(7);
	GET_PCDWORD(ip);
	PUSH0_32(CPU_EIP);
	ADD_EIP(ip);
	CPU_PREFETCH_CLEAR();
}

void
CALL_Ew(UINT32 op)
{
	UINT32 madr;
	UINT32 new_ip;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(7);
		new_ip = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(11);
		madr = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	PUSH0_16(CPU_IP);
	SET_EIP(new_ip);
}

void
CALL_Ed(UINT32 op)
{
	UINT32 madr;
	UINT32 new_ip;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(7);
		new_ip = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(11);
		madr = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	PUSH0_32(CPU_EIP);
	SET_EIP(new_ip);
}

void
CALL16_Ap(void)
{
	UINT32 new_ip;
	UINT16 new_cs;

	CPU_WORKCLOCK(13);
	GET_PCWORD(new_ip);
	GET_PCWORD(new_cs);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		PUSH0_16(CPU_CS);
		PUSH0_16(CPU_IP);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		CALLfar_pm(new_cs, new_ip);
	}
}

void
CALL32_Ap(void)
{
	UINT32 new_ip;
	UINT16 new_cs;

	CPU_WORKCLOCK(13);
	GET_PCDWORD(new_ip);
	GET_PCWORD(new_cs);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		PUSH0_32(CPU_CS);
		PUSH0_32(CPU_EIP);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		CALLfar_pm(new_cs, new_ip);
	}
}

void
CALL16_Ep(UINT32 op)
{
	UINT32 ad;
	UINT32 new_ip;
	UINT16 new_cs;

	CPU_WORKCLOCK(16);
	if (op < 0xc0) {
		ad = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad);
		new_cs = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad + 2);
		if (!CPU_STAT_PM || CPU_STAT_VM86) {
			/* Real mode or VM86 mode */
			PUSH0_16(CPU_CS);
			PUSH0_16(CPU_IP);

			CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
			SET_EIP(new_ip);
		} else {
			/* Protected mode */
			CALLfar_pm(new_cs, new_ip);
		}
		return;
	} 
	EXCEPTION(UD_EXCEPTION, 0);
}

void
CALL32_Ep(UINT32 op)
{
	UINT32 ad;
	UINT32 new_ip;
	UINT16 new_cs;

	CPU_WORKCLOCK(16);
	if (op < 0xc0) {
		ad = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, ad);
		new_cs = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad + 4);
		if (!CPU_STAT_PM || CPU_STAT_VM86) {
			/* Real mode or VM86 mode */
			PUSH0_32(CPU_CS);
			PUSH0_32(CPU_EIP);

			CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
			SET_EIP(new_ip);
		} else {
			/* Protected mode */
			CALLfar_pm(new_cs, new_ip);
		}
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

/*
 * RET
 */
void
RETnear16(void)
{
	UINT32 new_ip;

	CPU_WORKCLOCK(11);
	POP0_16(new_ip);
	SET_EIP(new_ip);
}

void
RETnear32(void)
{
	UINT32 new_ip;

	CPU_WORKCLOCK(11);
	POP0_32(new_ip);
	SET_EIP(new_ip);
}

void
RETnear16_Iw(void)
{
	UINT32 new_ip;
	UINT16 size;

	CPU_WORKCLOCK(11);
	GET_PCWORD(size);
	POP0_16(new_ip);
	SET_EIP(new_ip);
	if (!CPU_STAT_SS32) {
		CPU_SP += size;
	} else {
		CPU_ESP += size;
	}
}

void
RETnear32_Iw(void)
{
	UINT32 new_ip;
	UINT16 size;

	CPU_WORKCLOCK(11);
	GET_PCWORD(size);
	POP0_32(new_ip);
	SET_EIP(new_ip);
	if (!CPU_STAT_SS32) {
		CPU_SP += size;
	} else {
		CPU_ESP += size;
	}
}

void
RETfar16(void)
{
	UINT32 new_ip;
	UINT16 new_cs;

	CPU_WORKCLOCK(15);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		POP0_16(new_ip);
		POP0_16(new_cs);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		RETfar_pm(0);
	}
}

void
RETfar32(void)
{
	UINT32 new_ip;
	UINT32 new_cs;

	CPU_WORKCLOCK(15);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		POP0_32(new_ip);
		POP0_32(new_cs);

		CPU_SET_SEGREG(CPU_CS_INDEX, (UINT16)new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		RETfar_pm(0);
	}
}

void
RETfar16_Iw(void)
{
	UINT32 new_ip;
	UINT16 new_cs;
	UINT16 size;

	CPU_WORKCLOCK(15);
	GET_PCWORD(size);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		POP0_16(new_ip);
		POP0_16(new_cs);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);

		if (!CPU_STAT_SS32) {
			CPU_SP += size;
		} else {
			CPU_ESP += size;
		}
	} else {
		/* Protected mode */
		RETfar_pm(size);
	}
}

void
RETfar32_Iw(void)
{
	UINT32 new_ip;
	UINT32 new_cs;
	UINT16 size;

	CPU_WORKCLOCK(15);
	GET_PCWORD(size);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		POP0_32(new_ip);
		POP0_32(new_cs);

		CPU_SET_SEGREG(CPU_CS_INDEX, (UINT16)new_cs);
		SET_EIP(new_ip);

		if (!CPU_STAT_SS32) {
			CPU_SP += size;
		} else {
			CPU_ESP += size;
		}
	} else {
		/* Protected mode */
		RETfar_pm(size);
	}
}

void
IRET(void)
{
	UINT32 new_ip;
	UINT32 new_flags;
	UINT32 new_cs;
#if !defined(IA32_DONT_USE_SET_EFLAGS_FUNCTION)
	UINT32 mask;
#endif

	if (!CPU_STAT_PM) {
		/* Real mode */
		CPU_WORKCLOCK(22);
#if defined(IA32_DONT_USE_SET_EFLAGS_FUNCTION)
		if (!CPU_INST_OP32) {
			POP0_16(new_ip);
			POP0_16(new_cs);
			POP0_16(new_flags);
			CPU_FLAG = new_flags & ALL_FLAG;
		} else {
			POP0_32(new_ip);
			POP0_32(new_cs);
			POP0_32(new_flags);
			CPU_EFLAG = (new_flags & (ALL_FLAG|RF_FLAG|AC_FLAG|ID_FLAG)) | (CPU_EFLAG & (VM_FLAG|VIF_FLAG|VIP_FLAG));
		}
		CPU_OV = CPU_FLAG & O_FLAG;
		CPU_TRAP = (CPU_FLAG & (I_FLAG|T_FLAG)) == (I_FLAG|T_FLAG);
#else
		mask = I_FLAG|IOPL_FLAG;
		if (!CPU_INST_OP32) {
			POP0_16(new_ip);
			POP0_16(new_cs);
			POP0_16(new_flags);
		} else {
			POP0_32(new_ip);
			POP0_32(new_cs);
			POP0_32(new_flags);
			mask |= RF_FLAG;
		}

		set_eflags(new_flags, mask);
#endif

		CPU_SET_SEGREG(CPU_CS_INDEX, (UINT16)new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		IRET_pm();
	}
#if defined(IA32_SUPPORT_DEBUG_REGISTER)
	if (CPU_EFLAG & RF_FLAG) {
		CPU_STAT_BP_EVENT |= CPU_STAT_BP_EVENT_RF;
	}
#endif
	IRQCHECKTERM();
}

/*
 * INT
 */
void
INT1(void)
{

	CPU_WORKCLOCK(33);
	INTERRUPT(1, 1, 0, 0);
}

void
INT3(void)
{

	CPU_WORKCLOCK(33);
	INTERRUPT(3, 2, 0, 0);
}

void
INTO(void)
{

	if (!CPU_OV) {
		CPU_WORKCLOCK(3);
		return;
	}
	CPU_WORKCLOCK(35);
	INTERRUPT(4, 3, 0, 0);
}

void
INT_Ib(void)
{
	UINT8 vect;

	CPU_WORKCLOCK(37);
	if (!CPU_STAT_PM || !CPU_STAT_VM86 || (CPU_STAT_IOPL == CPU_IOPL3)) {
		GET_PCBYTE(vect);
		INTERRUPT(vect, -1, 0, 0);
		return;
	}
	EXCEPTION(GP_EXCEPTION, 0);
}

void
BOUND_GwMa(void)
{
	UINT32 op, madr;
	UINT16 reg;
	int vect;

	CPU_WORKCLOCK(13);
	GET_PCBYTE(op);
	if (op < 0xc0) {
		reg = *(reg16_b53[op]);
		madr = calc_ea_dst(op);
		if (reg >= cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr) &&
		    reg <= cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr + 2)) {
				return;
		}
		vect = BR_EXCEPTION;
	} else {
		vect = UD_EXCEPTION;
	}
	EXCEPTION(vect, 0);
}

void
BOUND_GdMa(void)
{
	UINT32 op, madr;
	UINT32 reg;
	int vect;

	CPU_WORKCLOCK(13);
	GET_PCBYTE(op);
	if (op < 0xc0) {
		reg = *(reg32_b53[op]);
		madr = calc_ea_dst(op);
		if (reg >= cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr) &&
		    reg <= cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr + 4)) {
				return;
		}
		vect = BR_EXCEPTION;
	} else {
		vect = UD_EXCEPTION;
	}
	EXCEPTION(vect, 0);
}

/*
 * STACK
 */
void
ENTER16_IwIb(void)
{
	UINT32 sp, bp;
	UINT32 size;
	UINT32 val;
	UINT16 dimsize;
	UINT8 level;

	GET_PCWORD(dimsize);
	GET_PCBYTE(level);
	level &= 0x1f;

	/* check stack room size */
	if (CPU_STAT_PM) {
		size = dimsize + (level + 1) * 2;
		if (!CPU_STAT_SS32) {
			sp = CPU_SP;
		} else {
			sp = CPU_ESP;
		}
		STACK_PUSH_CHECK(CPU_REGS_SREG(CPU_SS_INDEX), &CPU_STAT_SREG(CPU_SS_INDEX), sp, size);
	}

	PUSH0_16(CPU_BP);
	if (level == 0) {			/* enter level=0 */
		CPU_WORKCLOCK(11);
		CPU_BP = CPU_SP;
		if (!CPU_STAT_SS32) {
			CPU_SP -= dimsize;
		} else {
			CPU_ESP -= dimsize;
		}
	} else {
		--level;
		if (level == 0) {		/* enter level=1 */
			CPU_WORKCLOCK(15);
			sp = CPU_SP;
			PUSH0_16(sp);
			CPU_BP = (UINT16)sp;
			if (!CPU_STAT_SS32) {
				CPU_SP -= dimsize;
			} else {
				CPU_ESP -= dimsize;
			}
		} else {			/* enter level=2-31 */
			CPU_WORKCLOCK(12 + level * 4);
			if (!CPU_STAT_SS32) {
				bp = CPU_BP;
				CPU_BP = CPU_SP;
				while (level--) {
					bp -= 2;
					CPU_SP -= 2;
					val = cpu_vmemoryread_w(CPU_SS_INDEX, bp);
					cpu_vmemorywrite_w(CPU_SS_INDEX, CPU_SP, (UINT16)val);
				}
				REGPUSH0(CPU_BP);
				CPU_SP -= dimsize;
			} else {
				bp = CPU_EBP;
				CPU_BP = CPU_SP;
				while (level--) {
					bp -= 2;
					CPU_ESP -= 2;
					val = cpu_vmemoryread_w(CPU_SS_INDEX, bp);
					cpu_vmemorywrite_w(CPU_SS_INDEX, CPU_ESP, (UINT16)val);
				}
				REGPUSH0_16_32(CPU_BP);
				CPU_ESP -= dimsize;
			}
		}
	}
}

void
ENTER32_IwIb(void)
{
	UINT32 sp, bp;
	UINT32 size;
	UINT32 val;
	UINT16 dimsize;
	UINT8 level;

	GET_PCWORD(dimsize);
	GET_PCBYTE(level);
	level &= 0x1f;

	/* check stack room size */
	if (CPU_STAT_PM) {
		size = dimsize + (level + 1) * 4;
		if (CPU_STAT_SS32) {
			sp = CPU_ESP;
		} else {
			sp = CPU_SP;
		}
		STACK_PUSH_CHECK(CPU_REGS_SREG(CPU_SS_INDEX), &CPU_STAT_SREG(CPU_SS_INDEX), sp, size);
	}

	PUSH0_32(CPU_EBP);
	if (level == 0) {			/* enter level=0 */
		CPU_WORKCLOCK(11);
		CPU_EBP = CPU_ESP;
		if (!CPU_STAT_SS32) {
			CPU_SP -= dimsize;
		} else {
			CPU_ESP -= dimsize;
		}
	} else {
		--level;
		if (level == 0) {		/* enter level=1 */
			CPU_WORKCLOCK(15);
			sp = CPU_ESP;
			PUSH0_32(sp);
			CPU_EBP = sp;
			if (CPU_STAT_SS32) {
				CPU_ESP -= dimsize;
			} else {
				CPU_SP -= dimsize;
			}
		} else {			/* enter level=2-31 */
			CPU_WORKCLOCK(12 + level * 4);
			if (CPU_STAT_SS32) {
				bp = CPU_EBP;
				CPU_EBP = CPU_ESP;
				while (level--) {
					bp -= 4;
					CPU_ESP -= 4;
					val = cpu_vmemoryread_d(CPU_SS_INDEX, bp);
					cpu_vmemorywrite_d(CPU_SS_INDEX, CPU_ESP, val);
				}
				REGPUSH0_32(CPU_EBP);
				CPU_ESP -= dimsize;
			} else {
				bp = CPU_BP;
				CPU_EBP = CPU_ESP;
				while (level--) {
					bp -= 4;
					CPU_SP -= 4;
					val = cpu_vmemoryread_d(CPU_SS_INDEX, bp);
					cpu_vmemorywrite_d(CPU_SS_INDEX, CPU_SP, val);
				}
				REGPUSH0_32_16(CPU_EBP);
				CPU_SP -= dimsize;
			}
		}
	}
}

void
LEAVE(void)
{
	UINT32 sp, bp;
	UINT s;

	CPU_WORKCLOCK(4);

	if (CPU_STAT_PM) {
		if (!CPU_INST_OP32) {
			s = 2;
		} else {
			s = 4;
		}
		if (!CPU_STAT_SS32) {
			sp = CPU_SP;
			bp = CPU_BP;
		} else {
			sp = CPU_ESP;
			bp = CPU_EBP;
		}
		STACK_POP_CHECK(CPU_REGS_SREG(CPU_SS_INDEX), &CPU_STAT_SREG(CPU_SS_INDEX), sp, (bp - sp) + s);
	}

	if (!CPU_STAT_SS32) {
		CPU_SP = CPU_BP;
	} else {
		CPU_ESP = CPU_EBP;
	}
	if (!CPU_INST_OP32) {
		REGPOP0(CPU_BP);
	} else {
		POP0_32(CPU_EBP);
	}
}
