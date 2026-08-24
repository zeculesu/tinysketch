# TinySketch

Hardware/software implementation of the **Count-Min Sketch** probabilistic data structure.

* **C++** — software reference implementation (`cpp/src/CountMinSketch.cpp`)
* **Chisel** — hardware implementation, compiled to SystemVerilog via CIRCT (`chisel/src/main/scala/TinySketch.scala`)

The generated RTL (`rtl/TinySketch.sv`) is simulated through **Verilator** and verified against the C++ reference.

---

## Algorithm

Count-Min Sketch estimates value frequencies in a stream using a fixed-size table and multiple hash functions, trading bounded accuracy for memory efficiency.

```
update(x): each table[hash_i(x)]++
query(x):  min(table[0][hash0(x)], table[1][hash1(x)], ...)
```

---

## Parameters

| Parameter       | Value    |
| --------------- | -------- |
| Rows / Hashes   | 4        |
| Columns per row | 256      |
| Counter width   | 32 bits  |
| Total counters  | 1024     |
| Storage         | 4096 B   |
| I/O             | `uint32` |

---

## Architecture

```
Chisel  ──CIRCT──►  SystemVerilog  ──Verilator──►  C++ hardware model
 C++ reference                                                │
      └──────────────────────── comparison tests ◄────────────┘
```

Both implementations share the same interface:

```cpp
void update(uint32_t value);
uint32_t query(uint32_t value);
```

---

## Hash function

Four instances of the same mixing function with different seeds produce 8-bit table indices:

```
v = value XOR seed
v = ((v >> 16) XOR v) × 0x45d9f3b
v = (v >> 16) XOR v
```

Seeds: 12345, 67890, 55555, 99999. Identical in C++ and Chisel.

---

## Hardware interface

```text
clock, reset

io_value        : UInt(32)
io_update_valid : Bool
io_query_valid  : Bool
io_query_result : UInt(32)
```

`io_update_valid` increments counters; `io_query_valid` returns the minimum of the four hashed counters. The table is implemented as registers.

---

## Project structure

```
chisel/src/main/scala/TinySketch.scala   — Chisel design
chisel/build.sbt                         — SBT build
rtl/TinySketch.sv                        — Generated SystemVerilog
cpp/inc/CountMinSketch.hpp               — Software reference
cpp/src/CountMinSketch.cpp
cpp/inc/TinySketchHardware.hpp           — Verilator wrapper
cpp/src/TinySketchHardware.cpp
test/CountMinSketchTests.hpp             — Shared invariant tests
test/TestSoftware.cpp                    — Software tests
test/TestHardware.cpp                    — Hardware + cross-verification tests
CMakeLists.txt                           — Build system
```

---

## Verification

**`test_software`** — invariant tests on C++ reference:

* update / query on new and updated values
* collisions, multiple updates per value
* `estimated_count >= exact_count` (verified via `std::unordered_map`)

**`test_hardware`** — same invariants on Verilated RTL plus cross-verification:

* 10,000 randomized operations compared against C++ reference
* reset clears the sketch

---

## Building

### Requirements

CMake 3.16+, C++20 compiler, Verilator, JDK, sbt, Chisel 7.6.0, Scala 2.13.18.

### Steps

```bash
cd chisel && sbt run && cd ..      # generate rtl/TinySketch.sv
cmake -S . -B build                # configure
cmake --build build                # build
ctest --test-dir build             # run tests
```
