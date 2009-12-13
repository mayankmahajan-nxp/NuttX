/********************************************************************************
 * include/signal.h
 *
 *   Copyright (C) 2007-2009 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ********************************************************************************/

#ifndef __SIGNAL_H
#define __SIGNAL_H

/********************************************************************************
 * Included Files
 ********************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>
#include <time.h>      /* Needed for struct timespec */
#include <sys/types.h>

/********************************************************************************
 * Compilations Switches
 ********************************************************************************/

/********************************************************************************
 * Definitions
 ********************************************************************************/

/* Signal set management definitions and macros. */

#define NULL_SIGNAL_SET ((sigset_t)0x00000000)
#define ALL_SIGNAL_SET  ((sigset_t)0xffffffff)
#define MIN_SIGNO       0
#define MAX_SIGNO       31
#define GOOD_SIGNO(s)   ((((unsigned)(s))<=MAX_SIGNO))
#define SIGNO2SET(s)    ((sigset_t)1 << (s))

/* All signals are "real time" signals */

#define SIGRTMIN        0  /* First real time signal */
#define SIGRTMAX        31 /* Last real time signal */

/* A few of the real time signals are used within the OS.  They have
 * default values that can be overridden from the configuration file. The
 * rest are all user signals:
 */

#ifndef CONFIG_SIG_SIGUSR1
#define SIGUSR1         0  /* User signal 1 */
#else
#define SIGUSR1         CONFIG_SIG_SIGUSR1
#endif

#ifndef CONFIG_SIG_SIGUSR2
#define SIGUSR2         1  /* User signal 2 */
#else
#define SIGUSR2         CONFIG_SIG_SIGUSR2
#endif

#ifndef CONFIG_SIG_SIGALARM
#define SIGALRM         2  /* Default signal used with POSIX timers (used only */
                           /* no other signal is provided) */
#else
#define SIGALRM         CONFIG_SIG_SIGALARM
#endif

#ifndef CONFIG_DISABLE_PTHREAD
#ifndef CONFIG_SIG_SIGCONDTIMEDOUT
#define SIGCONDTIMEDOUT 3  /* Used in the implementation of pthread_cond_timedwait */
#else
#define SIGCONDTIMEDOUT CONFIG_SIG_SIGCONDTIMEDOUT
#endif
#endif

#ifdef CONFIG_SCHED_WORKQUEUE
#ifndef CONFIG_SIG_SIGWORK
#define SIGWORK         4  /* Used to wake up the work queue */
#else
#define SIGWORK         CONFIG_SIG_SIGWORK
#endif
#endif

/* sigprocmask() "how" definitions. Only one of the following can be specified: */

#define SIG_BLOCK       1  /* Block the given signals */
#define SIG_UNBLOCK     2  /* Unblock the given signals */
#define SIG_SETMASK     3  /* Set the signal mask to the current set */

/* struct sigaction flag values */

#define SA_NOCLDSTOP    1 /* Do not generate SIGCHILD when
                           * children stop (ignored) */
#define SA_SIGINFO      2 /* Invoke the signal-catching function
                           * with 3 args instead of 1
                           * (always assumed) */

/* These are the possible values of the signfo si_code field */

#define SI_USER         0 /* Signal sent from kill, raise, or abort */
#define SI_QUEUE        1 /* Signal sent from sigqueue */
#define SI_TIMER        2 /* Signal is result of timer expiration */
#define SI_ASYNCIO      3 /* Signal is the result of asynch IO completion */
#define SI_MESGQ        4 /* Signal generated by arrival of a message on an */
                          /* empty message queue */

/* Values for the sigev_notify field of struct sigevent */

#define SIGEV_NONE      0 /* No notification desired */
#define SIGEV_SIGNAL    1 /* Notify via signal */

/* Special values of sigaction (all treated like NULL) */

#define SIG_ERR         ((CODE void*)-1)
#define SIG_DFL         ((CODE void*)0)
#define SIG_IGN         ((CODE void*)0)

/********************************************************************************
 * Global Type Declarations
 ********************************************************************************/

/* This defines a set of 32 signals (numbered 0 through 31). */

typedef uint32 sigset_t;     /* Bit set of 32 signals */

/* This defines the type of the siginfo si_value field */

union sigval
{
  int       sival_int;       /* Integer value */
  FAR void *sival_ptr;       /* Pointer value */
};

/* This structure contains elements that define a queue signal. The following is
 * used to attach a signal to a message queue to notify a task when a message is
 * available on a queue
 */

struct sigevent
{
  ubyte        sigev_notify; /* Notification method: SIGEV_SIGNAL or SIGEV_NONE */
  ubyte        sigev_signo;  /* Notification signal */
  union sigval sigev_value;  /* Data passed with notification */
};

/* The following types is used to pass parameters to/from signal handlers */

struct siginfo
{
  ubyte        si_signo;     /* Identifies signal */
  ubyte        si_code;      /* Source: SI_USER, SI_QUEUE, SI_TIMER, SI_ASYNCIO, or SI_MESGQ */
  union sigval si_value;     /* Data passed with signal */
};

typedef struct siginfo siginfo_t;

/* The following structure defines the action to take for given signal */

struct sigaction
{
  union
  {
    CODE void (*_sa_handler)(int);
    CODE void (*_sa_sigaction)(int, FAR siginfo_t *, FAR void *);
  } sa_u;
  sigset_t         sa_mask;
  int              sa_flags;
};

#define sa_handler   sa_u._sa_handler
#define sa_sigaction sa_u._sa_sigaction

/********************************************************************************
 * Global Variables
 ********************************************************************************/

/********************************************************************************
 * Global Function Prototypes
 ********************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

EXTERN int kill(pid_t, int);
EXTERN int sigemptyset(FAR sigset_t *set);
EXTERN int sigfillset(FAR sigset_t *set);
EXTERN int sigaddset(FAR sigset_t *set, int signo);
EXTERN int sigdelset(FAR sigset_t *set, int signo);
EXTERN int sigismember(FAR const sigset_t *set, int signo);
EXTERN int sigaction(int sig, FAR const struct sigaction *act, FAR struct sigaction *oact);
EXTERN int sigprocmask(int how, FAR const sigset_t *set, FAR sigset_t *oset);
EXTERN int sigpending(FAR sigset_t *set);
EXTERN int sigsuspend(FAR const sigset_t *sigmask);
EXTERN int sigwaitinfo(FAR const sigset_t *set, FAR struct siginfo *value);
EXTERN int sigtimedwait(FAR const sigset_t *set, FAR struct siginfo *value,
                        FAR const struct timespec *timeout);
#ifdef CONFIG_CAN_PASS_STRUCTS
EXTERN int sigqueue(int pid, int signo, union sigval value);
#else
EXTERN int sigqueue(int pid, int signo, FAR void *sival_ptr);
#endif

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __SIGNAL_H */

