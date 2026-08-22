#pragma once

#include "TinySketchInterface.hpp"

#include <array>
#include <cstddef>

constexpr size_t count_hashes = 4;
constexpr uint32_t seed_hash1 = 12345;
constexpr uint32_t seed_hash2 = 67890;
constexpr uint32_t seed_hash3 = 55555;
constexpr uint32_t seed_hash4 = 99999;

class CountMinSketch : TinySketchInterface {
public:
  CountMinSketch();
  void update(uint32_t value) override;
  uint32_t query(uint32_t value) override;

private:
  std::array<std::array<uint32_t, 256>, count_hashes> tables;

  uint32_t hash0(uint32_t value) const;
  uint32_t hash1(uint32_t value) const;
  uint32_t hash2(uint32_t value) const;
  uint32_t hash3(uint32_t value) const;
};