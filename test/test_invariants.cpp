#include "CountMinSketch.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>

void testCollisionCheck() {
  CountMinSketch tinySketch;
  tinySketch.update(0);
  assert(tinySketch.query(0) == 1);
  tinySketch.update(1443323093);
  assert(tinySketch.query(0) == 2);
}

void testNonUpdatedElement() {
  CountMinSketch tinySketch;
  assert(tinySketch.query(42) == 0);
}

void testSingleUpdate() {
  CountMinSketch tinySketch;
  tinySketch.update(42);
  assert(tinySketch.query(42) == 1);
}

void testMultipleUpdates() {
  CountMinSketch tinySketch;
  tinySketch.update(42);
  tinySketch.update(42);
  tinySketch.update(42);
  assert(tinySketch.query(42) == 3);
}

void testIndependentElements() {
  CountMinSketch tinySketch;
  tinySketch.update(42);
  tinySketch.update(42);
  tinySketch.update(42);
  tinySketch.update(17);
  assert(tinySketch.query(17) == 1);
  assert(tinySketch.query(42) == 3);
}

void testExactReference() {
  CountMinSketch tinySketch;
  std::unordered_map<uint32_t, uint32_t> exact;
  const size_t NUM_OPERATIONS = 1000;

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<uint32_t> dis(0, 99);

  for (size_t i = 0; i < NUM_OPERATIONS; i++) {
    uint32_t value = dis(gen);
    exact[value] += 1;
    tinySketch.update(value);
  }

  for (const auto &[key, value] : exact) {
    assert(tinySketch.query(key) >= value);
  }
}



int main() {
  auto run_test = [](auto f, const std::string &testName) {
    std::cout << "Start " << testName << ": ";
    f();
    std::cout << "Pass\n";
  };

  run_test(testCollisionCheck, "testCollisionCheck");
  run_test(testNonUpdatedElement, "testNonUpdatedElement");
  run_test(testSingleUpdate, "testSingleUpdate");
  run_test(testMultipleUpdates, "testMultipleUpdates");
  run_test(testIndependentElements, "testIndependentElements");
  run_test(testExactReference, "testExactReference");

  std::cout << "\nAll tests passed successfully!\n";
  return 0;
}