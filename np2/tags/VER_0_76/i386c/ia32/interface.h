/*	$Id: interface.h,v 1.16 2004/03/29 05:50:29 yui Exp $	*/

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

#ifndef	IA32_CPU_INTERFACE_H__
#define	IA32_CPU_INTERFACE_H__

#ifndef CPUCALL
#define	CPUCALL
#endif

#if !defined(QWORD_CONST)
#define	QWORD_CONST(v)	v ## ULL
#define	SQWORD_CONST(v)	v ## LL
#endif

#define CPU_isEI		(CPU_FLAG & I_FLAG)
#define CPU_isDI		(!CPU_isEI)
#define	CPU_A20EN(en)		CPU_ADRSMASK = (en)?0xffffffff:0x000fffff;

#define CPU_INITIALIZE()		i386c_initialize()
#define	CPU_DEINITIALIZE()
#define	CPU_RESET()			ia32reset()
#define	CPU_CLEARPREFETCH()		CPU_PREFETCH_CLEAR()
#define	CPU_INTERRUPT(vect, soft)	ia32_interrupt(vect, soft)
#define	CPU_EXEC()			ia32()
#define	CPU_EXECV30()			ia32()
#define	CPU_SHUT()			ia32shut()
#define	CPU_SETEXTSIZE(size)		ia32_setextsize((UINT32)(size) << 20)
#define CPU_SETEMM(frame, addr)		ia32_setemm(frame, addr)

#define	cpu_memorywrite(a,v)	i286_memorywrite(a,v)
#define	cpu_memorywrite_b(a,v)	i286_memorywrite(a,v)
#define	cpu_memorywrite_w(a,v)	i286_memorywrite_w(a,v)
#define	cpu_memorywrite_d(a,v)	i286_memorywrite_d(a,v)
#define	cpu_memoryread(a)	i286_memoryread(a)
#define	cpu_memoryread_b(a)	i286_memoryread(a)
#define	cpu_memoryread_w(a)	i286_memoryread_w(a)
#define	cpu_memoryread_d(a)	i286_memoryread_d(a)

#define	cpu_memoryread_region(a,p,l)	memp_read(a,p,l)
#define	cpu_memorywrite_region(a,p,l)	memp_write(a,p,l)

void i386c_initialize(void);

#endif	/* IA32_CPU_INTERFACE_H__ */