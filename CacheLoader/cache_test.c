#define _GNU_SOURCE

#include <inttypes.h>
#include <stdio.h>
#include <sched.h>
#include <math.h>

#include "cache.h"
#include "../CPULoader/precise_timer.h"

#define SAMPLE_SIZE 10

void testrun (uint32_t power) {

	double result[SAMPLE_SIZE];
	double integrator = 0, variator = 0, avg = 0;
	int i;
	uint32_t startpower = 1;
	uint32_t walk_iterations = 1000000;

	while (startpower < 20) {
		
		variator = 0;
		integrator = 0;
		
		/* prepare dataset & load to cache */
		element* list = generate_list(pow(2, startpower), 1);
		iterate_over_list_from(list, 0, walk_iterations);

		/* measurement loop */
		for (i = 0;  i < SAMPLE_SIZE; ++i) {


			uint64_t start_timestamp = start_measurement();

			/* begin of function to measure */
			uint64_t res = iterate_over_list_from(list, 0, walk_iterations);
			/* end of function to measure */

			uint64_t end_timestamp = stop_measurement();

			/* save stats */
			result[i] = (end_timestamp - start_timestamp) / walk_iterations;
			integrator += result[i];

		}

		/* print out statistics */
		printf("\nIteration over %i elements in %f KB \n", walk_iterations, (pow(2, startpower)));
		avg = integrator / SAMPLE_SIZE;
		for (i = 0; i < SAMPLE_SIZE; ++i) {	
			uint64_t single = ((result[i]-avg)*(result[i]-avg));
			variator += single;
			printf("\tMeasured cycles of loop iteration %i: %f\n", i, result[i]);
		}
		double variance = variator / SAMPLE_SIZE;
		printf("\n\tAverage:%f\n", avg);
		printf("\tVariance: %f\n\n", variance);

		startpower++;

	}

}

int simple_testrun (uint32_t power) {

	int size = pow(2, power);
	element* list = generate_list(size, 1);
	int i;
	for (i = 0; i < size; ++i) {
		printf("list element [%i] = %i\n", i,  iterate_over_list_from(list, 0, i));
	}
	printf("array integrated %lld \n", iterate_over_list(list, size));
}

int main(int argc, const char* argv[]) {

	/* bind process to core 0 */
	cpu_set_t my_set;
	CPU_ZERO(&my_set);
	CPU_SET(0, &my_set);
	sched_setaffinity(0, sizeof(cpu_set_t), &my_set);

	/* cache test */
	testrun(23);
	//simple_testrun(5);

	return 0;

}
