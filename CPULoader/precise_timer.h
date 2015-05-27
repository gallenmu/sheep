#define _GNU_SOURCE

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

#include <stdio.h>
#include <inttypes.h>
#include <stdio.h>

static uint64_t eal_tsc_resolution_hz = 0;
static 	union {
	uint64_t tsc_64;
	struct {
		uint32_t lo_32;
		uint32_t hi_32;
	};
} tsc;
static uint64_t start;
static uint64_t end;

/* forward declaration */
inline uint64_t start_measurement(void);
inline uint64_t read_rdtsc(void);
inline uint64_t read_rdtscp(void);
inline uint64_t stop_measurement(void);
inline void wait_cycles(uint32_t cycles);
uint64_t rdtsc_hz(void);

/**
 * @brief Read content of TSC register with enforced serialization.
 * Taken from "How to Benchmark Code Execution Times on Intel IA-32 and IA-64 Instruction Set Architectures"
 *
 * @return Content of TSC register.
 */
inline uint64_t
start_measurement(void) {

	asm volatile ("CPUID\n\t" 
			"RDTSC\n\t" 
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (tsc.hi_32),
			"=r" (tsc.lo_32):: "%rax", "%rbx", "%rcx", "%rdx"); 

	return tsc.tsc_64;

}

/**
 * @brief Read value of TSC register without enforced serialization.
 * Taken from "How to Benchmark Code Execution Times on Intel IA-32 and IA-64 Instruction Set Architectures"
 *
 * @return Value of TSC register
 */
inline uint64_t
read_rdtsc(void) {

	asm volatile("RDTSC\n\t" 
			"mov %%edx, %0\n\t" 
			"mov %%eax, %1\n\t" 
			: "=r" (tsc.hi_32), 
			"=r" (tsc.lo_32):: "%rax", "%rbx", "%rcx", "%rdx");

	return tsc.tsc_64;

}

/**
 * @brief Read value of TSC register without enforced serialization.
 * Taken from "How to Benchmark Code Execution Times on Intel IA-32 and IA-64 Instruction Set Architectures"
 *
 * @return Value of TSC register
 */
inline uint64_t
read_rdtscp(void) {

	asm volatile("RDTSCP\n\t"
			"mov %%edx, %0\n\t" 
			"mov %%eax, %1\n\t" 
			: "=r" (tsc.hi_32), 
			"=r" (tsc.lo_32):: "%rax", "%rbx", "%rcx", "%rdx");

	return tsc.tsc_64;

}

/**
 * @brief Read value of TSC register with enforced serialization.
 * Taken from "How to Benchmark Code Execution Times on Intel IA-32 and IA-64 Instruction Set Architectures"
 *
 * @return Value of TSC register
 */
inline uint64_t
stop_measurement(void) {

	asm volatile("RDTSCP\n\t" 
			"mov %%edx, %0\n\t" 
			"mov %%eax, %1\n\t" 
			"CPUID\n\t": "=r" (tsc.hi_32), 
			"=r" (tsc.lo_32):: "%rax", "%rbx", "%rcx", "%rdx");

	return tsc.tsc_64;

}

/**
 * @brief Wait for specified number of cycles doing busy wait.
 * 
 * @param cycles Number of cycles to wait (value depends on the actual CPU architecture).
 * 
 * @return Value of TSC at the end of the wait.
 */
inline void 
wait_cycles(uint32_t cycles) {

	asm volatile("mov %0, %%ecx\n\t"
			"inc %%ecx\n\t"
			"1: dec %%ecx\n\t"
			"cmp $0, %%ecx\n\t"
			"jnz 1b"::"r" (cycles));

}

/**
 * @brief Read the clock cycles per second of TSC.
 * Taken from Intel DPDK.
 *
 * @return Ticks per second of TSC.
 */
uint64_t
rdtsc_hz(void)
{
	if (likely(eal_tsc_resolution_hz != 0)) {
		return eal_tsc_resolution_hz;
	}

	char line[256];
	FILE *stream;
	double dmhz;

	stream = fopen("/proc/cpuinfo", "r");
	if (!stream) {
		printf("WARNING: Unable to open /proc/cpuinfo\n");
		return -1;
	}

	while (fgets(line, sizeof line, stream)) {
		if (sscanf(line, "cpu MHz\t: %lf", &dmhz) == 1) {
			eal_tsc_resolution_hz = (uint64_t)(dmhz * 1000000UL);
			break;
		}
	}

	fclose(stream);

	if (!eal_tsc_resolution_hz) {
		printf("WARNING: Cannot read CPU clock from cpuinfo\n");
		return -1;
	}
	return eal_tsc_resolution_hz;
}
