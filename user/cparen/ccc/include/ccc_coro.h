#ifndef ccc_coro_h
#define ccc_coro_h

namespace ccc
{
  int add(int a, int b) { return a + b; }
  /*
  template <class T>
  class Prom
  {
  public:

  }; 


  // https://en.cppreference.com/w/cpp/language/coroutines.html
  template <class T>
  class Coro {
    std::option<T> value_;
    handle_type handle_;
    std::function<void()>

  public:
    class promise_type 
    {
      std::option<T> value_;
    public:
      std::suspend_never initial_suspend() { return {}; }
      std::suspend_never final_suspend() { return {}; }
      void return_value(T value) {
        value_ = value;
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
    };
    using handle_type = std::coroutine_handle<promise_type>;
    Coro(T&& value) : value_(value) {}
    Coro(handle_type h) : handle(h) {}
    ~Coro() {
      if (handle_) {
        handle_.destroy();
      }
    }

    bool has_value() { return value_.has_value(); }
    T& value() { return value_.value(); }
    void subscribe(callback_type callback) {
      if (has_value()) {
        callback(value());
      } else {
        push(subscriber_, callback);
      }
    }
  };
  */
}

#endif