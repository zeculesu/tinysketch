#!/bin/bash
set -e

# Массив конфигураций
declare -a CONFIGS=()
# CONFIGS+=("2 64 8")
# CONFIGS+=("4 256 16")
CONFIGS+=("4 256 32")
CONFIGS+=("8 1024 16")

# for conf in "${CONFIGS[@]}"; do
#     read ROWS COLS WIDTH <<< "$conf"
#     cd chisel
#     sbt "run ${ROWS} ${COLS} ${WIDTH}"
#     cd ../rtl
#     mv TinySketch.sv TinySketch-${ROWS}x${COLS}x${WIDTH}.sv
#     cd ..
# done

for conf in "${CONFIGS[@]}"; do
    read ROWS COLS WIDTH <<< "$conf"
    echo "======== Config: ${ROWS}x${COLS}x${WIDTH} ========"

    rm -rf build

    cp rtl/TinySketch-${ROWS}x${COLS}x${WIDTH}.sv rtl/TinySketch.sv
    
    cmake -DRTL_ROWS=${ROWS} -DRTL_COLS=${COLS} -DRTL_WIDTH=${WIDTH} -B build
    cmake --build build

    ./build/test/test_hardware
    ./build/test/test_software
    echo ""
done
