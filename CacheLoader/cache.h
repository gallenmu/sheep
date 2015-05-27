#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define PADDING 6

/**
 * @brief Element of list.
 *
 * Element of single linked list containing:
 * - pointer to next element
 * - position of element in array
 * - padding to simulate different element sizes
 */
typedef struct element {
    struct element* n;
    uint64_t position;
    uint64_t pad[PADDING];
} element;

/**
 * Values for the linear congruential generator
 * used for generating randomly connected circular list.
 * 
 * m is chosen to be the size of the list. 
 * Conditions  [http://en.wikipedia.org/wiki/Linear_congruential_generator]:
 *   1. b & m coprime
 *   2. a - 1 divisable by all prime factors of m
 *   3. a - 1 is multiple of 4, if m is multiple of 4
 */
static uint64_t a = 5;
static uint64_t b = 13681;

/* forward declaration */
element* generate_list(uint64_t size_in_kb, uint8_t use_hugepages);
static element* prepare_list(uint32_t power, uint8_t use_hugepages);
static uint64_t fill_list(element* list, uint64_t size);
static void randomly_connect_list(element* list, uint64_t size, uint64_t seed);
uint64_t iterate_over_list(element* list, uint64_t iter); 
uint64_t iterate_over_list_from(element* list, uint64_t start, uint64_t iter); 

/**
 * @brief Generate a list.
 * 
 * Generate a list with n^2 KB size.
 *
 * @param size_in_kb size of list in KB
 * @param use_hugepages to allocate list in hugepages
 *
 * @return pointer to list.
 */
element* generate_list(uint64_t size_in_kb, uint8_t use_hugepages) {

	/* convert size to exponent and generate list */
	size_in_kb *= 1024;
	size_in_kb = (uint64_t) log2(size_in_kb);
	if (size_in_kb >= 6) {
		return prepare_list(size_in_kb - 6, use_hugepages);
	} else {
		return prepare_list(0, use_hugepages);
	}

}

/**
 * @brief Build the list.
 *
 * Allocating memory, filling array with data and randomly connect list.
 *
 * @param power 2^power specifying size of memory region
 * @param use_hugepages to allocate list in hugepages
 *
 * @return pointer to list
 */
static element* prepare_list(uint32_t power, uint8_t use_hugepages) {

	uint64_t size = pow(2, power);
	uint64_t memsize = sizeof(element) * size;
	printf ("Allocating list with %lu elements of size %lu B using a total memory of %lu KB.\n", size, sizeof(element), memsize);
	
	element* list = mmap(NULL, memsize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if (use_hugepages) {
		printf("Allocating memory in hugepages.\n");
		madvise(list, memsize, MADV_HUGEPAGE);
	} else {
		printf("Allocating memory in default sized pages.\n");
		madvise(list, memsize, MADV_NORMAL);
	}

	fill_list(list, size);
	randomly_connect_list(list, size, 0);
	return list;

}

/**
 * @brief Fill list with data.
 *
 * @param list to fill
 * @param size of list
 *
 * @return sum over all array elements
 */
static uint64_t fill_list(element* list, uint64_t size) {

	uint64_t sum = 0;
	uint64_t i;
	for (i = 0; i < size; ++i) {
		list[i].position = i;
		sum += list[i].position;
	}
	return sum;

}

/**
 * @brief Connecting a list circular and randomly.
 * 
 * @param list to connect
 * @param size of list
 * @param seed of random number generator
 */
static void randomly_connect_list(element* list, uint64_t size, uint64_t seed) {

	if (seed > size) seed = 0;

	uint64_t i = 0;
	uint64_t x0 = seed;
	for(i = 0; i < size; ++i) {
		uint64_t tmp = ((a * x0) + b) % size;
		list[x0].n = &list[tmp];
		x0 = tmp;
	}

}

/**
 * @brief Iterate n elements over list.
 *
 * @param list to iterate
 * @param iter number of elements to iterate
 *
 * @return sum of position value of travelled list elements
 */
inline uint64_t iterate_over_list(element* list, uint64_t iter) {

	uint64_t sum = 0;
	uint64_t i;
	for (i = 0; i < iter; ++i) {
		sum += list[i].position;
	}
	return sum;

}

/**
 * @brief Iterate n elements over list starting at specified element.
 *
 * @param list to iterate
 * @param start element to begin iteration
 * @param iter number of element to iterate
 *
 * @return position value of current list element
 */
inline uint64_t iterate_over_list_from(element* list, uint64_t start, uint64_t iter) {

	uint64_t i;
	element* currentElement = &list[start];
	for (i = 0; i < iter; ++i) {
		currentElement = currentElement->n;
	}
	return currentElement->position;

}
