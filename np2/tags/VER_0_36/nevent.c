#include	"compiler.h"
#include	"pccore.h"


	_NEVENT		nevent;


void nevent_init(void) {

	// ���ׂĂ����Z�b�g
	ZeroMemory(&nevent, sizeof(nevent));
}

void nevent_get1stevent(void) {

	// �ŒZ�̃C�x���g�̃N���b�N�����Z�b�g
	if (nevent.readyevents) {
		nevent.baseclock = nevent.item[nevent.level[0]].clock;
	}
	else {
		// �C�x���g���Ȃ��ꍇ�̃N���b�N�����Z�b�g
		nevent.baseclock = NEVENT_MAXCLOCK;
	}

	// �J�E���^�փZ�b�g
	nevent.remainclock = nevent.baseclock;
}

static void nevent_execute(void) {

	UINT		eventnum;
	UINT		i;
	UINT		curid;
	NEVENTITEM	item;

	eventnum = 0;
	for (i=0; i<nevent.waitevents; i++) {
		curid = nevent.waitevent[i];
		item = &nevent.item[curid];

		// �R�[���o�b�N�̎��s
		if (item->proc != NULL) {
			item->proc(item);

			// ����Ɏ����z���̃C�x���g�̃`�F�b�N
			if (item->flag & NEVENT_WAIT) {
				nevent.waitevent[eventnum++] = curid;
			}
		}
		else {
			item->flag &= ~(NEVENT_WAIT);
		}
		item->flag &= ~(NEVENT_SETEVENT);
	}
	nevent.waitevents = eventnum;
}

void nevent_progress(void) {

	UINT		eventnum;
	SINT32		nextbase;
	UINT		i;
	UINT		curid;
	NEVENTITEM	item;

	nevent.clock += nevent.baseclock;
	eventnum = 0;
	nextbase = NEVENT_MAXCLOCK;
	for (i=0; i<nevent.readyevents; i++) {
		curid = nevent.level[i];
		item = &nevent.item[curid];
		item->clock -= nevent.baseclock;
		if (item->clock > 0) {
			// �C�x���g�҂���
			nevent.level[eventnum++] = curid;
			if (nextbase >= item->clock) {
				nextbase = item->clock;
			}
		}
		else {
			// �C�x���g����
			if (!(item->flag & (NEVENT_SETEVENT | NEVENT_WAIT))) {
				nevent.waitevent[nevent.waitevents++] = curid;
			}
			item->flag |= NEVENT_SETEVENT;
			item->flag &= ~(NEVENT_ENABLE);
		}
	}
	nevent.readyevents = eventnum;
	nevent.baseclock = nextbase;
	nevent.remainclock += nextbase;
	nevent_execute();
//	TRACEOUT((buf, "nextbase = %d (%d)", nextbase, nevent.remainclock));
}


void nevent_reset(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			break;
		}
	}
	// �C�x���g�͑��݂����H
	if (i < nevent.readyevents) {
		// ���݂��Ă�������
		nevent.readyevents--;
		for (; i<nevent.readyevents; i++) {
			nevent.level[i] = nevent.level[i+1];
		}
	}
}

void nevent_waitreset(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.waitevents; i++) {
		if (nevent.waitevent[i] == id) {
			break;
		}
	}
	// �C�x���g�͑��݂����H
	if (i < nevent.waitevents) {
		// ���݂��Ă�������
		nevent.waitevents--;
		for (; i<nevent.waitevents; i++) {
			nevent.waitevent[i] = nevent.waitevent[i+1];
		}
	}
}

void nevent_set(UINT id, SINT32 eventclock, NEVENTCB proc, BOOL absolute) {

	SINT32		clock;
	NEVENTITEM	item;
	UINT		eventid;
	UINT		i;

	clock = nevent.baseclock - nevent.remainclock;
	item = &nevent.item[id];
	item->proc = proc;
	item->flag = 0;
	if (absolute) {
		item->clock = eventclock + clock;
	}
	else {
		item->clock += eventclock;
	}
#if 0
	if (item->clock < clock) {
		item->clock = clock;
	}
#endif
	// �C�x���g�̍폜
	nevent_reset(id);

	// �C�x���g�̑}���ʒu�̌���
	for (eventid=0; eventid<nevent.readyevents; eventid++) {
		if (item->clock < nevent.item[nevent.level[eventid]].clock) {
			break;
		}
	}

	// �C�x���g�̑}��
	for (i=nevent.readyevents; i>eventid; i--) {
		nevent.level[i] = nevent.level[i-1];
	}
	nevent.level[eventid] = id;
	nevent.readyevents++;

	// �����ŒZ�C�x���g��������...
	if (eventid == 0) {
		clock = nevent.baseclock - item->clock;
		nevent.baseclock -= clock;
		nevent.remainclock -= clock;
//		TRACEOUT(("reset nextbase -%d (%d)", clock, nevent.remainclock));
	}
}

void nevent_setbyms(UINT id, SINT32 ms, NEVENTCB proc, BOOL absolute) {

	nevent_set(id, (pc.realclock / 1000) * ms, proc, absolute);
}

BOOL nevent_iswork(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			return(TRUE);
		}
	}
	return(FALSE);
}

SINT32 nevent_getremain(UINT id) {

	UINT	i;

	// ���ݐi�s���Ă�C�x���g������
	for (i=0; i<nevent.readyevents; i++) {
		if (nevent.level[i] == id) {
			return(nevent.item[id].clock -
								(nevent.baseclock - nevent.remainclock));
		}
	}
	return(-1);
}

void nevent_forceexit(void) {

	if (nevent.remainclock > 0) {
		nevent.baseclock -= nevent.remainclock;
		nevent.remainclock = 0;
	}
}
