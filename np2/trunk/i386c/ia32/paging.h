/*	$Id: paging.h,v 1.8 2004/02/05 16:43:44 monaka Exp $	*/

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
 * ページ・ディレクトリ・エントリ (4K バイトページ使用時)
 *
 *  31                                    12 11   9 8  7 6 5  4   3   2   1  0 
 * +----------------------------------------+------+-+--+-+-+---+---+---+---+-+
 * |   ページ・テーブルのベース・アドレス   |使用可|G|PS|0|A|PCD|PWT|U/S|R/W|P|
 * +----------------------------------------+------+-+--+-+-+---+---+---+---+-+
 *                                              |   |  | | |  |   |   |   |  |
 * 9-11: システム・プログラマが使用可能 --------+   |  | | |  |   |   |   |  |
 *    8: グローバル・ページ(無視される) ------------+  | | |  |   |   |   |  |
 *    7: ページ・サイズ (0 = 4k バイトページ) ---------+ | |  |   |   |   |  |
 *    6: 予約 (0) ---------------------------------------+ |  |   |   |   |  |
 *    5: アクセス -----------------------------------------+  |   |   |   |  |
 *    4: キャッシュ無効 --------------------------------------+   |   |   |  |
 *    3: ライトスルー --------------------------------------------+   |   |  |
 *    2: ユーザ／スーパバイザ (0 = スーパバイザ) ---------------------+   |  |
 *    1: 読み取り／書き込み (0 = 読み取りのみ) ---------------------------+  |
 *    0: ページ存在 ---------------------------------------------------------+
 */
#define	CPU_PDE_BASEADDR_MASK	0xfffff000
#define	CPU_PDE_PAGE_SIZE	(1 << 7)
#define	CPU_PDE_ACCESS		(1 << 5)
#define	CPU_PDE_CACHE_DISABLE	(1 << 4)
#define	CPU_PDE_WRITE_THROUGH	(1 << 3)
#define	CPU_PDE_USER_MODE	(1 << 2)
#define	CPU_PDE_WRITABLE	(1 << 1)
#define	CPU_PDE_PRESENT		(1 << 0)

/*
 * ページ・ディレクトリ・エントリ (4M バイトページ使用時)
 * 
 *  31                        22 21       12 11   9 8  7 6 5  4   3   2   1  0 
 * +----------------------------+-----------+------+-+--+-+-+---+---+---+---+-+
 * |ページテーブルの物理アドレス|  予約済み |使用可|G|PS|D|A|PCD|PWT|U/S|R/W|P|
 * +----------------------------+-----------+------+-+--+-+-+---+---+---+---+-+
 *                                              |   |  | | |  |   |   |   |  |
 * 9-11: システム・プログラマが使用可能 --------+   |  | | |  |   |   |   |  |
 *    8: グローバル・ページ ------------------------+  | | |  |   |   |   |  |
 *    7: ページ・サイズ (1 = 4M バイトページ) ---------+ | |  |   |   |   |  |
 *    6: ダーティ ---------------------------------------+ |  |   |   |   |  |
 *    5: アクセス -----------------------------------------+  |   |   |   |  |
 *    4: キャッシュ無効 --------------------------------------+   |   |   |  |
 *    3: ライトスルー --------------------------------------------+   |   |  |
 *    2: ユーザ／スーパバイザ (0 = スーパバイザ) ---------------------+   |  |
 *    1: 読み取り／書き込み (0 = 読み取りのみ) ---------------------------+  |
 *    0: ページ存在 ---------------------------------------------------------+
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
 * ページ・テーブル・エントリ (4k バイト・ページ)
 *
 *  31                                    12 11   9 8 7 6 5  4   3   2   1  0 
 * +----------------------------------------+------+-+-+-+-+---+---+---+---+-+
 * |        ページのベース・アドレス        |使用可|G|0|D|A|PCD|PWT|U/S|R/W|P|
 * +----------------------------------------+------+-+-+-+-+---+---+---+---+-+
 *                                              |   | | | |  |   |   |   |  |
 *  9-11: システム・プログラマが使用可能 -------+   | | | |  |   |   |   |  |
 *     8: グローバル・ページ -----------------------+ | | |  |   |   |   |  |
 *     7: 予約 (0) -----------------------------------+ | |  |   |   |   |  |
 *     6: ダーティ -------------------------------------+ |  |   |   |   |  |
 *     5: アクセス ---------------------------------------+  |   |   |   |  |
 *     4: キャッシュ無効 ------------------------------------+   |   |   |  |
 *     3: ライトスルー ------------------------------------------+   |   |  |
 *     2: ユーザ／スーパバイザ (0 = スーパバイザ) -------------------+   |  |
 *     1: 読み取り／書き込み (0 = 読み取りのみ) -------------------------+  |
 *     0: ページ存在 -------------------------------------------------------+
 */
#define	CPU_PTE_BASEADDR_MASK	0xfffff000
#define	CPU_PTE_GLOBAL_PAGE	(1 << 8)
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
DWORD MEMCALL cpu_linear_memory_read(DWORD address, DWORD length, int code, int user_mode);
void MEMCALL cpu_linear_memory_write(DWORD address, DWORD value, DWORD length, int user_mode);
void MEMCALL paging_check(DWORD laddr, DWORD length, int crw, int user_mode);

/* crw */
#define	CPU_PAGE_READ		(0 << 0)
#define	CPU_PAGE_WRITE		(1 << 0)
#define	CPU_PAGE_CODE		(1 << 1)
#define	CPU_PAGE_DATA		(1 << 2)
#define	CPU_PAGE_READ_CODE	(CPU_PAGE_READ|CPU_PAGE_CODE)
#define	CPU_PAGE_READ_DATA	(CPU_PAGE_READ|CPU_PAGE_DATA)
#define	CPU_PAGE_WRITE_DATA	(CPU_PAGE_WRITE|CPU_PAGE_DATA)


#define	cpu_lmemoryread(a,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread(a) : \
	 (BYTE)cpu_linear_memory_read(a,1,CPU_PAGE_READ_DATA,pl)
#define	cpu_lmemoryread_w(a,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread_w(a) : \
	 (WORD)cpu_linear_memory_read(a,2,CPU_PAGE_READ_DATA,pl)
#define	cpu_lmemoryread_d(a,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread_d(a) : \
	 cpu_linear_memory_read(a,4,CPU_PAGE_READ_DATA,pl)

#define	cpu_lmemorywrite(a,v,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memorywrite(a,v) : \
	 cpu_linear_memory_write(a,v,1,pl)
#define	cpu_lmemorywrite_w(a,v,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memorywrite_w(a,v) : \
	 cpu_linear_memory_write(a,v,2,pl)
#define	cpu_lmemorywrite_d(a,v,pl) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memorywrite_d(a,v) : \
	 cpu_linear_memory_write(a,v,4,pl)

/*
 * code segment
 */
#define	cpu_lcmemoryread(a) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread(a) : \
	 (BYTE)cpu_linear_memory_read(a,1,CPU_PAGE_READ_CODE,CPU_STAT_USER_MODE)
#define	cpu_lcmemoryread_w(a) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread_w(a) : \
	 (WORD)cpu_linear_memory_read(a,2,CPU_PAGE_READ_CODE,CPU_STAT_USER_MODE)
#define	cpu_lcmemoryread_d(a) \
	(!CPU_STAT_PAGING) ? \
	 cpu_memoryread_d(a) : \
	 cpu_linear_memory_read(a,4,CPU_PAGE_READ_CODE,CPU_STAT_USER_MODE)

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
#if defined(IA32_SUPPORT_TLB)
void tlb_init();
void tlb_flush(BOOL allflush);
void tlb_flush_page(DWORD vaddr);
#else
#define	tlb_init()
#define	tlb_flush(allflush)	(void)allflush
#define	tlb_flush_page(vaddr)	(void)vaddr
#endif

#ifdef __cplusplus
}
#endif

#endif	/* !IA32_CPU_PAGING_H__ */
