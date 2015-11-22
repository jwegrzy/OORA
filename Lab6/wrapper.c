#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sched.h>
#include "papi.h"

/* init lib */
int events[5] = {PAPI_FP_OPS, PAPI_STL_ICY, PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L2_DCH};
long long value = 0;
int eventSet = PAPI_NULL;
int papi_err;
bool papi_supported = true;

void papi_init(int eventNumber){
	if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
		fprintf(stderr, "PAPI is unsupported.\n");
		papi_supported = false;
	}

	if (PAPI_num_counters() < 5) {
		fprintf(stderr, "PAPI is unsupported.\n");
		papi_supported = false;
	}

	if ((papi_err = PAPI_create_eventset(&eventSet)) != PAPI_OK) {
		fprintf(stderr, "Could not create event set: %s\n", PAPI_strerror(papi_err));
	}

    /* force program to run on a single CPU */
    cpu_set_t my_set;        /* Define your cpu_set bit mask. */
    CPU_ZERO(&my_set);       /* Initialize it all to 0, i.e. no CPUs selected. */
	CPU_SET(0, &my_set);
	if (sched_setaffinity(0, sizeof(cpu_set_t), &my_set) != 0) {
		perror("sched_setaffinity error");
	}

	if ((papi_err = PAPI_add_event(eventSet, events[eventNumber])) != PAPI_OK ) {
		fprintf(stderr, "Could not add event: %s\n", PAPI_strerror(papi_err));
	}
}


/* start counters */

void papi_start(){
	if (papi_supported) {
		if ((papi_err = PAPI_start(eventSet)) != PAPI_OK) {
			fprintf(stderr, "Could not start counters: %s\n", PAPI_strerror(papi_err));
		}
	}

}


/* stop counters */

long long papi_stop(){

	if (papi_supported) {
		if ((papi_err = PAPI_stop(eventSet, &value)) != PAPI_OK) {
			return -1;
		}
		return value;
	}
	return -1;
}

