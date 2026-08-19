#include "CountMinSketch.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>

int main() {
  CountMinSketch tidySketch;

  // for (uint64_t  i = 0; i < 4294967295ULL; i++) {
  //    uint32_t value1 = static_cast<uint32_t>(i);
  //   for (uint64_t  j = i + 1; j < 4294967295ULL; j++) {
  //      uint32_t value2 = static_cast<uint32_t>(j);
  //     if (tidySketch.hash0(value1) == tidySketch.hash0(value2) &&
  //         tidySketch.hash1(value1) == tidySketch.hash1(value2) &&
  //         tidySketch.hash2(value1) == tidySketch.hash2(value2) &&
  //         tidySketch.hash3(value1) == tidySketch.hash3(value2)) {
  //       std::cout << "i: " << value1 << " j: " << value2 << "\n";
  //     }
  //   }
  // }

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