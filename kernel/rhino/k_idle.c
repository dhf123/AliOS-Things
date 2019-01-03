/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>

void idle_task(void *arg)
{
#if (RHINO_CONFIG_CPU_NUM > 1)
    CPSR_ALLOC();
    klist_t *head;
    ktask_t *task_del;
    head =  &task_del_head;
#endif

    /* avoid warning */
    (void)arg;

#if (RHINO_CONFIG_USER_HOOK > 0)
    krhino_idle_pre_hook();
#endif

    while (RHINO_TRUE) {
#if (RHINO_CONFIG_CPU_NUM > 1)
        RHINO_CPU_INTRPT_DISABLE();
        if (head->next != head) {
            task_del = krhino_list_entry(head->next, ktask_t, task_del_item);
            if (task_del->cur_exc == 0) {
                klist_rm(&task_del->task_del_item);
                krhino_task_dyn_del(task_del);
            }
        }
        RHINO_CPU_INTRPT_ENABLE();
#endif
        /* type conversion is used to avoid compiler optimization */
        *(volatile idle_count_t*)(&g_idle_count[cpu_cur_get()]) = g_idle_count[cpu_cur_get()] + 1;

#if (RHINO_CONFIG_USER_HOOK > 0)
        krhino_idle_hook();
#endif

#if (RHINO_CONFIG_PWRMGMT > 0)
        cpu_pwr_down();
#endif
    }
}
