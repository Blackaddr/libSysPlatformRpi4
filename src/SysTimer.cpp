#include "circle/timer.h"
#include "circle/sched/scheduler.h"
#include "sysPlatform/SysTypes.h"
#include "sysPlatform/SysTimer.h"

namespace SysPlatform {

static inline uint64_t __attribute__((unused)) read_pmccntr(void);
static inline uint32_t __attribute__((unused)) read_pmccntr32(void);

// milliseconds since program start
uint32_t SysTimer::millis()
{
	CTimer* timerPtr = CTimer::Get();
	if (!timerPtr) { return 1; }

	// convert ticks to seconds, then to milliseconds
	return (uint32_t)(timerPtr->GetTicks() * (1.0f / (float)HZ) * 1e3);
}

// microseonds since program start
uint32_t SysTimer::micros()
{
	CTimer* timerPtr = CTimer::Get();
	if (!timerPtr) { return 1; }

	// convert ticks to seconds, then to milliseconds
	return (uint32_t)(timerPtr->GetTicks() * (1.0f / (float)HZ) * 1e6);
}

// CPU cycle counts since program start
uint32_t SysTimer::cycleCnt32()
{
    return read_pmccntr32();
}

uint64_t SysTimer::cycleCnt64()
{
    return read_pmccntr();
}

void SysTimer::delayMilliseconds(unsigned x)
{
    CScheduler* ptr = CScheduler::Get(); if (ptr) { ptr->MsSleep(x); }
}

void SysTimer::delayMicroseconds(unsigned x)
{
    CScheduler* ptr = CScheduler::Get(); if (ptr) { ptr->usSleep(x); }
}

// CPU cycle counts since program start
static inline uint64_t __attribute__((unused)) read_pmccntr(void)
{
	uint64_t val;
	asm volatile("mrs %0, pmccntr_el0" : "=r"(val));
	return val;
}

static inline uint32_t __attribute__((unused)) read_pmccntr32(void)
{
	uint64_t val;
	asm volatile("mrs %0, pmccntr_el0" : "=r"(val));
	return (uint32_t)(val & 0xFFFFFFFFU);
}

}
