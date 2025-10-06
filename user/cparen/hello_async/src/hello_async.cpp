#include <cstdio>
#include <ccc_coro.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif

using namespace ccc;

void println(const char* msg);

std::coroutine_handle<> todo_;

void user()
{
  char buf[80];

  println("Hello, world!");

  // TODO: kick off async input-output loop
  async_and_forget([]() -> Coro<int> {
    println("about to suspend...");
    co_await callcch_dangerous([](coroutine_handle<> h){
      todo_ = h;
    });
    println("...done suspending!");
    co_return 0;
  });
}

#ifdef ARDUINO
void setup()
{
  // init serial
  Serial.begin(115200);
  delay(1000);

  user();
}
void loop()
{
  if(todo_) {
    todo_.resume();
  }
}
void println(const char* msg)
{
  Serial.println(msg);
}
#else
int main()
{
  user();
  while(todo_) {
    todo_.resume();
  }
}
void println(const char* msg)
{
  printf("%s\n", msg);
}
#endif