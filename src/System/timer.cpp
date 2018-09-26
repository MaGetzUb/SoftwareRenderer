/*
Copyright © 2018, Marko Ranta
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "timer.hpp"
#include <cassert>

#if _WIN32
#include <windows.h>
double Timer() {
	static struct Performance {
		double frequency;
		Performance() {
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			frequency = (double)freq.QuadPart / TIMER_PRECISION;
		}
	} performance;
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);
	return (double)current.QuadPart / performance.frequency;
}
#endif 

#ifdef __linux__
#include <sys/time.h>
#include <cstdio>

double Timer() {

		constexpr double s = TIMER_PRECISION_MICROSECOND / TIMER_PRECISION;

		static struct Start {
			double time;
			Start() {

				timeval timePoint;
				gettimeofday(&timePoint, NULL);
				time = (double)timePoint.tv_sec * TIMER_PRECISION_MICROSECOND + (double)timePoint.tv_usec;
			}
		} start;
		
		timeval tval;
		gettimeofday(&tval, NULL);

		double now = tval.tv_sec * TIMER_PRECISION_MICROSECOND + tval.tv_usec;


		return (now - start.time) / s;


}
#endif 

