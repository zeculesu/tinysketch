#pragma once

#include "TinySketchInterface.hpp"

#include <array>
#include <cstddef>

static constexpr size_t kHashCount = 4;
static constexpr uint32_t kHashSeed0 = 12345;
static constexpr uint32_t kHashSeed1 = 67890;
static constexpr uint32_t kHashSeed2 = 55555;
static constexpr uint32_t kHashSeed3 = 99999;

class CountMinSketch : public TinySketchInterface {
public:
  CountMinSketch();
  void update(uint32_t value) override;
  uint32_t query(uint32_t value) override;

private:
  std::array<std::array<uint32_t, 256>, kHashCount> tables;

  uint32_t hash0(uint32_t value) const;
  uint32_t hash1(uint32_t value) const;
  uint32_t hash2(uint32_t value) const;
  uint32_t hash3(uint32_t value) const;
};