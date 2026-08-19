#include "CountMinSketch.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>

int main() {
  CountMinSketch tidySketch;

  tidySketch.update(0);
  assert(tidySketch.query(0) == 1);
  tidySketch.update(1443323093);
  assert(tidySketch.query(0) == 2);
  std::cout << "Pass Collision\n";

  assert(tidySketch.query(42) == 0);
  std::cout << "Pass test about dont update number\n";

  tidySketch.update(42);
  assert(tidySketch.query(42) >= 1);
  std::cout << "Pass test about 1 time update number\n";

  tidySketch.update(42);
  tidySketch.update(42);
  assert(tidySketch.query(42) >= 3);
  std::cout << "Pass test about 3 time update number\n";

  tidySketch.update(17);
  assert(tidySketch.query(17) >= 1);
  assert(tidySketch.query(42) >= 3);
  std::cout << "Pass test about 1 time update new number and first number's estimate dont change\n";
}