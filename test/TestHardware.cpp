#include "CountMinSketch.hpp"
#include "CountMinSketchTests.hpp"
#include "DataGenerator.hpp"
#include "TinySketchHardware.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>

template <std::size_t Rows, std::size_t Cols, std::size_t Width>
void testSoftwareAndHardware(WorkloadType workload) {
  using value_type = typename CountMinSketch<Rows, Cols, Width>::value_type;

  CountMinSketch<Rows, Cols, Width> software;
  TinySketchHardware<Rows, Cols, Width> hardware;

  DataGenerator<value_type> gen;
  auto data = gen.generate(workload);

  for (std::size_t i = 0; i < data.size(); ++i) {
    auto value = data[i];

    software.update(value);
    hardware.update(value);

    auto expected = software.query(value);
    auto actual = hardware.query(value);

    if (expected != actual) {
      std::cout << "Mismatch at operation " << i << "\n";
      std::cout << "value:    " << value << "\n";
      std::cout << "software: " << expected << "\n";
      std::cout << "hardware: " << actual << "\n";
      assert(false);
    }
  }
}

template <typename Sketch> void testReset() {
  Sketch hardware;

  hardware.update(42);
  hardware.update(42);

  assert(hardware.query(42) == 2);

  hardware.reset();

  assert(hardware.query(42) == 0);
}

int main() {
  constexpr auto ROWS = TINY_ROWS;
  constexpr auto COLS = TINY_COLS;
  constexpr auto WIDTH = TINY_WIDTH;

  std::cout << "Config: " << ROWS << "x" << COLS << "x" << WIDTH << "\n";

  runInvariantTests<TinySketchHardware<ROWS, COLS, WIDTH>>();

  using Sketch = TinySketchHardware<ROWS, COLS, WIDTH>;

  runTest(
      [] { testSoftwareAndHardware<ROWS, COLS, WIDTH>(WorkloadType::Uniform); },
      "testSoftwareAndHardware Uniform Workload");
  runTest(
      [] {
        testSoftwareAndHardware<ROWS, COLS, WIDTH>(WorkloadType::HeavyHitters);
      },
      "testSoftwareAndHardware Heavy Hitters Workload");
  runTest(
      [] { testSoftwareAndHardware<ROWS, COLS, WIDTH>(WorkloadType::Skewed); },
      "testSoftwareAndHardware Skewed Workload");

  runTest(testReset<Sketch>, "testReset");

  runTest(testReset<TinySketchHardware<ROWS, COLS, WIDTH>>, "testReset");
  
  return 0;
}