/*	$Id: paging.h,v 1.18 2004/06/15 13:50:13 monaka Exp $	*/

/*
 * Copyright (c) 2003 NONAKA Kimihiro
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

#ifndef	IA32_CPU_PAGING_H__
#define	IA32_CPU_PAGING_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * �ڡ������ǥ��쥯�ȥꡦ����ȥ� (4K �Х��ȥڡ������ѻ�)
 *
 *  31                                    12 11   9 8  7 6 5  4   3   2   1  0 
 * +----------------------------------------+------+-+--+-+-+---+---+---+---+-+
 * |   �ڡ������ơ��֥�Υ١��������ɥ쥹   |���Ѳ�|G|PS|-|A|PCD|PWT|U/S|R/W|P|
 * +----------------------------------------+------+-+--+-+-+---+---+---+---+-+
 *                                              |   |  | | |  |   |   |   |  |
 * 9-11: �����ƥࡦ�ץ�����ޤ����Ѳ�ǽ --------+   |  | | |  |   |   |   |  |
 *    8: �������Х롦�ڡ���(̵�뤵���) ------------+  | | |  |   |   |   |  |
 *    7: �ڡ����������� (0 = 4k �Х��ȥڡ���) ---------+ | |  |   |   |   |  |
 *    6: ͽ�� (-) ---------------------------------------+ |  |   |   |   |  |
 *    5: �������� -----------------------------------------+  |   |   |   |  |
 *    4: ����å���̵�� --------------------------------------+   |   |   |  |
 *    3: �饤�ȥ��롼 --------------------------------------------+   |   |  |
 *    2: �桼���������ѥХ��� (0 = �����ѥХ���) ---------------------+   |  |
 *    1: �ɤ߼�꡿�񤭹��� (0 = �ɤ߼��Τ�) ---------------------------+  |
 *    0: �ڡ���¸�� ---------------------------------------------------------+
 */
#define	CPU_PDE_BASEADDR_MASK	0xfffff000
#define	CPU_PDE_GLOBAL_PAGE	(1 << 8)
#define	CPU_PDE_PAGE_SIZE	(1 << 7)
#define	CPU_PDE_DIRTY		(1 << 6)
#define	CPU_PDE_ACCESS		(1 << 5)
#define	CPU_PDE_CACHE_DISABLE	(1 << 4)
#define	CPU_PDE_WRITE_THROUGH	(1 << 3)
#define	CPU_PDE_USER_MODE	(1 << 2)
#define	CPU_PDE_WRITABLE	(1 << 1)
#define	CPU_PDE_PRESENT		(1 << 0)

/*
 * �ڡ������ǥ��쥯�ȥꡦ����ȥ� (4M �Х��ȥڡ������ѻ�)
 * 
 *  31                        22 21       12 11   9 8  7 6 5  4   3   2   1  0 
 * +----------------------------+-----------+------+-+--+-+-+---+---+---+---+-+
 * |�ڡ����ơ��֥��ʪ�����ɥ쥹|  ͽ��Ѥ� |���Ѳ�|G|PS|D|A|PCD|PWT|U/S|R/W|P|
 * +----------------------------+-----------+------+-+--+-+-+---+---+---+---+-+
 *                                              |   |  | | |  |   |   |   |  |
 * 9-11: �����ƥࡦ�ץ�����ޤ����Ѳ�ǽ --------+   |  | | |  |   |   |   |  |
 *    8: �������Х롦�ڡ��� ------------------------+  | | |  |   |   |   |  |
 *    7: �ڡ����������� (1 = 4M �Х��ȥڡ���) ---------+ | |  |   |   |   |  |
 *    6: �����ƥ� ---------------------------------------+ |  |   |   |   |  |
 *    5: �������� -----------------------------------------+  |   |   |   |  |
 *    4: ����å���̵�� --------------------------------------+   |   |   |  |
 *    3: �饤�ȥ��롼 --------------------------------------------+   |   |  |
 *    2: �桼���������ѥХ��� (0 = �����ѥХ���) ---------------------+   |  |
 *    1: �ɤ߼�꡿�񤭹��� (0 = �ɤ߼��Τ�) ---------------------------+  |
 *    0: �ڡ���¸�� ---------------------------------------------------------+
 */
#define	CPU_PDE_4M_BASEADDR_MASK	0xffc00000
#define	CPU_PDE_4M_GLOBAL_PAGE		(1 << 8)
#define	CPU_PDE_4M_PAGE_SIZE		(1 << 7)
#define	CPU_PDE_4M_DIRTY		(1 << 6)
#define	CPU_PDE_4M_ACCESS		(1 << 5)
#define	CPU_PDE_4M_CACHE_DISABLE	(1 << 4)
#define	CPU_PDE_4M_WRITE_THROUGH	(1 << 3)
#define	CPU_PDE_4M_USER_MODE		(1 << 2)
#define	CPU_PDE_4M_WRITABLE		(1 << 1)
#define	CPU_PDE_4M_PRESENT		(1 << 0)

/*
 * �ڡ������ơ��֥롦����ȥ� (4k �Х��ȡ��ڡ���)
 *
 *  31                                    12 11   9 8 7 6 5  4   3   2   1  0 
 * +----------------------------------------+------+-+-+-+-+---+---+---+---+-+
 * |        �ڡ����Υ١��������ɥ쥹        |���Ѳ�|G|-|D|A|PCD|PWT|U/S|R/W|P|
 * +----------------------------------------+------+-+-+-+-+---+---+---+---+-+
 *                                              |   | | | |  |   |   |   |  |
 *  9-11: �����ƥࡦ�ץ�����ޤ����Ѳ�ǽ -------+   | | | |  |   |   |   |  |
 *     8: �������Х롦�ڡ��� -----------------------+ | | |  |   |   |   |  |
 *     7: ͽ�� (-) -----------------------------------+ | |  |   |   |   |  |
 *     6: �����ƥ� -------------------------------------+ |  |   |   |   |  |
 *     5: �������� ---------------------------------------+  |   |   |   |  |
 *     4: ����å���̵�� ------------------------------------+   |   |   |  |
 *     3: �饤�ȥ��롼 ------------------------------------------+   |   |  |
 *     2: �桼���������ѥХ��� (0 = �����ѥХ���) -------------------+   |  |
 *     1: �ɤ߼�꡿�񤭹��� (0 = �ɤ߼��Τ�) -------------------------+  |
 *     0: �ڡ���¸�� -------------------------------------------------------+
 */
#define	CPU_PTE_BASEADDR_MASK	0xfffff000
#define	CPU_PTE_GLOBAL_PAGE	(1 << 8)
#define	CPU_PTE_PAGE_SIZE	(1 << 7)
#define	CPU_PTE_DIRTY		(1 << 6)
#define	CPU_PTE_ACCESS		(1 << 5)
#define	CPU_PTE_CACHE_DISABLE	(1 << 4)
#define	CPU_PTE_WRITE_THROUGH	(1 << 3)
#define	CPU_PTE_USER_MODE	(1 << 2)
#define	CPU_PTE_WRITABLE	(1 << 1)
#define	CPU_PTE_PRESENT		(1 << 0)


/*
 * linear address memory access function
 */
void MEMCALL cpu_memory_access_la_region(UINT32 address, UINT length, const int ucrw, BYTE *data);
void MEMCALL paging_check(UINT32 laddr, UINT length, const int ucrw);

/* ucrw */
#define	CPU_PAGE_WRITE		(1 << 0)
#define	CPU_PAGE_CODE		(1 << 1)
#define	CPU_PAGE_DATA		(1 << 2)
#define	CPU_PAGE_USER_MODE	(1 << 3)	/* == CPU_MODE_USER */
#define	CPU_PAGE_READ_CODE	(CPU_PAGE_CODE)
#define	CPU_PAGE_READ_DATA	(CPU_PAGE_DATA)
#define	CPU_PAGE_WRITE_DATA	(CPU_PAGE_WRITE|CPU_PAGE_DATA)

#if defined(IA32_PAGING_EACHSIZE)

UINT8 MEMCALL cpu_memory_access_la_RMW_b(UINT32 laddr, UINT32 (*func)(UINT32, void *), void *arg) GCC_ATTR_REGPARM;
UINT16 MEMCALL cpu_memory_access_la_RMW_w(UINT32 laddr, UINT32 (*func)(UINT32, void *), void *arg) GCC_ATTR_REGPARM;
UINT32 MEMCALL cpu_memory_access_la_RMW_d(UINT32 laddr, UINT32 (*func)(UINT32, void *), void *arg) GCC_ATTR_REGPARM;
UINT8 MEMCALL cpu_linear_memory_read_b(UINT32 laddr, const int ucrw) GCC_ATTR_REGPARM;
UINT16 MEMCALL cpu_linear_memory_read_w(UINT32 laddr, const int ucrw) GCC_ATTR_REGPARM;
UINT32 MEMCALL cpu_linear_memory_read_d(UINT32 laddr, const int ucrw) GCC_ATTR_REGPARM;
void MEMCALL cpu_linear_memory_write_b(UINT32 laddr, UINT8 value, const int user_mode) GCC_ATTR_REGPARM;
void MEMCALL cpu_linear_memory_write_w(UINT32 laddr, UINT16 value, const int user_mode) GCC_ATTR_REGPARM;
void MEMCALL cpu_linear_memory_write_d(UINT32 laddr, UINT32 value, const int user_mode) GCC_ATTR_REGPARM;

#define	cpu_lmemoryread(a,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread(a) : \
	 cpu_linear_memory_read_b(a,CPU_PAGE_READ_DATA | (pl))
#define	cpu_lmemoryread_b(a,pl) cpu_lmemoryread(a,pl)
#define	cpu_lmemoryread_w(a,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread_w(a) : \
	 cpu_linear_memory_read_w(a,CPU_PAGE_READ_DATA | (pl))
#define	cpu_lmemoryread_d(a,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread_d(a) : \
	 cpu_linear_memory_read_d(a,CPU_PAGE_READ_DATA | (pl))

#define	cpu_lmemorywrite(a,v,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memorywrite(a,v) : cpu_linear_memory_write_b(a,v,pl)
#define	cpu_lmemorywrite_b(a,v,pl) cpu_lmemorywrite(a,v,pl)
#define	cpu_lmemorywrite_w(a,v,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memorywrite_w(a,v) : cpu_linear_memory_write_w(a,v,pl)
#define	cpu_lmemorywrite_d(a,v,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memorywrite_d(a,v) : cpu_linear_memory_write_d(a,v,pl)

#else	/* !IA32_PAGING_EACHSIZE */

UINT32 MEMCALL cpu_memory_access_la_RMW(UINT32 laddr, UINT length, UINT32 (*func)(UINT32, void *), void *arg) GCC_ATTR_REGPARM;
#define	cpu_memory_access_la_RMW_b(l,f,a) cpu_memory_access_la_RMW(l,1,f,a)
#define	cpu_memory_access_la_RMW_w(l,f,a) cpu_memory_access_la_RMW(l,2,f,a)
#define	cpu_memory_access_la_RMW_d(l,f,a) cpu_memory_access_la_RMW(l,4,f,a)

UINT32 MEMCALL cpu_linear_memory_read(UINT32 address, UINT length, const int ucrw) GCC_ATTR_REGPARM;
#define	cpu_linear_memory_read_b(a,pl) cpu_linear_memory_read(a,1,pl)
#define	cpu_linear_memory_read_w(a,pl) cpu_linear_memory_read(a,2,pl)
#define	cpu_linear_memory_read_d(a,pl) cpu_linear_memory_read(a,4,pl)

void MEMCALL cpu_linear_memory_write(UINT32 address, UINT32 value, UINT length, const int user_mode) GCC_ATTR_REGPARM;
#define	cpu_linear_memory_write_b(a,v,pl) cpu_linear_memory_write(a,v,1,pl)
#define	cpu_linear_memory_write_w(a,v,pl) cpu_linear_memory_write(a,v,2,pl)
#define	cpu_linear_memory_write_d(a,v,pl) cpu_linear_memory_write(a,v,4,pl)

#define	cpu_lmemoryread(a,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread(a) : \
	 (UINT8)cpu_linear_memory_read(a,1,CPU_PAGE_READ_DATA | (pl))
#define	cpu_lmemoryread_b(a,pl) cpu_lmemoryread(a,pl)
#define	cpu_lmemoryread_w(a,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread_w(a) : \
	 (UINT16)cpu_linear_memory_read(a,2,CPU_PAGE_READ_DATA | (pl))
#define	cpu_lmemoryread_d(a,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread_d(a) : \
	 cpu_linear_memory_read(a,4,CPU_PAGE_READ_DATA | (pl))

#define	cpu_lmemorywrite(a,v,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memorywrite(a,v) : \
	 cpu_linear_memory_write(a,v,1,pl)
#define	cpu_lmemorywrite_b(a,v,pl) cpu_lmemorywrite(a,v,pl)
#define	cpu_lmemorywrite_w(a,v,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memorywrite_w(a,v) : \
	 cpu_linear_memory_write(a,v,2,pl)
#define	cpu_lmemorywrite_d(a,v,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memorywrite_d(a,v) : \
	 cpu_linear_memory_write(a,v,4,pl)

#endif	/* IA32_PAGING_EACHSIZE */

/*
 * linear address memory access with superviser mode
 */
#define	cpu_kmemoryread(a)	cpu_lmemoryread(a,CPU_MODE_SUPERVISER)
#define	cpu_kmemoryread_w(a)	cpu_lmemoryread_w(a,CPU_MODE_SUPERVISER)
#define	cpu_kmemoryread_d(a)	cpu_lmemoryread_d(a,CPU_MODE_SUPERVISER)
#define	cpu_kmemorywrite(a,v)	cpu_lmemorywrite(a,v,CPU_MODE_SUPERVISER)
#define	cpu_kmemorywrite_w(a,v)	cpu_lmemorywrite_w(a,v,CPU_MODE_SUPERVISER)
#define	cpu_kmemorywrite_d(a,v)	cpu_lmemorywrite_d(a,v,CPU_MODE_SUPERVISER)


/*
 * CR3 (Page Directory Entry base physical address)
 */
#define	set_CR3(cr3) \
do { \
	VERBOSE(("set_CR3: old = %08x, new = 0x%08x", CPU_CR3, (cr3) & CPU_CR3_MASK)); \
	CPU_CR3 = (cr3) & CPU_CR3_MASK; \
	CPU_STAT_PDE_BASE = CPU_CR3 & CPU_CR3_PD_MASK; \
	tlb_flush(FALSE); \
} while (/*CONSTCOND*/ 0)


/*
 * TLB function
 */
typedef struct {
	UINT32	tag;	/* linear address */
#define	TLB_ENTRY_TAG_VALID		(1 << 0)
/*	pde & pte & CPU_PTE_WRITABLE	(1 << 1)	*/
/*	pde & pte & CPU_PTE_USER_MODE	(1 << 2)	*/
#define	TLB_ENTRY_TAG_DIRTY		CPU_PTE_DIRTY		/* (1 << 6) */
#define	TLB_ENTRY_TAG_GLOBAL		CPU_PTE_GLOBAL_PAGE	/* (1 << 8) */
#define	TLB_ENTRY_TAG_MAX_SHIFT		12

	UINT32	paddr;	/* physical address */

	UINT8	*memp;	/* shortcut for pre-fetch queue */
} TLB_ENTRY_T;


#if defined(IA32_SUPPORT_TLB)
void tlb_init(void);
void MEMCALL tlb_flush(BOOL allflush) GCC_ATTR_REGPARM;
void MEMCALL tlb_flush_page(UINT32 laddr) GCC_ATTR_REGPARM;
TLB_ENTRY_T* MEMCALL tlb_lookup(const UINT32 laddr, const int ucrw) GCC_ATTR_REGPARM;
#else
#define	tlb_init()
#define	tlb_flush(allflush)	(void)(allflush)
#define	tlb_flush_page(la)	(void)(la)
#define	tlb_lookup(la, ucrw)	NULL
#endif

#ifdef __cplusplus
}
#endif

#endif	/* !IA32_CPU_PAGING_H__ */