#include <cstdio>
#include <cstdlib>
#include <iostream>
int main() {
  int * x = NULL;
  char y = x[4];
  std::cout << y << std::endl;
  return EXIT_SUCCESS;
}
