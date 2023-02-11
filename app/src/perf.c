/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2022 Intel Corporation
 */

#define _GNU_SOURCE /* See feature_test_macros(7) */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <sched.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <dirent.h>

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <numa.h>

#include <sys/shm.h>
#include <sys/socket.h>

#include <argp.h>

#include <linux/errqueue.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
//#include <linux/ip.h>
#include <linux/net_tstamp.h>
#include <linux/socket.h>
#include <linux/sockios.h>

#include <poll.h>

#include <sys/timerfd.h>
#include <sys/timex.h>

#include <sys/types.h> 
#include <sys/socket.h> 

#include <stdarg.h>

#include <semaphore.h>

#include "perf.h"

#define PERF_DB_PORT (8998)

static void* perf_thread_func(void* arg) {
  struct st_perf_rep* perf = arg;
  int sock;
  int ret;
  struct st_perf_msg perf_msg;
  struct timespec time_frame;

  if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    err("%s(%d), create socket fail: %d\n", __func__, 0, errno);
    return NULL;;
  }

  perf_msg.flag = 1;

  while (!perf->perf_thread_stop) {
    st_pthread_mutex_lock(&perf->perf_wake_mutex);
    if (!perf->perf_thread_stop)
      st_pthread_cond_wait(&perf->perf_wake_cond, &perf->perf_wake_mutex);
    st_pthread_mutex_unlock(&perf->perf_wake_mutex);

    time_frame = perf->perf_buf[perf->cons_indx].time_frame;
    perf_msg.time_record = (uint64_t)time_frame.tv_sec * NS_PER_S + time_frame.tv_nsec;
    perf_msg.timestamp = perf->perf_buf[perf->cons_indx].timestamp;
    perf_msg.freq++;

    ret = sendto(sock, &perf_msg, sizeof(struct st_perf_msg), 0, (struct sockaddr *)&(perf->addr), sizeof(perf->addr));
    if (ret < 0) {
      err("%s(%d), sendto socket fail: %d\n", __func__, 0, errno);
      break;
    }

    perf->cons_indx ++;
    if(perf->cons_indx >= PERF_BUF_LEN) {
        perf->cons_indx = 0;
    }

    if (perf->prod_indx != perf->cons_indx) {
        st_pthread_mutex_lock(&(perf->perf_wake_mutex));
        st_pthread_cond_signal(&(perf->perf_wake_cond));
        st_pthread_mutex_unlock(&(perf->perf_wake_mutex));
    }
  }

  perf->perf_thread_stop = true;
  close(sock);

  info("%s(%d), stop\n", __func__, 0);
  return NULL;
}

int st_app_uinit_perf(struct st_perf_rep* perf) {

  if (!perf) {
    return 0;
  }

  perf->perf_thread_stop = true;

  if (perf->perf_thread) {
    /* wake up the thread */
    st_pthread_mutex_lock(&perf->perf_wake_mutex);
    st_pthread_cond_signal(&perf->perf_wake_cond);
    st_pthread_mutex_unlock(&perf->perf_wake_mutex);
    info("%s(%d), wait perf thread stop\n", __func__, 0);
    pthread_join(perf->perf_thread, NULL);
  }

  st_pthread_mutex_destroy(&perf->perf_wake_mutex);
  st_pthread_cond_destroy(&perf->perf_wake_cond);

  return 0;
}

int st_app_init_perf(struct st_perf_rep* perf, uint8_t* addr) {
  int ret;

  if (!perf) {
    return -ENOMEM;
  }

  st_pthread_mutex_init(&perf->perf_wake_mutex, NULL);
  st_pthread_cond_init(&perf->perf_wake_cond, NULL);

  perf->addr.sin_family = AF_INET;
  perf->addr.sin_port = htons(PERF_DB_PORT);
  memcpy(&(perf->addr.sin_addr.s_addr), addr, ST_IP_ADDR_LEN);  

  info("%s(%d), perf to %d.%d.%d.%d\n", __func__, 0, addr[0], addr[1], addr[2], addr[3]);

  ret = pthread_create(&perf->perf_thread, NULL, perf_thread_func, perf);
  if (ret < 0) {
    err("%s(%d), create perf thread fail: %d\n", __func__, 0, ret);
    st_app_uinit_perf(perf);

    return ret;
  }

  return 0;
}

