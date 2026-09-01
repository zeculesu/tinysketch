#pragma once

#include "TinySketchInterface.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <nlohmann/json.hpp>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;
static fs::path find_config_path() {
  std::vector<std::string> potential_paths = {
      "config/seeds.json", "../config/seeds.json", "../../config/seeds.json"};

  for (const auto &p : potential_paths) {
    if (fs::exists(p)) {
      return fs::path(p);
    }
  }
  std::string err_msg =
      "Critical Error: 'config/seeds.json' not found!\nChecked paths:\n";
  for (const auto &p : potential_paths) {
    err_msg += "  - " + fs::absolute(p).string() + "\n";
  }

  throw std::runtime_error(err_msg);
}

template <std::size_t Rows> static std::array<uint32_t, Rows> load_seeds() {
  std::array<uint32_t, Rows> seeds{};
  fs::path config_path = find_config_path();
  std::ifstream f(config_path);
  std::string line;
  std::string content;
  while (std::getline(f, line)) {
    content += line;
  }
  auto start = content.find('[');
  auto end = content.find(']', start);
  if (start != std::string::npos && end != std::string::npos) {
    std::string arr = content.substr(start + 1, end - start - 1);
    std::istringstream iss(arr);
    std::string token;
    std::size_t idx = 0;
    while (std::getline(iss, token, ',')) {
      token.erase(std::remove_if(token.begin(), token.end(), ::isspace),
                  token.end());
      if (token.empty())
        continue;
      if (idx < Rows) {
        seeds[idx] = std::stoul(token);
      }
      ++idx;
    }
  }
  return seeds;
}

template <std::size_t Rows, std::size_t Columns, std::size_t CounterWidth>
class CountMinSketch : public TinySketchInterface<Rows, Columns, CounterWidth> {
public:
  using value_type =
      typename TinySketchInterface<Rows, Columns, CounterWidth>::value_type;

  CountMinSketch() {
    for (auto &row : tables) {
      row.fill(0);
    }
    seeds = load_seeds<Rows>();
  }

  void update(value_type value) override {
    for (std::size_t i = 0; i < Rows; ++i) {
      ++tables[i][hash_with_seed(value, seeds[i])];
    }
  }

  value_type query(value_type value) const override {
    value_type min = std::numeric_limits<value_type>::max();
    for (std::size_t i = 0; i < Rows; ++i) {
      value_type x = tables[i][hash_with_seed(value, seeds[i])];
      if (x < min) {
        min = x;
      }
    }
    return min;
  }

std::pair<value_type, value_type> findCollidingPair() {
    std::random_device rd;
    std::mt19937_64 gen(12345);
    std::uniform_int_distribution<value_type> dist(
        0, std::numeric_limits<value_type>::max()
    );
    
    std::unordered_map<std::string, value_type> hash_to_value;
    
    for (int attempt = 0; attempt < 1000000; ++attempt) {
        value_type val = dist(gen);
        
        std::string key;
        for (std::size_t i = 0; i < Rows; ++i) {
            key += std::to_string(hash_with_seed(val, seeds[i])) + ",";
        }
        
        auto it = hash_to_value.find(key);
        if (it != hash_to_value.end()) {
            value_type other = it->second;
            if (val != other) {
                return {std::min(val, other), std::max(val, other)};
            }
        } else {
            hash_to_value[key] = val;
        }
    }
    
    return {value_type(0), value_type(1)};
}

private:
  static value_type hash_with_seed(value_type value, uint32_t seed) {
    value ^= seed;
    value = ((value >> 16) ^ value) * 0x45d9f3b;
    value = ((value >> 16) ^ value) * 0x45d9f3b;
    value = (value >> 16) ^ value;
    return value % Columns;
  }

  bool _allRowsMatch(value_type a, value_type b) const {
    for (std::size_t i = 0; i < Rows; ++i) {
      if (hash_with_seed(a, seeds[i]) != hash_with_seed(b, seeds[i]))
        return false;
    }
    return true;
  }

  std::array<std::array<value_type, Columns>, Rows> tables{};
  std::array<uint32_t, Rows> seeds{};
};