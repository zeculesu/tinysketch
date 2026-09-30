#pragma once

#include <algorithm>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

enum class WorkloadType { Uniform, HeavyHitters, Skewed };

template <typename value_type> class DataGenerator {
public:
  static constexpr std::size_t kDefaultSize = 10000;

  explicit DataGenerator(std::uint32_t seed = 12345) : gen_(seed) {}

  std::vector<value_type> generate(WorkloadType type,
                                   std::size_t n = kDefaultSize) {
    switch (type) {
    case WorkloadType::Uniform:
      return uniform(n);
    case WorkloadType::HeavyHitters:
      return heavyHitters(n);
    case WorkloadType::Skewed:
      return skewed(n);
    }
    throw std::runtime_error("Unknown workload type");
  }

private:
  std::mt19937 gen_;

  std::vector<value_type> uniform(std::size_t n) {
    std::uniform_int_distribution<value_type> dist;
    std::vector<value_type> data;
    data.reserve(n);
    for (std::size_t i = 0; i < n; ++i)
      data.push_back(dist(gen_));
    return data;
  }

  std::vector<value_type> heavyHitters(std::size_t n) {
    // 90% трафика — 3 "тяжёлых" ключа, 10% — редкие
    const std::vector<value_type> heavy_values = {42, 7, 99};
    const std::vector<double> heavy_weights = {0.5, 0.3, 0.1};

    std::discrete_distribution<std::size_t> heavy_pick(heavy_weights.begin(),
                                                       heavy_weights.end());

    std::uniform_real_distribution<double> prob(0.0, 1.0);
    std::uniform_int_distribution<value_type> rare_dist(0, 10000);

    std::vector<value_type> data;
    data.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
      value_type v;
      if (prob(gen_) < 0.9) {
        v = heavy_values[heavy_pick(gen_)];
      } else {
        do {
          v = rare_dist(gen_);
        } while (std::find(heavy_values.begin(), heavy_values.end(), v) !=
                 heavy_values.end());
      }
      data.push_back(v);
    }
    return data;
  }

  std::vector<value_type> skewed(std::size_t n) {
    // 100 различных значений с весами 1/(i+1)
    constexpr std::size_t N = 100;
    std::vector<double> weights(N);
    for (std::size_t i = 0; i < N; ++i)
      weights[i] = 1.0 / static_cast<double>(i + 1);

    std::discrete_distribution<std::size_t> dist(weights.begin(),
                                                 weights.end());

    std::vector<value_type> data;
    data.reserve(n);
    for (std::size_t i = 0; i < n; ++i)
      data.push_back(static_cast<value_type>(dist(gen_)));
    return data;
  }
};