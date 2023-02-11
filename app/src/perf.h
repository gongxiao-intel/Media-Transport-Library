/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2022 Intel Corporation
 */

#include <pthread.h>

#include "app_base.h"
#include "log.h"

#ifndef _PERF_HEAD_H_
#define _PERF_HEAD_H_

int st_app_init_perf(struct st_perf_rep* perf, uint8_t* addr);
int st_app_uinit_perf(struct st_perf_rep* perf);

#endif

