# TinySketch

Parameterized hardware/software implementation of the **Count-Min Sketch** probabilistic data structure.

* **C++** — software reference implementation (`cpp/inc/CountMinSketch.hpp`)
* **Chisel** — hardware implementation, compiled to SystemVerilog via CIRCT (`chisel/src/main/scala/TinySketch.scala`)

The generated RTL (`rtl/TinySketch.sv`) is simulated through **Verilator** and verified against the C++ reference.

---

## Parameterized Architecture

TinySketch is parameterized by three compile-time / RTL-generation-time parameters:

| Parameter       | Description                  | Valid range                        |
| --------------- | ---------------------------- | ---------------------------------- |
| `ROWS`          | Number of hash tables        | 1 - 16                             |
| `COLUMNS`       | Entries per table            | 1 - 4096, must be power of two     |
| `COUNTER_WIDTH` | Bit-width of each counter    | 8, 16, 32, 64                      |

The same C++ reference model and Chisel hardware implementation share all three parameters.

---

## Algorithm

Count-Min Sketch estimates value frequencies in a stream using a fixed-size table and multiple hash functions, trading bounded accuracy for memory efficiency.

```
update(x): each table[hash_i(x)]++
query(x):  min(table[0][hash0(x)], table[1][hash1(x)], ...)
```

---

## Parameters

### COUNTER_WIDTH

**Values:** 8, 16, 32, 64 (exclusive — no other values accepted)

**Why:** C++ maps to `uint8_t` / `uint16_t` / `uint32_t` / `uint64_t`. Chisel uses `UInt(8.W)` / `UInt(16.W)` / etc. Intermediate values are not supported by either side without workarounds.

### COLUMNS

**Range:** 1–4096, must be a power of two.

**Why power of two:** For power-of-two COLUMNS, the modulo operation is equivalent to a bit mask: hash_value & (COLUMNS - 1). This avoids general modulo/division logic in the hardware implementation.

**Why 4096 (upper bound):**

```
max memory = 16 (ROWS) × 4096 (COLUMNS) × 64 (WIDTH) = 4 194 304 bits = 524 288 bytes ≈ 512 KB
```
The upper bounds are chosen based on the practical size of the generated register-based RTL. Larger configurations significantly increase the number of generated registers and RTL size.

### ROWS

**Range:** 1–16, no power-of-two requirement.

**Why:**

* The upper bound of 16 is sufficient for the intended design space; increasing the number of rows also significantly increases the size of the generated register-based RTL.
* `1` is the minimum meaningful case (one hash function).

### Memory footprint

```
memory_bits  = ROWS × COLUMNS × COUNTER_WIDTH
```

For the default `4 × 256 × 32` configuration: `32768 bits = 4096 bytes`.

---

## Hash function

The same 32-bit mixing function with different seeds runs in C++ and Chisel:

```
v = value XOR seed
v = ((v >> 16) XOR v) * 0x45d9f3b    // repeated twice for better mixing
v = ((v >> 16) XOR v) * 0x45d9f3b
v = (v >> 16) XOR v
```

### Seeds

16 distinct seeds are stored in `config/seeds.json`.

## Hardware interface

```text
clock, reset

io_value        : UInt<CounterWidth>
io_update_valid : Bool
io_query_valid  : Bool
io_query_result : UInt<CounterWidth>
```

`io_update_valid` increments counters; `io_query_valid` returns the minimum of the `ROWS` hashed counters. The table is implemented as registers -- one per entry.

---

## Project structure

```
chisel/src/main/scala/TinySketch.scala   -- Chisel design (parameterized)
chisel/build.sbt                         -- SBT build
rtl/TinySketch.sv                        -- Generated SystemVerilog
cpp/inc/CountMinSketch.hpp               -- Parameterized C++ reference
cpp/inc/TinySketchHardware.hpp           -- Verilator C++ wrapper
cpp/inc/TinySketchInterface.hpp          -- Base class + static_assert constraints
test/CountMinSketchTests.hpp             -- Shared invariant tests
test/TestSoftware.cpp                    -- Software tests (C++ only)
test/TestHardware.cpp                    -- Hardware + cross-verification tests
test/CMakeLists.txt                      -- Test build + compile definitions
config/seeds.json                        -- 16 hash function seeds
CMakeLists.txt                           -- Build system (RTL params via -DRTL_*)
run_all.sh                               -- Batch RTL generation + test for multiple configs
```

---

## Verification

### Test suites

**`test_software`** — invariant tests on the C++ reference implementation.

**`test_hardware`** — the same invariant tests on Verilated RTL, plus
differential testing against the C++ reference implementation.

### Collision detection

The collision test uses the **birthday paradox** rather than brute-force iteration. It stores `(hash_signature -> value)` pairs in a map, where `hash_signature = (hash_0(v), hash_1(v), ..., hash_(R-1)(v))`. When a new value produces a signature already in the map, a colliding pair is found.

Since the total number of possible hash signatures is $2^{R \times \log_2(C)}$ (where R=ROWS, C=COLUMNS), the birthday paradox implies that a collision appears after roughly $\sqrt{2^{R \times \log_2(C)}}$ random values -- far fewer than enumerating all $2^{32}$ possible inputs.

---

## Building and running

### Requirements

CMake 3.16+, C++20 compiler (gcc/clang), Verilator 5+, JDK 11+, sbt 1.10+, Chisel.

### Build single configuration

```bash
# 1. Generate RTL (defaults to 4 256 32, or specify parameters)
cd chisel && sbt "run ROW COLUMNS COUNTER_WIDTH" && cd ..

# 2. Build C++ tests (RTL params passed via -D)
cmake -DRTL_ROWS=4 -DRTL_COLS=256 -DRTL_WIDTH=32 -B build
cmake --build build

# 3. Run
ctest --test-dir build
```

### Quick default build (4x256x32)

```bash
cd chisel && sbt run && cd ..
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

### Tested configurations

To test multiple configurations sequentially, use the provided script:

```bash
bash run_all.sh