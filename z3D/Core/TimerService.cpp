
#include "TimerService.h"
#include "Timer.h"

#include "MathFunc.h"

#include <math.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <mmsystem.h>
#include <intrin.h>

#pragma intrinsic(__rdtsc, __cpuid)

#undef min
#undef max

namespace z3D
{
	namespace Core
	{
		TimerService::TimerService()
		{
			int info[4];
			__cpuid(info, 0);

			int max_infotype = info[0];

			Timer t;

			memset(Timer::_cpu_id, 0, sizeof(Timer::_cpu_id));
			memcpy(Timer::_cpu_id, info + 1, 4);
			memcpy(Timer::_cpu_id + 4, info + 3, 4);
			memcpy(Timer::_cpu_id + 8, info + 2, 4);

			Timer::_cpu_speed = getCPUSpeed();

			SYSTEM_INFO sysinfo;
			GetSystemInfo(&sysinfo);
			Timer::_cpus = (uint8_t)sysinfo.dwNumberOfProcessors;
			DWORD proc_aff, sys_aff;
			GetProcessAffinityMask(GetCurrentProcess(), &proc_aff, &sys_aff);
			Timer::_affinities = BITCOUNT(sys_aff);

			Timer::_intel_speedstep = false;
			bool tsc = false;
			if(max_infotype >= 1)
			{
				__cpuid(info, 1);
				if(info[3] & (1<<4))
					tsc = true;
			}
			if(strcmp(Timer::_cpu_id, "GenuineIntel") == 0)
			{
				if(max_infotype >= 1)
				{
					__cpuid(info, 1);

					if(info[2] & (1<<7))
						Timer::_intel_speedstep = true;
				}
			}
			Timer::_qpc_freq = 0;
			QueryPerformanceFrequency((LARGE_INTEGER*)&Timer::_qpc_freq);

			if(tsc && Timer::_cpu_speed && Timer::_cpus == 1 && Timer::_affinities == 1 && !Timer::_intel_speedstep)
			{
				Timer::_timer_impl = Timer::TIMER_IMPL_TSC;
				return;
			}

			if(Timer::_qpc_freq)
			{
				// http://www.gamedev.net/community/forums/topic.asp?topic_id=357026

				bool bad_qpc = false;
				if(false)
				{
					uint64_t last;
					QueryPerformanceCounter((LARGE_INTEGER*)&last);
					uint64_t end = last + Timer::_qpc_freq / 2;
					while(last < end)
					{
						uint64_t cur;
						QueryPerformanceCounter((LARGE_INTEGER*)&cur);
						int64_t diff = (int64_t)(cur - last);
						if(diff < 0)
						{
							bad_qpc = true;
							break;
						}
						last = cur;										// no need to Sleep(0) to yield???
					}
				}
				Z_VERIFY( !bad_qpc && "QPC is buggy\n\nFor AMD cpu\ncheck http://www.amd.com/us-en/Processors/TechnicalResources/0,,30_182_871_13118,00.html\nand install Processer Driver");

				if(Timer::_qpc_freq == 1193182 || Timer::_qpc_freq == 3579545)						// PIT - 1193182, PMT - 3579545
				{
					Timer::_timer_impl = Timer::TIMER_IMPL_QPC;
					return;
				}
				else if(fabs((double)Timer::_cpu_speed / Timer::_qpc_freq - 1.0) > 0.05)			// HPET, http://www.gamedev.net/reference/articles/article2086.asp
				{
					Timer::_timer_impl = Timer::TIMER_IMPL_QPC;
					return;
				}
				else																				// sorry, hardcoded, just no simple way to detect whether using qpc is safe or not, i experience failed cases that they shouldn't
				{
					Timer::_timer_impl = Timer::TIMER_IMPL_QPC;
					return;
				}
			}

			Timer::_timer_impl = Timer::TIMER_IMPL_TGT;
			timeBeginPeriod(1);
		}
		TimerService::~TimerService()
		{
			if(Timer::_timer_impl == Timer::TIMER_IMPL_TGT)
				timeEndPeriod(1);
		}
		void			TimerService::getCPUTicks(uint64_t& cputicks, uint64_t& ticks)
		{
			uint64_t t;

			Z_STATIC_ASSERT( sizeof(t) == sizeof(LARGE_INTEGER), WRONG_ASSUMPTION );

			QueryPerformanceCounter((LARGE_INTEGER*)&t);
			do
			{
				QueryPerformanceCounter((LARGE_INTEGER*)&ticks);
			}while(ticks == t);

			cputicks = __rdtsc();
		}
		uint64_t		TimerService::getCPUSpeed()
		{
			uint64_t start, end;
			uint64_t start2, end2;

			DWORD proc_class;
			int thread_prior;

			DWORD_PTR proc_aff, sys_aff;

			GetProcessAffinityMask(GetCurrentProcess(), &proc_aff, &sys_aff);

			proc_class = GetPriorityClass(GetCurrentProcess());
			thread_prior = GetThreadPriority(GetCurrentThread());

			SetProcessAffinityMask(GetCurrentProcess(), 1);
			SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

			getCPUTicks(start, start2);
			getCPUTicks(end, end2);

			SetThreadPriority(GetCurrentThread(), thread_prior);
			SetPriorityClass(GetCurrentProcess(), proc_class);
			SetProcessAffinityMask(GetCurrentProcess(), proc_aff);

			uint64_t freq;
			Z_STATIC_ASSERT( sizeof(freq) == sizeof(LARGE_INTEGER), WRONG_ASSUMPTION );
			QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

			return (uint64_t)((end - start) / ((end2 - start2) / (double)freq));
		}
		uint64_t		TimerService::getCurCPUTick()
		{
			return __rdtsc();
		}
	};
};

