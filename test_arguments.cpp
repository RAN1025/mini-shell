#include <cstdlib>
#include <iostream>

int main(int argc, char ** argv) {
  if (argc < 2) {
    std::cout << "./test with arguments" << std::endl;
    return EXIT_FAILURE;
  }
  for (int i = 0; i < argc; i++) {
    std::cout << argv[i] << std::endl;
  }
  return EXIT_SUCCESS;
}
