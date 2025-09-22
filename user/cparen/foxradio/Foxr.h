// might need -std=c++20 to get async-await
// https://en.cppreference.com/w/cpp/language/coroutines.html

#ifndef FOXR_H
#define FOXR_H

#include <cstddef>
#include <coroutine>
#include <new>

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
    return 1;
  }

  // set a forever timer
  // TODO cleanup
  template <class Fn>
  int once(int delayMs, Fn callable) 
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
    timer_schedule(timer, delayMs, false);
    return 1;
  }
  
#if __cpp_impl_coroutine
  class BasicCoroutine {
  public:
    struct Promise {
      BasicCoroutine get_return_object() { return BasicCoroutine {}; }
      void unhandled_exception() noexcept { }
      void return_void() noexcept { }
      std::suspend_never initial_suspend() noexcept { 
        return {};
      }
      std::suspend_never final_suspend() noexcept { 
        return {};
      }
    };
    using promise_type = Promise;
  };

  // awaits the execution of std::once
  struct later
  {
    int delayMs;
    later(int delayMs) : delayMs(delayMs) {}
    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
      foxr::once(delayMs, [=](){
        handle.resume();
      });
    }
    void await_resume() const noexcept { }

    auto operator co_await() {
      return *this;
    }
  };
#endif

}


#endif