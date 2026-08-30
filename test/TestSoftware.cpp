#include "CountMinSketch.hpp"
#include "CountMinSketchTests.hpp"

int main() {
  constexpr auto ROWS = TINY_ROWS;
  constexpr auto COLS = TINY_COLS;
  constexpr auto WIDTH = TINY_WIDTH;

  std::cout << "Config: " << ROWS << "x" << COLS << "x" << WIDTH << "\n";
  runInvariantTests<CountMinSketch<ROWS, COLS, WIDTH>>();
}