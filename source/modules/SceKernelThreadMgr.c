#include <stdbool.h>
#include <stdlib.h>
#include <switch.h>
#include <psp2/kernel/threadmgr.h>
#include "SceKernelThreadMgr.h"
#include "SceSysmem.h"
#include "utils.h"
#include "log.h"

#define MAX_THREADS	32
#define KERNEL_TLS_SIZE 0x800

static VitaThreadInfo g_vita_thread_infos[MAX_THREADS];
static bool g_vita_thread_infos_valid[MAX_THREADS];
static s32 g_vita_thread_info_tls_slot_id;

static VitaThreadInfo *thread_info_alloc(void)
{
	for (int i = 0; i < ARRAY_SIZE(g_vita_thread_infos); i++) {
		if (!g_vita_thread_infos_valid[i]) {
			g_vita_thread_infos_valid[i] = true;
			return &g_vita_thread_infos[i];
		}
	}

	return NULL;
}

int SceKernelThreadMgr_init(void)
{
	VitaThreadInfo *ti;

	g_vita_thread_info_tls_slot_id = threadTlsAlloc(NULL);

	/* Create a Vita thread info for this calling (main) thread */
	ti = thread_info_alloc();
	ti->thid = SceSysmem_get_next_uid();
	ti->vita_tls = malloc(KERNEL_TLS_SIZE);
	threadTlsSet(g_vita_thread_info_tls_slot_id, ti);

	return 0;
}

VitaThreadInfo *SceKernelThreadMgr_get_thread_info(void)
{
	return threadTlsGet(g_vita_thread_info_tls_slot_id);
}

int sceKernelDelayThread(SceUInt delay)
{
	svcSleepThread((s64)delay * 1000);
	return 0;
}
