#include <cstdio>

void on_start();

int main()
{
  on_start();
}

namespace h
{
  void println(const char* s) 
  {
    printf("%s\n", s);
  }
}
