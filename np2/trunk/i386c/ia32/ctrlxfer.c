/*	$Id: ctrlxfer.c,v 1.14 2004/02/19 03:04:01 yui Exp $	*/

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

#include "compiler.h"
#include "cpu.h"
#include "ia32.mcr"

#include "ctrlxfer.h"


/*------------------------------------------------------------------------------
 * JMPfar_pm
 */
static void JMPfar_pm_code_segment(selector_t *jmp_sel, DWORD new_ip);
static void JMPfar_pm_call_gate(selector_t *jmp_sel);
static void JMPfar_pm_task_gate(selector_t *jmp_sel);
static void JMPfar_pm_tss(selector_t *jmp_sel);

void
JMPfar_pm(WORD selector, DWORD new_ip)
{
	selector_t jmp_sel;
	int rv;

	VERBOSE(("JMPfar_pm: old EIP = %04x:%08x, ESP = %04x:%08x", CPU_CS, CPU_PREV_EIP, CPU_SS, CPU_ESP));
	VERBOSE(("JMPfar_pm: selector = 0x%04x, new_ip = 0x%08x", selector, new_ip));

	rv = parse_selector(&jmp_sel, selector);
	if (rv < 0) {
		VERBOSE(("JMPfar_pm: parse_selector (selector = %04x, rv = %d)", selector, rv));
		EXCEPTION(GP_EXCEPTION, jmp_sel.idx);
	}

	if (jmp_sel.desc.s) {
		VERBOSE(("JMPfar_pm: code or data segment descriptor"));

		/* check segment type */
		if (!jmp_sel.desc.u.seg.c) {
			/* data segment */
			VERBOSE(("JMPfar_pm: data segment"));
			EXCEPTION(GP_EXCEPTION, jmp_sel.idx);
		}

		/* code segment descriptor */
		JMPfar_pm_code_segment(&jmp_sel, new_ip);
	} else {
		/* system descriptor */
		VERBOSE(("JMPfar_pm: system descriptor"));

		switch (jmp_sel.desc.type) {
		case CPU_SYSDESC_TYPE_CALL_16:
		case CPU_SYSDESC_TYPE_CALL_32:
			JMPfar_pm_call_gate(&jmp_sel);
			break;

		case CPU_SYSDESC_TYPE_TASK:
			JMPfar_pm_task_gate(&jmp_sel);
			break;

		case CPU_SYSDESC_TYPE_TSS_16:
		case CPU_SYSDESC_TYPE_TSS_32:
			JMPfar_pm_tss(&jmp_sel);
			break;

		case CPU_SYSDESC_TYPE_TSS_BUSY_16:
		case CPU_SYSDESC_TYPE_TSS_BUSY_32:
			VERBOSE(("JMPfar_pm: task is busy"));
			/*FALLTHROUGH*/
		default:
			VERBOSE(("JMPfar_pm: invalid descriptor type (type = %d)", jmp_sel.desc.type));
			EXCEPTION(GP_EXCEPTION, jmp_sel.idx);
			break;
		}
	}

	VERBOSE(("JMPfar_pm: new EIP = %04x:%08x, ESP = %04x:%08x", CPU_CS, CPU_EIP, CPU_SS, CPU_ESP));
}

/*---
 * JMPfar: code segment
 */
static void
JMPfar_pm_code_segment(selector_t *cs_sel, DWORD new_ip)
{

	VERBOSE(("JMPfar_pm: CODE-SEGMENT"));

	/* check privilege level */
	if (!cs_sel->desc.u.seg.ec) {
		VERBOSE(("JMPfar_pm: NON-CONFORMING-CODE-SEGMENT"));
		/* 布船 p.119 4.8.1.1. */
		if (cs_sel->rpl > CPU_STAT_CPL) {
			VERBOSE(("JMPfar_pm: RPL(%d) > CPL(%d)", cs_sel->rpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, cs_sel->idx);
		}
		if (cs_sel->desc.dpl != CPU_STAT_CPL) {
			VERBOSE(("JMPfar_pm: DPL(%d) != CPL(%d)", cs_sel->desc.dpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, cs_sel->idx);
		}
	} else {
		VERBOSE(("JMPfar_pm: CONFORMING-CODE-SEGMENT"));
		/* 布船 p.120 4.8.1.2. */
		if (cs_sel->desc.dpl > CPU_STAT_CPL) {
			VERBOSE(("JMPfar_pm: DPL(%d) > CPL(%d)", cs_sel->desc.dpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, cs_sel->idx);
		}
	}

	/* not present */
	if (selector_is_not_present(cs_sel)) {
		VERBOSE(("JMPfar_pm: selector is not present"));
		EXCEPTION(NP_EXCEPTION, cs_sel->idx);
	}

	/* out of range */
	if (new_ip > cs_sel->desc.u.seg.limit) {
		VERBOSE(("JMPfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, cs_sel->desc.u.seg.limit));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	load_cs(cs_sel->selector, &cs_sel->desc, CPU_STAT_CPL);
	SET_EIP(new_ip);
}

/*---
 * JMPfar: call gate
 */
static void
JMPfar_pm_call_gate(selector_t *callgate_sel)
{
	selector_t cs_sel;
	int rv;

	VERBOSE(("JMPfar_pm: CALL-GATE"));

	/* check privilege level */
	if (callgate_sel->desc.dpl < CPU_STAT_CPL) {
		VERBOSE(("JMPfar_pm: DPL(%d) < CPL(%d)", callgate_sel->desc.dpl, CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, callgate_sel->idx);
	}
	if (callgate_sel->desc.dpl < callgate_sel->rpl) {
		VERBOSE(("JMPfar_pm: DPL(%d) < RPL(%d)",  callgate_sel->desc.dpl, callgate_sel->rpl));
		EXCEPTION(GP_EXCEPTION, callgate_sel->idx);
	}

	/* not present */
	if (selector_is_not_present(callgate_sel)) {
		VERBOSE(("JMPfar_pm: selector is not present"));
		EXCEPTION(NP_EXCEPTION, callgate_sel->idx);
	}

	/* parse code segment selector */
	rv = parse_selector(&cs_sel, callgate_sel->desc.u.gate.selector);
	if (rv < 0) {
		VERBOSE(("JMPfar_pm: parse_selector (selector = %04x, rv = %d)", callgate_sel->desc.u.gate.selector, rv));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* check segment type */
	if (!cs_sel.desc.s) {
		VERBOSE(("JMPfar_pm: code segment is system segment"));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}
	if (!cs_sel.desc.u.seg.c) {
		VERBOSE(("JMPfar_pm: code segment is data segment"));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* check privilege level */
	if (!cs_sel.desc.u.seg.ec) {
		/* 布船 p.119 4.8.1.1. */
		if (cs_sel.rpl > CPU_STAT_CPL) {
			VERBOSE(("JMPfar_pm: RPL(%d) > CPL(%d)", cs_sel.rpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, cs_sel.idx);
		}
		if (cs_sel.desc.dpl != CPU_STAT_CPL) {
			VERBOSE(("JMPfar_pm: DPL(%d) != CPL(%d)", cs_sel.desc.dpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, cs_sel.idx);
		}
	} else {
		/* 布船 p.120 4.8.1.2. */
		if (cs_sel.desc.dpl > CPU_STAT_CPL) {
			VERBOSE(("JMPfar_pm: DPL(%d) > CPL(%d)", cs_sel.desc.dpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, cs_sel.idx);
		}
	}

	/* not present */
	if (selector_is_not_present(&cs_sel)) {
		VERBOSE(("JMPfar_pm: selector is not present"));
		EXCEPTION(NP_EXCEPTION, cs_sel.idx);
	}

	/* out of range */
	if (callgate_sel->desc.u.gate.offset > cs_sel.desc.u.seg.limit) {
		VERBOSE(("JMPfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", callgate_sel->desc.u.gate.offset, cs_sel.desc.u.seg.limit));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	load_cs(cs_sel.selector, &cs_sel.desc, CPU_STAT_CPL);
	SET_EIP(callgate_sel->desc.u.gate.offset);
}

/*---
 * JMPfar: task gate
 */
static void
JMPfar_pm_task_gate(selector_t *taskgate_sel)
{
	selector_t tss_sel;
	int rv;

	VERBOSE(("JMPfar_pm: TASK-GATE"));

	/* check privilege level */
	if (taskgate_sel->desc.dpl < CPU_STAT_CPL) {
		VERBOSE(("JMPfar_pm: DPL(%d) < CPL(%d)", taskgate_sel->desc.dpl, CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, taskgate_sel->idx);
	}
	if (taskgate_sel->desc.dpl < taskgate_sel->rpl) {
		VERBOSE(("JMPfar_pm: DPL(%d) < RPL(%d)", taskgate_sel->desc.dpl, taskgate_sel->rpl));
		EXCEPTION(GP_EXCEPTION, taskgate_sel->idx);
	}

	/* not present */
	if (selector_is_not_present(taskgate_sel)) {
		VERBOSE(("JMPfar_pm: selector is not present"));
		EXCEPTION(NP_EXCEPTION, taskgate_sel->idx);
	}

	/* parse tss selector */
	rv = parse_selector(&tss_sel, taskgate_sel->desc.u.gate.selector);
	if (rv < 0 || tss_sel.ldt) {
		VERBOSE(("JMPfar_pm: parse_selector (selector = %04x, rv = %d, %cDT)", taskgate_sel->desc.u.gate.selector, rv, tss_sel.ldt ? 'L' : 'G'));
		EXCEPTION(GP_EXCEPTION, tss_sel.idx);
	}

	/* check descriptor type */
	switch (tss_sel.desc.type) {
	case CPU_SYSDESC_TYPE_TSS_16:
	case CPU_SYSDESC_TYPE_TSS_32:
		break;

	case CPU_SYSDESC_TYPE_TSS_BUSY_16:
	case CPU_SYSDESC_TYPE_TSS_BUSY_32:
		VERBOSE(("JMPfar_pm: task is busy"));
		/*FALLTHROUGH*/
	default:
		VERBOSE(("JMPfar_pm: invalid descriptor type (type = %d)", tss_sel.desc.type));
		EXCEPTION(GP_EXCEPTION, tss_sel.idx);
		break;
	}

	/* not present */
	if (selector_is_not_present(&tss_sel)) {
		VERBOSE(("JMPfar_pm: selector is not present"));
		EXCEPTION(NP_EXCEPTION, tss_sel.idx);
	}

	task_switch(&tss_sel, TASK_SWITCH_JMP);
}

/*---
 * JMPfar: TSS
 */
static void
JMPfar_pm_tss(selector_t *tss_sel)
{

	VERBOSE(("JMPfar_pm: TASK-STATE-SEGMENT"));

	/* check privilege level */
	if (tss_sel->desc.dpl < CPU_STAT_CPL) {
		VERBOSE(("JMPfar_pm: DPL(%d) < CPL(%d)", tss_sel->desc.dpl, CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, tss_sel->idx);
	}
	if (tss_sel->desc.dpl < tss_sel->rpl) {
		VERBOSE(("JMPfar_pm: DPL(%d) < RPL(%d)", tss_sel->desc.dpl, tss_sel->rpl));
		EXCEPTION(GP_EXCEPTION, tss_sel->idx);
	}

	/* not present */
	if (selector_is_not_present(tss_sel)) {
		VERBOSE(("JMPfar_pm: selector is not present"));
		EXCEPTION(NP_EXCEPTION, tss_sel->idx);
	}

	task_switch(tss_sel, TASK_SWITCH_JMP);
}


/*------------------------------------------------------------------------------
 * CALLfar_pm
 */
static void CALLfar_pm_code_segment(selector_t *call_sel, DWORD new_ip);
static void CALLfar_pm_call_gate(selector_t *call_sel);
static void CALLfar_pm_task_gate(selector_t *call_sel);
static void CALLfar_pm_tss(selector_t *call_sel);

void
CALLfar_pm(WORD selector, DWORD new_ip)
{
	selector_t call_sel;
	int rv;

	VERBOSE(("CALLfar_pm: old EIP = %04x:%08x, ESP = %04x:%08x", CPU_CS, CPU_PREV_EIP, CPU_SS, CPU_ESP));
	VERBOSE(("CALLfar_pm: selector = 0x%04x, new_ip = 0x%08x", selector, new_ip));

	rv = parse_selector(&call_sel, selector);
	if (rv < 0) {
		VERBOSE(("CALLfar_pm: parse_selector (selector = %04x, rv = %d)", selector, rv));
		EXCEPTION(GP_EXCEPTION, call_sel.idx);
	}

	if (call_sel.desc.s) {
		/* code or data segment descriptor */
		VERBOSE(("CALLfar_pm: code or data segment descriptor"));

		if (!call_sel.desc.u.seg.c) {
			/* data segment */
			VERBOSE(("CALLfar_pm: data segment"));
			EXCEPTION(GP_EXCEPTION, call_sel.idx);
		}

		/* code segment descriptor */
		CALLfar_pm_code_segment(&call_sel, new_ip);
	} else {
		/* system descriptor */
		VERBOSE(("CALLfar_pm: system descriptor"));

		switch (call_sel.desc.type) {
		case CPU_SYSDESC_TYPE_CALL_16:
		case CPU_SYSDESC_TYPE_CALL_32:
			CALLfar_pm_call_gate(&call_sel);
			break;

		case CPU_SYSDESC_TYPE_TASK:
			CALLfar_pm_task_gate(&call_sel);
			break;

		case CPU_SYSDESC_TYPE_TSS_16:
		case CPU_SYSDESC_TYPE_TSS_32:
			CALLfar_pm_tss(&call_sel);
			break;

		case CPU_SYSDESC_TYPE_TSS_BUSY_16:
		case CPU_SYSDESC_TYPE_TSS_BUSY_32:
			VERBOSE(("CALLfar_pm: task is busy"));
			/*FALLTHROUGH*/
		default:
			VERBOSE(("CALLfar_pm: invalid descriptor type (type = %d)", call_sel.desc.type));
			EXCEPTION(GP_EXCEPTION, call_sel.idx);
			break;
		}
	}

	VERBOSE(("CALLfar_pm: new EIP = %04x:%08x, new ESP = %04x:%08x", CPU_CS, CPU_EIP, CPU_SS, CPU_ESP));
}

/*---
 * CALLfar_pm: code segment
 */
static void
CALLfar_pm_code_segment(selector_t *cs_sel, DWORD new_ip)
{
	DWORD sp;

	VERBOSE(("CALLfar_pm: CODE-SEGMENT"));

	/* check privilege level */
	if (!cs_sel->desc.u.seg.ec) {
		VERBOSE(("CALLfar_pm: NON-CONFORMING-CODE-SEGMENT"));
		/* 布船 p.119 4.8.1.1. */
		if (cs_sel->rpl > CPU_STAT_CPL) {
			VERBOSE(("CALLfar_pm: RPL(%d) > CPL(%d)", cs_sel->rpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, cs_sel->idx);
		}
		if (cs_sel->desc.dpl != CPU_STAT_CPL) {
			VERBOSE(("CALLfar_pm: DPL(%d) != CPL(%d)", cs_sel->desc.dpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, cs_sel->idx);
		}
	} else {
		VERBOSE(("CALLfar_pm: CONFORMING-CODE-SEGMENT"));
		/* 布船 p.120 4.8.1.2. */
		if (cs_sel->desc.dpl > CPU_STAT_CPL) {
			VERBOSE(("CALLfar_pm: DPL(%d) > CPL(%d)", cs_sel->desc.dpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, cs_sel->idx);
		}
	}

	/* not present */
	if (selector_is_not_present(cs_sel)) {
		VERBOSE(("CALLfar_pm: selector is not present"));
		EXCEPTION(NP_EXCEPTION, cs_sel->idx);
	}

	if (CPU_STAT_SS32) {
		sp = CPU_ESP;
	} else {
		sp = CPU_SP;
	}
	if (CPU_INST_OP32) {
		CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 8);

		/* out of range */
		if (new_ip > cs_sel->desc.u.seg.limit) {
			VERBOSE(("CALLfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, cs_sel->desc.u.seg.limit));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		PUSH0_32(CPU_CS);
		PUSH0_32(CPU_EIP);
	} else {
		CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 4);

		/* out of range */
		if (new_ip > cs_sel->desc.u.seg.limit) {
			VERBOSE(("CALLfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, cs_sel->desc.u.seg.limit));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		PUSH0_16(CPU_CS);
		PUSH0_16(CPU_IP);
	}

	load_cs(cs_sel->selector, &cs_sel->desc, CPU_STAT_CPL);
	SET_EIP(new_ip);
}

/*---
 * CALLfar_pm: call gate
 */
static void CALLfar_pm_call_gate_same_privilege(selector_t *call_sel, selector_t *cs_sel);
static void CALLfar_pm_call_gate_more_privilege(selector_t *call_sel, selector_t *cs_sel);

static void
CALLfar_pm_call_gate(selector_t *callgate_sel)
{
	selector_t cs_sel;
	int rv;

	VERBOSE(("CALLfar_pm: CALL-GATE"));

	/* check privilege level */
	if (callgate_sel->desc.dpl < CPU_STAT_CPL) {
		VERBOSE(("CALLfar_pm: DPL(%d) < CPL(%d)", callgate_sel->desc.dpl, CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, callgate_sel->idx);
	}
	if (callgate_sel->desc.dpl < callgate_sel->rpl) {
		VERBOSE(("CALLfar_pm: DPL(%d) < CPL(%d)", callgate_sel->desc.dpl, callgate_sel->rpl));
		EXCEPTION(GP_EXCEPTION, callgate_sel->idx);
	}

	/* not present */
	if (selector_is_not_present(callgate_sel)) {
		VERBOSE(("CALLfar_pm: selector is not present"));
		EXCEPTION(NP_EXCEPTION, callgate_sel->idx);
	}

	/* parse code segment descriptor */
	rv = parse_selector(&cs_sel, callgate_sel->desc.u.gate.selector);
	if (rv < 0) {
		VERBOSE(("CALLfar_pm: parse_selector (selector = %04x, rv = %d)", callgate_sel->desc.u.gate.selector, rv));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* check segment type */
	if (!cs_sel.desc.s) {
		VERBOSE(("CALLfar_pm: code segment is system segment"));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}
	if (!cs_sel.desc.u.seg.c) {
		VERBOSE(("CALLfar_pm: code segment is data segment"));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* check privilege level */
	if (cs_sel.desc.dpl > CPU_STAT_CPL) {
		VERBOSE(("CALLfar_pm: DPL(%d) > CPL(%d)", cs_sel.desc.dpl, CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* not present */
	if (selector_is_not_present(&cs_sel)) {
		VERBOSE(("CALLfar_pm: selector is not present"));
		EXCEPTION(NP_EXCEPTION, cs_sel.idx);
	}

	/* out of range */
	if (callgate_sel->desc.u.gate.offset > cs_sel.desc.u.seg.limit) {
		VERBOSE(("CALLfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", callgate_sel->desc.u.gate.offset, cs_sel.desc.u.seg.limit));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	if (!cs_sel.desc.u.seg.ec && (cs_sel.desc.dpl < CPU_STAT_CPL)) {
	 	CALLfar_pm_call_gate_more_privilege(callgate_sel, &cs_sel);
	} else {
		CALLfar_pm_call_gate_same_privilege(callgate_sel, &cs_sel);
	}
}

/*---
 * CALLfar_pm: call gate (SAME-PRIVILEGE)
 */
static void
CALLfar_pm_call_gate_same_privilege(selector_t *callgate_sel, selector_t *cs_sel)
{
	DWORD sp;

	VERBOSE(("CALLfar_pm: SAME-PRIVILEGE"));

	if (CPU_STAT_SS32) {
		sp = CPU_ESP;
	} else {
		sp = CPU_SP;
	}

	if (callgate_sel->desc.type == CPU_SYSDESC_TYPE_CALL_32) {
		CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 8);

		PUSH0_32(CPU_CS);
		PUSH0_32(CPU_EIP);

		load_cs(cs_sel->selector, &cs_sel->desc, CPU_STAT_CPL);
		SET_EIP(callgate_sel->desc.u.gate.offset);
	} else {
		CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 4);

		PUSH0_16(CPU_CS);
		PUSH0_16(CPU_IP);

		load_cs(cs_sel->selector, &cs_sel->desc, CPU_STAT_CPL);
		SET_EIP(callgate_sel->desc.u.gate.offset);
	}
}

/*---
 * CALLfar_pm: call gate (MORE-PRIVILEGE)
 */
static void
CALLfar_pm_call_gate_more_privilege(selector_t *callgate_sel, selector_t *cs_sel)
{
	DWORD param[32];	/* copy param */
	selector_t ss_sel;
	DWORD old_eip, old_esp;
	DWORD new_esp;
	WORD old_cs, old_ss;
	WORD new_ss;
	int param_count;
	int i;
	int rv;

	VERBOSE(("CALLfar_pm: MORE-PRIVILEGE"));

	/* save register */
	old_cs = CPU_CS;
	old_ss = CPU_SS;
	old_eip = CPU_EIP;
	old_esp = CPU_ESP;
	if (!CPU_STAT_SS32) {
		old_esp &= 0xffff;
	}

	/* get stack pointer from TSS */
	get_stack_pointer_from_tss(cs_sel->desc.dpl, &new_ss, &new_esp);

	/* parse stack segment descriptor */
	rv = parse_selector(&ss_sel, new_ss);
	if (rv < 0) {
		VERBOSE(("CALLfar_pm: parse_selector (selector = %04x, rv = %d)", new_ss, rv));
		EXCEPTION(TS_EXCEPTION, ss_sel.idx);
	}

	/* check privilege level */
	if (ss_sel.rpl != cs_sel->desc.dpl) {
		VERBOSE(("CALLfar_pm: RPL[SS](%d) != DPL[CS](%d)", ss_sel.rpl, cs_sel->desc.dpl));
		EXCEPTION(TS_EXCEPTION, ss_sel.idx);
	}
	if (ss_sel.desc.dpl != cs_sel->desc.dpl) {
		VERBOSE(("CALLfar_pm: DPL[SS](%d) != DPL[CS](%d)", ss_sel.desc.dpl, cs_sel->desc.dpl));
		EXCEPTION(TS_EXCEPTION, ss_sel.idx);
	}

	/* stack segment must be writable data segment. */
	if (!ss_sel.desc.s) {
		VERBOSE(("CALLfar_pm: stack segment is system segment"));
		EXCEPTION(TS_EXCEPTION, ss_sel.idx);
	}
	if (ss_sel.desc.u.seg.c) {
		VERBOSE(("CALLfar_pm: stack segment is code segment"));
		EXCEPTION(TS_EXCEPTION, ss_sel.idx);
	}
	if (!ss_sel.desc.u.seg.wr) {
		VERBOSE(("CALLfar_pm: stack segment is read-only data segment"));
		EXCEPTION(TS_EXCEPTION, ss_sel.idx);
	}

	/* not present */
	if (selector_is_not_present(&ss_sel)) {
		VERBOSE(("CALLfar_pm: stack segment selector is not present"));
		EXCEPTION(SS_EXCEPTION, ss_sel.idx);
	}

	param_count = callgate_sel->desc.u.gate.count;
	VERBOSE(("CALLfar_pm: param_count = %d", param_count));

	if (callgate_sel->desc.type == CPU_SYSDESC_TYPE_CALL_32) {
		CHECK_STACK_PUSH(&ss_sel.desc, new_esp, 16 + param_count * 4);

		/* dump param */
		for (i = 0; i < param_count; i++) {
			param[i] = cpu_vmemoryread_d(CPU_SS_INDEX, old_esp + i * 4);
			VERBOSE(("CALLfar_pm: get param[%d] = %08x", i, param[i]));
		}

		load_ss(ss_sel.selector, &ss_sel.desc, ss_sel.desc.dpl);
		if (CPU_STAT_SS32) {
			CPU_ESP = new_esp;
		} else {
			CPU_SP = (WORD)new_esp;
		}

		load_cs(cs_sel->selector, &cs_sel->desc, cs_sel->desc.dpl);
		SET_EIP(callgate_sel->desc.u.gate.offset);

		PUSH0_32(old_ss);
		PUSH0_32(old_esp);

		/* restore param */
		for (i = param_count; i > 0; i--) {
			PUSH0_32(param[i - 1]);
			VERBOSE(("CALLfar_pm: set param[%d] = %08x", i - 1, param[i - 1]));
		}

		PUSH0_32(old_cs);
		PUSH0_32(old_eip);
	} else {
		CHECK_STACK_PUSH(&ss_sel.desc, new_esp, 8 + param_count * 2);

		/* dump param */
		for (i = 0; i < param_count; i++) {
			param[i] = cpu_vmemoryread_w(CPU_SS_INDEX, old_esp + i * 2);
			VERBOSE(("CALLfar_pm: get param[%d] = %04x", i, param[i]));
		}

		load_ss(ss_sel.selector, &ss_sel.desc, ss_sel.desc.dpl);
		if (CPU_STAT_SS32) {
			CPU_ESP = new_esp;
		} else {
			CPU_SP = (WORD)new_esp;
		}

		load_cs(cs_sel->selector, &cs_sel->desc, cs_sel->desc.dpl);
		SET_EIP(callgate_sel->desc.u.gate.offset);

		PUSH0_16(old_ss);
		PUSH0_16(old_esp);

		/* restore param */
		for (i = param_count; i > 0; i--) {
			PUSH0_16(param[i - 1]);
			VERBOSE(("CALLfar_pm: set param[%d] = %04x", i - 1, param[i - 1]));
		}

		PUSH0_16(old_cs);
		PUSH0_16(old_eip);
	}
}

/*---
 * CALLfar_pm: task gate
 */
static void
CALLfar_pm_task_gate(selector_t *taskgate_sel)
{
	selector_t tss_sel;
	int rv;

	VERBOSE(("CALLfar_pm: TASK-GATE"));

	/* check privilege level */
	if (taskgate_sel->desc.dpl < CPU_STAT_CPL) {
		VERBOSE(("CALLfar_pm: DPL(%d) < CPL(%d)", taskgate_sel->desc.dpl, CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, taskgate_sel->idx);
	}
	if (taskgate_sel->desc.dpl < taskgate_sel->rpl) {
		VERBOSE(("CALLfar_pm: DPL(%d) < CPL(%d)", taskgate_sel->desc.dpl, taskgate_sel->rpl));
		EXCEPTION(GP_EXCEPTION, taskgate_sel->idx);
	}

	/* not present */
	if (selector_is_not_present(taskgate_sel)) {
		VERBOSE(("CALLfar_pm: selector is not present"));
		EXCEPTION(NP_EXCEPTION, taskgate_sel->idx);
	}

	/* tss descriptor */
	rv = parse_selector(&tss_sel, taskgate_sel->desc.u.gate.selector);
	if (rv < 0 || tss_sel.ldt) {
		VERBOSE(("CALLfar_pm: parse_selector (selector = %04x, rv = %d, %cDT)", tss_sel.selector, rv, tss_sel.ldt ? 'L' : 'G'));
		EXCEPTION(GP_EXCEPTION, tss_sel.idx);
	}

	/* check descriptor type */
	switch (tss_sel.desc.type) {
	case CPU_SYSDESC_TYPE_TSS_16:
	case CPU_SYSDESC_TYPE_TSS_32:
		break;

	case CPU_SYSDESC_TYPE_TSS_BUSY_16:
	case CPU_SYSDESC_TYPE_TSS_BUSY_32:
		VERBOSE(("CALLfar_pm: task is busy"));
		/*FALLTHROUGH*/
	default:
		VERBOSE(("CALLfar_pm: invalid descriptor type (type = %d)", tss_sel.desc.type));
		EXCEPTION(GP_EXCEPTION, tss_sel.idx);
		break;
	}

	/* not present */
	if (selector_is_not_present(&tss_sel)) {
		VERBOSE(("CALLfar_pm: TSS selector is not present"));
		EXCEPTION(NP_EXCEPTION, tss_sel.idx);
	}

	task_switch(&tss_sel, TASK_SWITCH_CALL);
}

/*---
 * CALLfar_pm: TSS
 */
static void
CALLfar_pm_tss(selector_t *tss_sel)
{

	VERBOSE(("TASK-STATE-SEGMENT"));

	/* check privilege level */
	if (tss_sel->desc.dpl < CPU_STAT_CPL) {
		VERBOSE(("CALLfar_pm: DPL(%d) < CPL(%d)", tss_sel->desc.dpl, CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, tss_sel->idx);
	}
	if (tss_sel->desc.dpl < tss_sel->rpl) {
		VERBOSE(("CALLfar_pm: DPL(%d) < CPL(%d)", tss_sel->desc.dpl, tss_sel->rpl));
		EXCEPTION(GP_EXCEPTION, tss_sel->idx);
	}

	/* not present */
	if (selector_is_not_present(tss_sel)) {
		VERBOSE(("CALLfar_pm: TSS selector is not present"));
		EXCEPTION(NP_EXCEPTION, tss_sel->idx);
	}

	task_switch(tss_sel, TASK_SWITCH_CALL);
}


/*------------------------------------------------------------------------------
 * RETfar_pm
 */

void
RETfar_pm(DWORD nbytes)
{
	selector_t cs_sel, ss_sel, temp_sel;
	DWORD sp;
	DWORD new_ip, new_sp;
	WORD new_cs, new_ss;
	int rv;
	int i;

	VERBOSE(("RETfar_pm: old EIP = %04x:%08x, ESP = %04x:%08x, nbytes = %d", CPU_CS, CPU_PREV_EIP, CPU_SS, CPU_ESP, nbytes));

	if (CPU_STAT_SS32) {
		sp = CPU_ESP;
	} else {
		sp = CPU_SP;
	}
	if (CPU_INST_OP32) {
		CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), sp, nbytes + 8);
		new_ip = cpu_vmemoryread_d(CPU_SS_INDEX, sp);
		new_cs = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 4);
	} else {
		CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), sp, nbytes + 4);
		new_ip = cpu_vmemoryread_w(CPU_SS_INDEX, sp);
		new_cs = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 2);
	}

	rv = parse_selector(&cs_sel, new_cs);
	if (rv < 0) {
		VERBOSE(("RETfar_pm: parse_selector (selector = %04x, rv = %d, %s)", cs_sel.selector, rv));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* check segment type */
	if (!cs_sel.desc.s) {
		VERBOSE(("RETfar_pm: return to system segment"));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}
	if (!cs_sel.desc.u.seg.c) {
		VERBOSE(("RETfar_pm: return to data segment"));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* check privilege level */
	if (cs_sel.rpl < CPU_STAT_CPL) {
		VERBOSE(("RETfar_pm: RPL(%d) < CPL(%d)", cs_sel.rpl, CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}
	if (!cs_sel.desc.u.seg.ec && (cs_sel.desc.dpl > cs_sel.rpl)) {
		VERBOSE(("RETfar_pm: NON-COMFORMING-CODE-SEGMENT and DPL(%d) > RPL(%d)", cs_sel.desc.dpl, cs_sel.rpl));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* not present */
	if (selector_is_not_present(&cs_sel)) {
		VERBOSE(("RETfar_pm: returned code segment is not present"));
		EXCEPTION(NP_EXCEPTION, cs_sel.idx);
	}

	if (cs_sel.rpl == CPU_STAT_CPL) {
		VERBOSE(("RETfar_pm: RETURN-TO-SAME-PRIVILEGE-LEVEL"));

		/* check code segment limit */
		if (new_ip > cs_sel.desc.u.seg.limit) {
			VERBOSE(("RETfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, cs_sel.desc.u.seg.limit));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		VERBOSE(("RETfar_pm: new_ip = %08x, new_cs = %04x", new_ip, cs_sel.selector));

		if (CPU_INST_OP32) {
			nbytes += 8;
		} else {
			nbytes += 4;
		}
		if (CPU_STAT_SS32) {
			CPU_ESP += nbytes;
		} else {
			CPU_SP += (WORD)nbytes;
		}

		load_cs(cs_sel.selector, &cs_sel.desc, CPU_STAT_CPL);
		SET_EIP(new_ip);
	} else {
		VERBOSE(("RETfar_pm: RETURN-OUTER-PRIVILEGE-LEVEL"));

		if (CPU_INST_OP32) {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 8 + 8 + nbytes);
			new_sp = cpu_vmemoryread_d(CPU_SS_INDEX, sp + 8 + nbytes);
			new_ss = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 8 + nbytes + 4);
		} else {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 4 + 4 + nbytes);
			new_sp = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 4 + nbytes);
			new_ss = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 4 + nbytes + 2);
		}

		rv = parse_selector(&ss_sel, new_ss);
		if (rv < 0) {
			VERBOSE(("RETfar_pm: parse_selector (selector = %04x, rv = %d, %s)", ss_sel.selector, rv));
			EXCEPTION(GP_EXCEPTION, ss_sel.idx);
		}

		/* check stack segment descriptor */
		if (!ss_sel.desc.s) {
			VERBOSE(("RETfar_pm: stack segment is system segment"));
			EXCEPTION(GP_EXCEPTION, cs_sel.idx);
		}
		if (ss_sel.desc.u.seg.c) {
			VERBOSE(("RETfar_pm: stack segment is code segment"));
			EXCEPTION(GP_EXCEPTION, cs_sel.idx);
		}
		if (!ss_sel.desc.u.seg.wr) {
			VERBOSE(("RETfar_pm: stack segment is read-only data segment"));
			EXCEPTION(GP_EXCEPTION, cs_sel.idx);
		}

		/* check privilege level */
		if (ss_sel.rpl != cs_sel.rpl) {
			VERBOSE(("RETfar_pm: RPL[SS](%d) != RPL[CS](%d)", ss_sel.rpl, cs_sel.rpl));
			EXCEPTION(GP_EXCEPTION, cs_sel.idx);
		}
		if (ss_sel.desc.dpl != cs_sel.rpl) {
			VERBOSE(("RETfar_pm: DPL[SS](%d) != RPL[CS](%d)", ss_sel.desc.dpl, cs_sel.rpl));
			EXCEPTION(GP_EXCEPTION, cs_sel.idx);
		}

		/* not present */
		if (selector_is_not_present(&ss_sel)) {
			VERBOSE(("RETfar_pm: stack segment is not present"));
			EXCEPTION(SS_EXCEPTION, ss_sel.idx);
		}

		/* check code segment limit */
		if (new_ip > cs_sel.desc.u.seg.limit) {
			VERBOSE(("RETfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, cs_sel.desc.u.seg.limit));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		VERBOSE(("RETfar_pm: new_ip = %08x, new_cs = %04x", new_ip, cs_sel.selector));
		VERBOSE(("RETfar_pm: new_sp = %08x, new_ss = %04x", new_sp, ss_sel.selector));

		load_cs(cs_sel.selector, &cs_sel.desc, cs_sel.rpl);
		SET_EIP(new_ip);

		load_ss(ss_sel.selector, &ss_sel.desc, cs_sel.rpl);
		if (CPU_STAT_SS32) {
			CPU_ESP = new_sp + nbytes;
		} else {
			CPU_SP = (WORD)(new_sp + nbytes);
		}

		/* check segment register */
		for (i = 0; i < CPU_SEGREG_NUM; i++) {
			descriptor_t *dp;
			BOOL valid;

			dp = &CPU_STAT_SREG(i);
			if ((!dp->u.seg.c || !dp->u.seg.ec)
			 && (CPU_STAT_SREG(i).dpl < CPU_STAT_CPL)) {
				/* segment register is invalid */
				CPU_REGS_SREG(i) = 0;
				CPU_STAT_SREG_CLEAR(i);
				continue;
			}

			rv = parse_selector(&temp_sel, CPU_REGS_SREG(i));
			if (rv < 0) {
				/* segment register is invalid */
				CPU_REGS_SREG(i) = 0;
				CPU_STAT_SREG_CLEAR(i);
				continue;
			}

			valid = TRUE;
			if (!temp_sel.desc.s) {
				/* system segment */
				valid = FALSE;
			}
			if (temp_sel.desc.u.seg.c && !temp_sel.desc.u.seg.wr) {
				/* execute-only code segment */
				valid = FALSE;
			}
			if (!temp_sel.desc.u.seg.c || !temp_sel.desc.u.seg.ec) {
				if (CPU_STAT_CPL > temp_sel.desc.dpl) {
					valid = FALSE;
				}
			}

			if (!valid) {
				/* segment register is invalid */
				CPU_REGS_SREG(i) = 0;
				CPU_STAT_SREG(i).valid = 0;
			}
		}
	}

	VERBOSE(("RETfar_pm: new EIP = %04x:%08x, ESP = %04x:%08x", CPU_CS, CPU_EIP, CPU_SS, CPU_ESP));
}


/*------------------------------------------------------------------------------
 * IRET_pm
 */
static void IRET_pm_nested_task(void);
static void IRET_pm_protected_mode_return(DWORD new_cs, DWORD new_ip, DWORD new_flags);
static void IRET_pm_protected_mode_return_same_privilege(selector_t *cs_sel, DWORD new_ip, DWORD new_flags);
static void IRET_pm_protected_mode_return_outer_privilege(selector_t *cs_sel, DWORD new_ip, DWORD new_flags);
static void IRET_pm_return_to_vm86(DWORD new_cs, DWORD new_ip, DWORD new_flags);
static void IRET_pm_return_from_vm86(DWORD new_cs, DWORD new_ip, DWORD new_flags);

void
IRET_pm(void)
{
	DWORD sp;
	DWORD new_ip, new_flags;
	WORD new_cs;
//	int rv;

	VERBOSE(("IRET_pm: old EIP = %04x:%08x, ESP = %04x:%08x", CPU_CS, CPU_PREV_EIP, CPU_SS, CPU_ESP));

	if (!(CPU_EFLAG & VM_FLAG) && (CPU_EFLAG & NT_FLAG)) {
		/* TASK-RETURN: PE=1, VM=0, NT=1 */
		IRET_pm_nested_task();
	} else {
		if (CPU_STAT_SS32) {
			sp = CPU_ESP;
		} else {
			sp = CPU_SP;
		}
		if (CPU_INST_OP32) {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 12);
			new_ip = cpu_vmemoryread_d(CPU_SS_INDEX, sp);
			new_cs = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 4);
			new_flags = cpu_vmemoryread_d(CPU_SS_INDEX, sp + 8);
		} else {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 6);
			new_ip = cpu_vmemoryread_w(CPU_SS_INDEX, sp);
			new_cs = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 2);
			new_flags = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 4);
		}

		VERBOSE(("IRET_pm: new_ip = %08x, new_cs = %04x, new_eflags = %08x", new_ip, new_cs, new_flags));

		if (CPU_EFLAG & VM_FLAG) {
			/* RETURN-FROM-VIRTUAL-8086-MODE */
			IRET_pm_return_from_vm86(new_cs, new_ip, new_flags);
		} else if (new_flags & VM_FLAG) {
			/* RETURN-TO-VIRTUAL-8086-MODE */
			IRET_pm_return_to_vm86(new_cs, new_ip, new_flags);
		} else {
			/* PROTECTED-MODE-RETURN */
			IRET_pm_protected_mode_return(new_cs, new_ip, new_flags);
		}
	}

	VERBOSE(("IRET_pm: new EIP = %04x:%08x, ESP = %04x:%08x", CPU_CS, CPU_EIP, CPU_SS, CPU_ESP));
}

/*---
 * IRET_pm: NT_FLAG
 */
static void
IRET_pm_nested_task(void)
{
	selector_t tss_sel;
	int rv;
	WORD new_tss;

	VERBOSE(("IRET_pm: TASK-RETURN: PE=1, VM=0, NT=1"));

	new_tss = get_backlink_selector_from_tss();

	rv = parse_selector(&tss_sel, new_tss);
	if (rv < 0 || tss_sel.ldt) {
		VERBOSE(("IRET_pm: parse_selector (selector = %04x, rv = %d, %cDT)", tss_sel.selector, rv, tss_sel.ldt ? 'L' : 'G'));
		EXCEPTION(GP_EXCEPTION, tss_sel.idx);
	}

	/* check system segment */
	if (tss_sel.desc.s) {
		VERBOSE(("IRET_pm: task segment is %s segment", tss_sel.desc.u.seg.c ? "code" : "data"));
		EXCEPTION(GP_EXCEPTION, tss_sel.idx);
	}

	switch (tss_sel.desc.type) {
	case CPU_SYSDESC_TYPE_TSS_BUSY_16:
	case CPU_SYSDESC_TYPE_TSS_BUSY_32:
		break;

	case CPU_SYSDESC_TYPE_TSS_16:
	case CPU_SYSDESC_TYPE_TSS_32:
		VERBOSE(("IRET_pm: task is not busy"));
		/*FALLTHROUGH*/
	default:
		VERBOSE(("IRET_pm: invalid descriptor type (type = %d)", tss_sel.desc.type));
		EXCEPTION(GP_EXCEPTION, tss_sel.idx);
		break;
	}

	/* not present */
	if (selector_is_not_present(&tss_sel)) {
		VERBOSE(("IRET_pm: tss segment is not present"));
		EXCEPTION(NP_EXCEPTION, tss_sel.idx);
	}

	task_switch(&tss_sel, TASK_SWITCH_IRET);
}

/*---
 * IRET_pm: PROTECTED-MODE-RETURN
 */
static void
IRET_pm_protected_mode_return(DWORD new_cs, DWORD new_ip, DWORD new_flags)
{
	selector_t cs_sel;
	int rv;

	/* PROTECTED-MODE-RETURN */
	VERBOSE(("IRET_pm: PE=1, VM=0 in flags image"));

	rv = parse_selector(&cs_sel, (WORD)new_cs);
	if (rv < 0) {
		VERBOSE(("IRET_pm: parse_selector (selector = %04x, rv = %d)", cs_sel.selector, rv));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* check code segment descriptor */
	if (!cs_sel.desc.s) {
		VERBOSE(("IRET_pm: return code segment is system segment"));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}
	if (!cs_sel.desc.u.seg.c) {
		VERBOSE(("IRET_pm: return code segment is data segment"));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* check privilege level */
	if (cs_sel.rpl < CPU_STAT_CPL) {
		VERBOSE(("IRET_pm: RPL(%d) < CPL(%d)", cs_sel.rpl, CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}
	if (cs_sel.desc.u.seg.ec && (cs_sel.desc.dpl > cs_sel.rpl)) {
		VERBOSE(("IRET_pm: CONFORMING-CODE-SEGMENT and DPL(%d) != RPL(%d)", cs_sel.desc.dpl, cs_sel.rpl));
		EXCEPTION(GP_EXCEPTION, cs_sel.idx);
	}

	/* not present */
	if (selector_is_not_present(&cs_sel)) {
		VERBOSE(("IRET_pm: code segment is not present"));
		EXCEPTION(NP_EXCEPTION, cs_sel.idx);
	}

	if (cs_sel.rpl > CPU_STAT_CPL) {
		IRET_pm_protected_mode_return_outer_privilege(&cs_sel, new_ip, new_flags);
	} else {
		IRET_pm_protected_mode_return_same_privilege(&cs_sel, new_ip, new_flags);
	}
}

/*---
 * IRET_pm: SAME-PRIVILEGE
 */
static void
IRET_pm_protected_mode_return_same_privilege(selector_t *cs_sel, DWORD new_ip, DWORD new_flags)
{
	DWORD mask;
	DWORD stacksize;

	VERBOSE(("IRET_pm: RETURN-TO-SAME-PRIVILEGE-LEVEL"));

	/* check code segment limit */
	if (new_ip > cs_sel->desc.u.seg.limit) {
		VERBOSE(("IRET_pm: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, cs_sel->desc.u.seg.limit));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	mask = 0;
	if (CPU_INST_OP32)
		mask |= RF_FLAG;
	if (CPU_STAT_CPL <= CPU_STAT_IOPL)
		mask |= I_FLAG;
	if (CPU_STAT_CPL == 0) {
		mask |= IOPL_FLAG;
		if (CPU_INST_OP32) {
			mask |= VM_FLAG|VIF_FLAG|VIP_FLAG;
		}
	}
	if (CPU_INST_OP32) {
		stacksize = 12;
	} else {
		stacksize = 6;
	}

	/* set new register */
	load_cs(cs_sel->selector, &cs_sel->desc, CPU_STAT_CPL);
	SET_EIP(new_ip);

	set_eflags(new_flags, mask);

	if (CPU_STAT_SS32) {
		CPU_ESP += stacksize;
	} else {
		CPU_SP += (WORD)stacksize;
	}
}

/*---
 * IRET_pm: OUTER-PRIVILEGE
 */
static void
IRET_pm_protected_mode_return_outer_privilege(selector_t *cs_sel, DWORD new_ip, DWORD new_flags)
{
	descriptor_t *dp;
	selector_t ss_sel;
	DWORD mask;
	DWORD sp;
	DWORD new_sp;
	WORD new_ss;
	int rv;
	int i;

	VERBOSE(("IRET_pm: RETURN-OUTER-PRIVILEGE-LEVEL"));

	if (CPU_STAT_SS32) {
		sp = CPU_ESP;
	} else {
		sp = CPU_SP;
	}
	if (CPU_INST_OP32) {
		CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 20);
		new_sp = cpu_vmemoryread_d(CPU_SS_INDEX, sp + 12);
		new_ss = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 16);
	} else {
		CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 10);
		new_sp = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 6);
		new_ss = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 8);
	}
	VERBOSE(("IRET_pm: new_sp = 0x%08x, new_ss = 0x%04x", new_sp, new_ss));

	rv = parse_selector(&ss_sel, new_ss);
	if (rv < 0) {
		VERBOSE(("IRET_pm: parse_selector (selector = %04x, rv = %d)", ss_sel.selector, rv));
		EXCEPTION(GP_EXCEPTION, ss_sel.idx);
	}

	/* check privilege level */
	if (ss_sel.rpl != cs_sel->rpl) {
		VERBOSE(("IRET_pm: RPL[SS](%d) != RPL[CS](%d)", ss_sel.rpl, cs_sel->rpl));
		EXCEPTION(GP_EXCEPTION, ss_sel.idx);
	}
	if (ss_sel.desc.dpl != cs_sel->rpl) {
		VERBOSE(("IRET_pm: DPL[SS](%d) != RPL[CS](%d)", ss_sel.desc.dpl, cs_sel->rpl));
		EXCEPTION(GP_EXCEPTION, ss_sel.idx);
	}

	/* check stack segment descriptor */
	if (!ss_sel.desc.s) {
		VERBOSE(("IRET_pm: stack segment is system segment"));
		EXCEPTION(GP_EXCEPTION, ss_sel.idx);
	}
	if (ss_sel.desc.u.seg.c) {
		VERBOSE(("IRET_pm: stack segment is code segment"));
		EXCEPTION(GP_EXCEPTION, ss_sel.idx);
	}
	if (!ss_sel.desc.u.seg.wr) {
		VERBOSE(("IRET_pm: stack segment is read-only data segment"));
		EXCEPTION(GP_EXCEPTION, ss_sel.idx);
	}

	/* not present */
	if (selector_is_not_present(&ss_sel)) {
		VERBOSE(("IRET_pm: stack segment is not present"));
		EXCEPTION(SS_EXCEPTION, ss_sel.idx);
	}

	/* check code segment limit */
	if (new_ip > cs_sel->desc.u.seg.limit) {
		VERBOSE(("IRET_pm: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, cs_sel->desc.u.seg.limit));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	mask = 0;
	if (CPU_INST_OP32)
		mask |= RF_FLAG;
	if (CPU_STAT_CPL <= CPU_STAT_IOPL)
		mask |= I_FLAG;
	if (CPU_STAT_CPL == 0) {
		mask |= IOPL_FLAG;
		if (CPU_INST_OP32) {
			mask |= VM_FLAG|VIF_FLAG|VIP_FLAG;
		}
	}

	/* set new register */
	load_cs(cs_sel->selector, &cs_sel->desc, cs_sel->rpl);
	SET_EIP(new_ip);

	set_eflags(new_flags, mask);

	load_ss(ss_sel.selector, &ss_sel.desc, cs_sel->rpl);
	if (CPU_STAT_SS32) {
		CPU_ESP = new_sp;
	} else {
		CPU_SP = (WORD)new_sp;
	}

	/* check segment register */
	for (i = 0; i < CPU_SEGREG_NUM; i++) {
		if ((i != CPU_CS_INDEX) && (i != CPU_SS_INDEX)) {
			dp = &CPU_STAT_SREG(i);
			if ((!dp->u.seg.c || !dp->u.seg.ec)
			 && (CPU_STAT_SREG(i).dpl < CPU_STAT_CPL)) {
				/* segment register is invalid */
				CPU_REGS_SREG(i) = 0;
				CPU_STAT_SREG_CLEAR(i);
				continue;
			}
		}
	}
}

/*---
 * IRET_pm: new_flags & VM_FLAG
 */
static void
IRET_pm_return_to_vm86(DWORD new_cs, DWORD new_ip, DWORD new_flags)
{
	WORD segsel[CPU_SEGREG_NUM];
	DWORD sp;
	DWORD new_sp;
	int i;

	VERBOSE(("IRET_pm: Interrupt procedure was in virtual-8086 mode: PE=1, VM=1 in flags image"));

	if (CPU_STAT_CPL != 0) {
		ia32_panic("IRET_pm: CPL != 0");
	}

	if (!CPU_INST_OP32) {
		ia32_panic("IRET_pm: 16bit mode");
	}

	if (CPU_STAT_SS32) {
		sp = CPU_ESP;
	} else {
		sp = CPU_SP;
	}
	CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), sp, 36);
	new_sp = cpu_vmemoryread_d(CPU_SS_INDEX, sp + 12);
	segsel[CPU_SS_INDEX] = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 16);
	segsel[CPU_ES_INDEX] = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 20);
	segsel[CPU_DS_INDEX] = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 24);
	segsel[CPU_FS_INDEX] = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 28);
	segsel[CPU_GS_INDEX] = cpu_vmemoryread_w(CPU_SS_INDEX, sp + 32);
	segsel[CPU_CS_INDEX] = (WORD)new_cs;

	for (i = 0; i < CPU_SEGREG_NUM; i++) {
		CPU_REGS_SREG(i) = segsel[i];
		CPU_STAT_SREG_INIT(i);
	}

	/* to VM86 mode */
	set_eflags(new_flags, IOPL_FLAG|I_FLAG|VM_FLAG|RF_FLAG);

	new_sp &= 0xffff;
	new_ip &= 0xffff;

	CPU_ESP = new_sp;
	SET_EIP(new_ip);
}

/*---
 * IRET_pm: VM_FLAG
 */
static void
IRET_pm_return_from_vm86(DWORD new_cs, DWORD new_ip, DWORD new_flags)
{
	DWORD stacksize;

	VERBOSE(("IRET_pm: virtual-8086 mode: VM=1"));

	if (CPU_STAT_IOPL == CPU_IOPL3) {
		VERBOSE(("IRET_pm: virtual-8086 mode: IOPL=3"));
		if (CPU_INST_OP32) {
			stacksize = 12;
		} else {
			stacksize = 6;
		}
		if (CPU_STAT_SS32) {
			CPU_ESP += stacksize;
		} else {
			CPU_SP += (WORD)stacksize;
		}

		set_eflags(new_flags, I_FLAG|RF_FLAG);

		CPU_SET_SEGREG(CPU_CS_INDEX, (WORD)new_cs);
		SET_EIP(new_ip);
		return;
	}
	VERBOSE(("IRET_pm: trap to virtual-8086 monitor: VM=1, IOPL<3"));
	EXCEPTION(GP_EXCEPTION, 0);
}
