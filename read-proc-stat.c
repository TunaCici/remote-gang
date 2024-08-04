#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <unistd.h>
#include <fcntl.h>
#include <limits.h>


#define PROG_NAME "read-proc-stat"
#define PROC_STAT_BUFF_SIZE 128u // bytes. not sure what to set this :/

/*
 * Format of '/proc/<PID>/task/<TID>'
 *
 * See: https://www.kernel.org/doc/Documentation/filesystems/proc.txt
 * Also: $ man proc
 */

typedef struct proc_stat_t {
        int             pid;
        char            comm[NAME_MAX];
        char            state;

        int             ppid;
        int             pgrp;
        int             session;
        int             tty_nr;
        int             tgpid;

        unsigned int    flags;
        unsigned long   minflt;
        unsigned long   cminflt;
        unsigned long   majflt;
        unsigned long   cmajflt;

        unsigned long   utime;
        unsigned long   stime;
        unsigned int    cutime;
        unsigned int    cstime;

        long            priority;
        long            nice;
        long            num_threads;
        long            iter_value;

        unsigned long long      starttime;

        unsigned long   vsize;
        long            rss;
        unsigned long   rsslim;
        unsigned long   startcode;
        unsigned long   endcode;
        unsigned long   startstacl;
        unsigned long   kstkesp;
        unsigned long   kstkeip;
        unsigned long   signal;
        unsigned long   blocked;
        unsigned long   sigignore;
        unsigned long   sigcatch;
        unsigned long   wchan;
        unsigned long   nswap;
        unsigned long   cnswap;

        int             exit_signal;
        int             processor;
        unsigned int    rt_priority;
        unsigned int    policy;

        unsigned long long      delayacct_blkio_ticks;
        
        unsigned long   guest_time;
        long            cguest_time;
        unsigned long   start_data;
        unsigned long   end_data;
        unsigned long   start_brk;
        unsigned long   arg_start;
        unsigned long   arg_end;
        unsigned long   env_start;
        unsigned long   env_end;

        int exit_code;
} proc_stat_t;

/*
 * ?
 */
void read_proc_stat(proc_stat_t *proc, int pid, int tid)
{
        static int fd = 0;
        static mode_t mode = O_RDONLY;
        static char buff[PROC_STAT_BUFF_SIZE] = {0};

        char pathname[NAME_MAX] = {'\0'};

        if (!proc) {
                return;
        }

        sprintf(pathname, "/proc/%d/task/%d/stat", pid, tid);

        if (!fd) {
                fd = open(pathname, mode);

                if (!fd) {
                        fprintf(stderr, "%s: unable to open '%s'\n",
                                PROG_NAME, pathname);
                        exit(EXIT_FAILURE);
                }
        }

        ssize_t offset = lseek(fd, 0, SEEK_SET);
        ssize_t bytes_read = read(fd, buff, PROC_STAT_BUFF_SIZE);

        if (offset == -1 || bytes_read == -1) {
                fprintf(stderr, "%s: unable to read '%s'",
                        PROG_NAME, pathname);
                exit(EXIT_FAILURE);
        }

        sscanf(buff, "%d %s %c "
                     "%d %d %d %d %d "
                     "%u %lu %lu %lu %lu "
                     "%lu %lu %u %u "
                     "%ld %ld %ld %ld "
                     "%llu "
                     "%lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu "
                     "%d %d %u %u "
                     "%llu "
                     "%lu %ld %lu %lu %lu %lu %lu %lu %lu "
                     "%d",
               &proc->pid,
               proc->comm,
               &proc->state,

               &proc->ppid,
               &proc->pgrp,
               &proc->session,
               &proc->tty_nr,
               &proc->tgpid,

               &proc->flags,
               &proc->minflt,
               &proc->cminflt,
               &proc->majflt,
               &proc->cmajflt,

               &proc->utime,
               &proc->stime,
               &proc->cutime,
               &proc->cstime,

               &proc->priority,
               &proc->nice,
               &proc->num_threads,
               &proc->iter_value,

               &proc->starttime,

               &proc->vsize,
               &proc->rss,
               &proc->rsslim,
               &proc->startcode,
               &proc->endcode,
               &proc->startstacl,
               &proc->kstkesp,
               &proc->kstkeip,
               &proc->signal,
               &proc->blocked,
               &proc->sigignore,
               &proc->sigcatch,
               &proc->wchan,
               &proc->nswap,
               &proc->cnswap,

               &proc->exit_signal,
               &proc->processor,
               &proc->rt_priority,
               &proc->policy,

               &proc->delayacct_blkio_ticks,

               &proc->guest_time,
               &proc->cguest_time,
               &proc->start_data,
               &proc->end_data,
               &proc->start_brk,
               &proc->arg_start,
               &proc->arg_end,
               &proc->env_start,
               &proc->env_end,

               &proc->exit_code);
}

int main(int argc, char **argv)
{
        proc_stat_t proc = {0};

        if (argc != 2) {
                fprintf(stderr, "%s: usage ./%s <PID>\n", PROG_NAME, PROG_NAME);
                exit(EXIT_FAILURE);
        }

        int pid = atoi(argv[1]);
        int tid = pid; // Main thread

        while (1) {
                read_proc_stat(&proc, pid, tid);

                printf("%s: utime: %lu\n", proc.comm, proc.utime);

                usleep(1000 * 1000);
        }

        return EXIT_SUCCESS;
}
