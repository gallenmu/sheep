#define _GNU_SOURCE

#include <inttypes.h>
#include <stdio.h>
#include <sched.h>

#include "precise_timer.h"

#define SAMPLE_SIZE 10
#define ITERATION_SIZE 100000

int main( int argc, const char* argv[] ) {

    volatile int counter = 0;
    uint64_t result[SAMPLE_SIZE][ITERATION_SIZE];
    uint64_t max = 0, min = 0, integrator = 0, variator = 0, avg = 0, var = 0;
    uint32_t wait = 100;
    int i, ii;
	int count = 0;

	/* bind process to core 0 */
    cpu_set_t my_set;
    CPU_ZERO(&my_set);
    CPU_SET(0, &my_set);
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set);

    /* perform measurements with different values for wait */
    for (wait = 0; wait <= 1000; wait+=1) {
            
		integrator = 0;
        variator = 0;
        avg = 0;

        printf("Wait cycles count: %i\n", wait);
            
		/* measurement loop */
        for (i = 0;  i < SAMPLE_SIZE; ++i) {

			for (ii = 0; ii < ITERATION_SIZE; ++ii) {
								
				uint64_t start_timestamp = start_measurement();
					
				/* begin of function to measure */
				wait_cycles(wait);
				/* end of function to measure */
					
				uint64_t end_timestamp = stop_measurement();
				result[i][ii] = (end_timestamp - start_timestamp);

			}
			
		}
			
		/* print out statistics */
		for (i = 0; i < SAMPLE_SIZE; ++i) {
			var = 0;
			max = 0;
			min = 0xFFFFFFFFFFFFFFFF;
			integrator = 0;
			count = 0;
			variator = 0;
			for (ii = 0; ii < ITERATION_SIZE; ++ii) {
				if (result[i][ii] > max)  max = result[i][ii];
				if (result[i][ii] < min)  min = result[i][ii];
				integrator += result[i][ii];
			}
			avg = integrator / ITERATION_SIZE;
			integrator = 0;
			for (ii = 0; ii < ITERATION_SIZE; ++ii) {
				/* simple data sanity check */
				if (result[i][ii] < 3 * min) {
					count++;
					integrator += result[i][ii];
				}	
			}
			avg = integrator / count;
			for (ii = 0; ii < ITERATION_SIZE; ++ii) {
				/* simple data sanity check */
				if (result[i][ii] < 3 * min) {
				   variator += ((result[i][ii]-avg)*(result[i][ii]-avg));
				}
			}
			printf("Used %7"PRIu32" ", count);
			var = variator / count;
			printf("values in loop iteration [%2"PRIu32"]: avg:%10"PRIu64", var:%10"PRIu64", max:%10"PRIu64", min:%10"PRIu64"\n", i, avg, var, max, min);
		}
				
        printf("-----------------------------------------------\n\n");
    }

    return 0;

}
