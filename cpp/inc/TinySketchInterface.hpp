#pragma once

#include <cstdint>
#include <type_traits>

template <std::size_t Width> struct CounterTypeSelector {
  static_assert(Width == 8 || Width == 16 || Width == 32 || Width == 64,
                "CounterWidth must be 8, 16, 32, or 64");
  using type = void;
};

template <> struct CounterTypeSelector<8> {
  using type = uint8_t;
};
template <> struct CounterTypeSelector<16> {
  using type = uint16_t;
};
template <> struct CounterTypeSelector<32> {
  using type = uint32_t;
};
template <> struct CounterTypeSelector<64> {
  using type = uint64_t;
};

template <std::size_t Width>
using CounterType = typename CounterTypeSelector<Width>::type;

template <std::size_t N> static constexpr bool is_power_of_two() {
  return N > 0 && (N & (N - 1)) == 0;
}

template <std::size_t Rows, std::size_t Columns, std::size_t CounterWidth>
class TinySketchInterface {
  static_assert(Rows >= 1 && Rows <= 16,
                "ROWS must be between 1 and 16 (inclusive)");

  static_assert(Columns >= 1 && Columns <= 4096,
                "COLUMNS must be between 1 and 4096 (inclusive)");

  static_assert(is_power_of_two<Columns>(),
                "COLUMNS must be a power of two (1, 2, 4, 8, ..., 4096)");

public:
  using value_type = CounterType<CounterWidth>;
  static constexpr std::size_t ROWS = Rows;
  static constexpr std::size_t COLUMNS = Columns;
  static constexpr std::size_t WIDTH = CounterWidth;

  virtual ~TinySketchInterface() = default;

  virtual void update(value_type value) = 0;
  virtual value_type query(value_type value) const = 0;
};
