#include "CountMinSketch.hpp"
#include "CountMinSketchTests.hpp"
#include "TinySketchHardware.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>

void testSoftwareAndHardware() {
  CountMinSketch software;
  TinySketchHardware hardware;

  std::mt19937 gen(12345);
  std::uniform_int_distribution<uint32_t> dist;

  for (int i = 0; i < 10000; ++i) {
    uint32_t value = dist(gen);

    software.update(value);
    hardware.update(value);

    uint32_t expected = software.query(value);
    uint32_t actual = hardware.query(value);

    if (expected != actual) {
      std::cout << "Mismatch at operation " << i << "\n";
      std::cout << "value: " << value << "\n";
      std::cout << "software: " << expected << "\n";
      std::cout << "hardware: " << actual << "\n";
      assert(false);
    }
  }
}

void testReset() {
  TinySketchHardware hardware;

  hardware.update(42);
  hardware.update(42);

  assert(hardware.query(42) == 2);

  hardware.reset();

  assert(hardware.query(42) == 0);
}

int main() {
  runInvariantTests<TinySketchHardware>();

  runTest(testSoftwareAndHardware, "testSoftwareAndHardware");
  runTest(testReset, "testReset");

  return 0;
}
