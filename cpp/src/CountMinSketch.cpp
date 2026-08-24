#include "CountMinSketch.hpp"

#include <algorithm>
#include <array>

static uint32_t hash_with_seed(uint32_t value, uint32_t seed) {
  value ^= seed;
  value = ((value >> 16) ^ value) * 0x45d9f3b;
  value = ((value >> 16) ^ value) * 0x45d9f3b;
  value = (value >> 16) ^ value;
  return value % 256;
}

CountMinSketch::CountMinSketch() {
  tables.fill(std::array<uint32_t, 256>{});
}

void CountMinSketch::update(uint32_t value) {
  tables[0][hash0(value)]++;
  tables[1][hash1(value)]++;
  tables[2][hash2(value)]++;
  tables[3][hash3(value)]++;
}

uint32_t CountMinSketch::query(uint32_t value) {
  uint32_t min = std::min({tables[0][hash0(value)], tables[1][hash1(value)],
                            tables[2][hash2(value)], tables[3][hash3(value)]});
  return min;
}

uint32_t CountMinSketch::hash0(uint32_t value) const {
  return hash_with_seed(value, kHashSeed0);
}

uint32_t CountMinSketch::hash1(uint32_t value) const {
  return hash_with_seed(value, kHashSeed1);
}

uint32_t CountMinSketch::hash2(uint32_t value) const {
  return hash_with_seed(value, kHashSeed2);
}

uint32_t CountMinSketch::hash3(uint32_t value) const {
  return hash_with_seed(value, kHashSeed3);
}
