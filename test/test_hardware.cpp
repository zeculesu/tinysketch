#include "CountMinSketch.hpp"
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

    assert(software.query(value) == hardware.query(value));
  }
}

int main() {
  testSoftwareAndHardware();
  std::cout << "Test pass\n";
  return 0;
}