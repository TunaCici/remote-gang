#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <linux/prctl.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sched.h>

#define PROG_NAME "ping-pong"
#define BCAS(ptr, expected, desired) \
                __atomic_compare_exchange_n(ptr, expected, desired, 0, \
                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

/* These are variables, but should be used as 'const's due to BCAS */
static int OP_NONE = 0;
static int OP_PING = 1;
static int OP_PONG = 2;

/* Atomics */
static int table = 0;
static unsigned long long ping_missed = 0;
static unsigned long long pong_missed = 0;

static long get_usec()
{
        static struct timeval currentTime = {0};

	gettimeofday(&currentTime, NULL);

	return currentTime.tv_sec * (long) 1e6 + currentTime.tv_usec;       
}

static void *ping_guy(void *arg)
{
        unsigned int cycle = *((unsigned int*) arg);
        cpu_set_t set = {0};

        CPU_ZERO(&set);
        CPU_SET(1, &set);

        int res = prctl(PR_SET_NAME, "ping_thread");
        res = sched_setaffinity(gettid(), sizeof(set), &set);

        /* Start ping ponging */
        if (table == OP_NONE) {
                BCAS(&table, &OP_NONE, OP_PING);
        }

        long begin_time = get_usec();

        while (1) {
                begin_time = get_usec();

                if (table == OP_PONG) {
                        BCAS(&table, &OP_PONG, OP_PING);
                }

                long elapsed = (get_usec() - begin_time);
                if (cycle < elapsed) {
                        ping_missed++;
                        elapsed = 0;
                }

                usleep(cycle - elapsed);
        }
}

static void *pong_guy(void *arg)
{
        unsigned int cycle = *((unsigned int*) arg);
        cpu_set_t set = {0};

        CPU_ZERO(&set);
        CPU_SET(1, &set);

        int res = prctl(PR_SET_NAME, "pong_thread");
        res = sched_setaffinity(gettid(), sizeof(set), &set);

        long begin_time = get_usec();

        while (1) {
                long begin_time = get_usec();

                if (table == OP_PING) {
                        BCAS(&table, &OP_PING, OP_PONG);
                }

                long elapsed = (get_usec() - begin_time);
                if (cycle < elapsed) {
                        pong_missed++;
                        elapsed = 0;
                }

                usleep(cycle - elapsed);
        }
}

int main(int argc, char **argv)
{
        if (argc != 3) {
                fprintf(stderr,
                        "%s: usage: "
                        "./%s <PING CYCLE (us)> <PONG CYCLE (us)>\n",
                        PROG_NAME, PROG_NAME);
                exit(EXIT_FAILURE);
        }

        unsigned int ping_cycle = strtoul(argv[1], NULL, 10);
        unsigned int pong_cycle = strtoul(argv[2], NULL, 10);

        pthread_t ping_thr = 0;
        pthread_t pong_thr = 0;

        pthread_attr_t ping_thr_attr = {0};
        pthread_attr_t pong_thr_attr = {0};

        struct sched_param ping_thr_param = {0};
        struct sched_param pong_thr_param = {0};

        int ping_thr_policy = 0;
        int pong_thr_policy = 0;

        /* Thread initialziation */
        int res = pthread_attr_init(&ping_thr_attr);
        res = pthread_attr_init(&pong_thr_attr);

        res = pthread_getschedparam(ping_thr, &ping_thr_policy,
                                    &ping_thr_param);
        res = pthread_getschedparam(pong_thr, &pong_thr_policy,
                                    &pong_thr_param);
        
        ping_thr_param.sched_priority = 20;
        pong_thr_param.sched_priority = 20;

        res = pthread_attr_setschedparam(&ping_thr_attr, &ping_thr_param);
        res = pthread_attr_setschedparam(&pong_thr_attr, &pong_thr_param);

        if (pthread_create(&ping_thr, &ping_thr_attr, ping_guy, &ping_cycle)) {
                fprintf(stderr, "%s: failed to create ping_guy\n", PROG_NAME);
                exit(EXIT_FAILURE);
        }
        
        if (pthread_create(&pong_thr, &pong_thr_attr, pong_guy, &pong_cycle)) {
                fprintf(stderr, "%s: failed to create pong_guy\n", PROG_NAME);
                exit(EXIT_FAILURE);
        }

        while (1) {
                printf("\033[H\033[J");
                printf("main: ping_missed %llu pong-missed %llu\n", ping_missed, pong_missed);

                usleep(1e6);
        }

        return EXIT_SUCCESS;
}
