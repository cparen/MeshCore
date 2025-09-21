// might need -std=c++20 to get async-await
// https://en.cppreference.com/w/cpp/language/coroutines.html

#ifndef FOXR_H
#define FOXR_H

#include <cstddef>

namespace foxr
{
	void println(const char* line);

	// Arduino lopp function for foxr library functions
	void loop();

	// Timer functionality
	class Timer;
	typedef void (*TimerPersonality)(Timer* timer);

	const size_t timer_max_alloc = 12;

	// low level timer allocation functions
	Timer* timer_alloc();
	void   timer_setpersonality(Timer* timer, TimerPersonality personality);
	void*  timer_getctx(Timer* timer);
	void   timer_schedule(Timer* timer, int period, bool repeat);

	// set a forever timer
	// TODO cleanup
	template <class Fn>
	int periodic(int everyMs, Fn callable) 
	{
		if (sizeof(Fn) > timer_max_alloc) {
			return 0;
		}
		Timer* timer = timer_alloc();
		if (!timer) {
			return 0;
		}
		new (timer_getctx(timer)) Fn(callable);
		timer_setpersonality(timer, [](Timer* t) {
			Fn& callable = *static_cast<Fn*>(timer_getctx(t));
			callable();
		});
		timer_schedule(timer, everyMs, true);
	}
}


#endif