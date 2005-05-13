/*	$Id: bin_arith.h,v 1.2 2003/12/11 15:06:50 monaka Exp $	*/

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

#ifndef	IA32_CPU_INSTRUCTION_BIN_ARITH_H__
#define	IA32_CPU_INSTRUCTION_BIN_ARITH_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ADD */
void ADD_EbGb(void);
void ADD_EwGw(void);
void ADD_EdGd(void);
void ADD_GbEb(void);
void ADD_GwEw(void);
void ADD_GdEd(void);
void ADD_ALIb(void);
void ADD_AXIw(void);
void ADD_EAXId(void);
void ADD_EbIb(BYTE *, DWORD);
void ADD_EwIx(WORD *, DWORD);
void ADD_EdIx(DWORD *, DWORD);
void ADD_EbIb_ext(DWORD, DWORD);
void ADD_EwIx_ext(DWORD, DWORD);
void ADD_EdIx_ext(DWORD, DWORD);

/* ADC */
void ADC_EbGb(void);
void ADC_EwGw(void);
void ADC_EdGd(void);
void ADC_GbEb(void);
void ADC_GwEw(void);
void ADC_GdEd(void);
void ADC_ALIb(void);
void ADC_AXIw(void);
void ADC_EAXId(void);
void ADC_EbIb(BYTE *, DWORD);
void ADC_EwIx(WORD *, DWORD);
void ADC_EdIx(DWORD *, DWORD);
void ADC_EbIb_ext(DWORD, DWORD);
void ADC_EwIx_ext(DWORD, DWORD);
void ADC_EdIx_ext(DWORD, DWORD);

/* SUB */
void SUB_EbGb(void);
void SUB_EwGw(void);
void SUB_EdGd(void);
void SUB_GbEb(void);
void SUB_GwEw(void);
void SUB_GdEd(void);
void SUB_ALIb(void);
void SUB_AXIw(void);
void SUB_EAXId(void);
void SUB_EbIb(BYTE *, DWORD);
void SUB_EwIx(WORD *, DWORD);
void SUB_EdIx(DWORD *, DWORD);
void SUB_EbIb_ext(DWORD, DWORD);
void SUB_EwIx_ext(DWORD, DWORD);
void SUB_EdIx_ext(DWORD, DWORD);

/* SBB */
void SBB_EbGb(void);
void SBB_EwGw(void);
void SBB_EdGd(void);
void SBB_GbEb(void);
void SBB_GwEw(void);
void SBB_GdEd(void);
void SBB_ALIb(void);
void SBB_AXIw(void);
void SBB_EAXId(void);
void SBB_EbIb(BYTE *, DWORD);
void SBB_EwIx(WORD *, DWORD);
void SBB_EdIx(DWORD *, DWORD);
void SBB_EbIb_ext(DWORD, DWORD);
void SBB_EwIx_ext(DWORD, DWORD);
void SBB_EdIx_ext(DWORD, DWORD);

/* IMUL */
void IMUL_ALEb(DWORD op);
void IMUL_AXEw(DWORD op);
void IMUL_EAXEd(DWORD op);
void IMUL_GwEw(void);
void IMUL_GdEd(void);
void IMUL_GwEwIb(void);
void IMUL_GdEdIb(void);
void IMUL_GwEwIw(void);
void IMUL_GdEdId(void);

/* MUL */
void MUL_ALEb(DWORD op);
void MUL_AXEw(DWORD op);
void MUL_EAXEd(DWORD op);

/* IDIV */
void IDIV_ALEb(DWORD op);
void IDIV_AXEw(DWORD op);
void IDIV_EAXEd(DWORD op);

/* DIV */
void DIV_ALEb(DWORD op);
void DIV_AXEw(DWORD op);
void DIV_EAXEd(DWORD op);

/* INC */
void INC_Eb(DWORD op);
void INC_Ew(DWORD op);
void INC_Ed(DWORD op);
void INC_AX(void);
void INC_CX(void);
void INC_DX(void);
void INC_BX(void);
void INC_SP(void);
void INC_BP(void);
void INC_SI(void);
void INC_DI(void);
void INC_EAX(void);
void INC_ECX(void);
void INC_EDX(void);
void INC_EBX(void);
void INC_ESP(void);
void INC_EBP(void);
void INC_ESI(void);
void INC_EDI(void);

/* DEC */
void DEC_Eb(DWORD op);
void DEC_Ew(DWORD op);
void DEC_Ed(DWORD op);
void DEC_AX(void);
void DEC_CX(void);
void DEC_DX(void);
void DEC_BX(void);
void DEC_SP(void);
void DEC_BP(void);
void DEC_SI(void);
void DEC_DI(void);
void DEC_EAX(void);
void DEC_ECX(void);
void DEC_EDX(void);
void DEC_EBX(void);
void DEC_ESP(void);
void DEC_EBP(void);
void DEC_ESI(void);
void DEC_EDI(void);

/* NEG */
void NEG_Eb(DWORD op);
void NEG_Ew(DWORD op);
void NEG_Ed(DWORD op);

/* CMP */
void CMP_EbGb(void);
void CMP_EwGw(void);
void CMP_EdGd(void);
void CMP_GbEb(void);
void CMP_GwEw(void);
void CMP_GdEd(void);
void CMP_ALIb(void);
void CMP_AXIw(void);
void CMP_EAXId(void);
void CMP_EbIb(BYTE *, DWORD);
void CMP_EwIx(WORD *, DWORD);
void CMP_EdIx(DWORD *, DWORD);
void CMP_EbIb_ext(DWORD, DWORD);
void CMP_EwIx_ext(DWORD, DWORD);
void CMP_EdIx_ext(DWORD, DWORD);

#ifdef __cplusplus
}
#endif

#endif	/* IA32_CPU_INSTRUCTION_BIN_ARITH_H__ */