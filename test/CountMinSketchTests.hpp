#pragma once

#include "TinySketchInterface.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <unordered_map>

template <typename Sketch> void testCollisionCheck() {
  CountMinSketch<Sketch::ROWS, Sketch::COLUMNS, Sketch::WIDTH> ref;
  auto pair = ref.findCollidingPair();
  if (!pair) {
    std::cout << "SKIP: could not find a full-row "
                 "collision for config "
              << Sketch::ROWS << "x" << Sketch::COLUMNS << "x" << Sketch::WIDTH
              << " (key space " << Sketch::COLUMNS << "^" << Sketch::ROWS
              << " is too large for birthday search). Test skipped.\n";
    return;
  }
  auto a = pair->first;
  auto b = pair->second;

  Sketch tinySketch;
  tinySketch.update(a);
  assert(tinySketch.query(a) == 1);

  tinySketch.update(b);
  assert(tinySketch.query(a) == 2);
  assert(tinySketch.query(b) == 2);
}

template <typename Sketch> void testNonUpdatedElement() {
  Sketch tinySketch;
  assert(tinySketch.query(42) == 0);
}

template <typename Sketch> void testSingleUpdate() {
  Sketch tinySketch;
  tinySketch.update(42);
  assert(tinySketch.query(42) == 1);
}

template <typename Sketch> void testMultipleUpdates() {
  Sketch tinySketch;
  tinySketch.update(42);
  tinySketch.update(42);
  tinySketch.update(42);
  assert(tinySketch.query(42) == 3);
}

template <typename Sketch> void testIndependentElements() {
  Sketch tinySketch;
  tinySketch.update(42);
  tinySketch.update(42);
  tinySketch.update(42);
  tinySketch.update(17);
  assert(tinySketch.query(17) == 1);
  assert(tinySketch.query(42) == 3);
}

template <typename Sketch> void testNoUnderestimation() {
  Sketch tinySketch;
  std::unordered_map<uint32_t, uint32_t> exact;
  const size_t NUM_OPERATIONS = 1000;

  std::mt19937_64 gen(12345);
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


template <typename Sketch> void testBoundaryValues() {
  using value_type = typename Sketch::value_type;
  constexpr auto MAX = std::numeric_limits<value_type>::max();
  
  Sketch tinySketch;
  tinySketch.update(0);
  assert(tinySketch.query(0) == 1);
  
  tinySketch.update(0);
  assert(tinySketch.query(0) == 2);
  
  tinySketch.update(1);
  assert(tinySketch.query(1) == 1);
  assert(tinySketch.query(0) == 2);
  
  value_type max_minus_1 = MAX - 1;
  tinySketch.update(max_minus_1);
  assert(tinySketch.query(max_minus_1) == 1);
  assert(tinySketch.query(1) == 1);
  assert(tinySketch.query(0) == 2);
  
  tinySketch.update(MAX);
  assert(tinySketch.query(MAX) == 1);
  assert(tinySketch.query(max_minus_1) == 1);
  assert(tinySketch.query(1) == 1);
  assert(tinySketch.query(0) == 2);
}

inline void runTest(auto f, const std::string &testName) {
  std::cout << "Start " << testName << ": ";
  f();
  std::cout << "Pass\n";
}

template <typename Sketch> void runInvariantTests() {
  runTest(testCollisionCheck<Sketch>, "testCollisionCheck");
  runTest(testNonUpdatedElement<Sketch>, "testNonUpdatedElement");
  runTest(testSingleUpdate<Sketch>, "testSingleUpdate");
  runTest(testMultipleUpdates<Sketch>, "testMultipleUpdates");
  runTest(testIndependentElements<Sketch>, "testIndependentElements");
  runTest(testNoUnderestimation<Sketch>, "testNoUnderestimation");
  runTest(testBoundaryValues<Sketch>, "testBoundaryValues Software");
}