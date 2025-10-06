#ifndef ccc_coro_h
#define ccc_coro_h

#include <optional>
#include <coroutine>

namespace ccc
{
  // template <class T>
  // class Prom
  // {
  // public:

  // }; 


  // https://en.cppreference.com/w/cpp/language/coroutines.html
  template <class T>
  class Coro {
  public:
    class promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    class promise_type
    {
      std::optional<T> value_;
      handle_type handle_;
    public:
      std::suspend_never initial_suspend() { return {}; }
      std::suspend_never final_suspend() { return {}; }
      void return_value(T value) {
        value_ = std::move(value);
        if (handle_) {
          auto h = handle_;
          handle_ = handle_type();
          h.resume();
        }
      }
      void unhandled_exception() {
        throw std::exception("TODO");
      }
      Coro get_return_object() { 
        if (value_.has_value()) {
          return Coro(std::move(value_));
        }
        return Coro(handle_type::from_promise(*this));
      }
      bool has_value() { return value_.has_value(); }
      T&& value() {
        if (!value_.has_value()) {
          throw std::exception("attempted to access promise value before complete");
        }
        return std::move(value_.value());
      }
      void subscribe(handle_type h) {
        if (value_.has_value() || handle_) {
          throw std::exception("bad subscribe()");
        } 
        handle_ = h;
      }
    };
    Coro(T&& value) : value_(value) {}
    Coro(handle_type h) : handle_(h) {}
    ~Coro() {
      if (handle_) {
        handle_.destroy();
      }
    }
    Coro (const Coro&) = delete;
    Coro& operator= (const Coro&) = delete;

    auto operator co_await() { return *this; }
    bool await_ready() { 
      // test if we have a prompt value
      if (value_->has_value())
        return true;
      return handle_.promise().has_value();
    }
    void await_suspend(handle_type h) {
      handle_.promise().subscribe(h);
    }
    T&& await_resume() {
      value_.has_value() ? value_.value() : handle_.promise().value();
    }
    T&& value() { return value_.has_value() ? value_.value() : handle_.promise().value(); }

  private:
    std::optional<T> value_;
    handle_type handle_;
  };

  //
  // Calls 'callback' with a coroutine handle to resume the caller's await.
  //
  template <class Fn>
  auto callcch_dangerous(Fn callback)
  {
    struct awaitable
    {
      Fn callback;
      bool await_ready() { return false; }
      void await_suspend(std::coroutine_handle<> h)
      {
        callback(h);
      }
      void await_resume() {}
    };
    return awaitable { std::move(callback) };
  }

  //
  // Start callback and ignore result
  //
  // EXAMPLE
  // async_and_forget([]() -> Coro<void>{ ... co_await ... });
  template <class Fn>
  void async_and_forget(Fn callback)
  {
    (void)callback();
  }
}

#endif