add_executable(test_hardware
    test_hardware.cpp
)

target_link_libraries(test_hardware
    PRIVATE
        tinysketch-hardware
        tinysketch-lib
)

add_test(
    NAME test_hardware
    COMMAND test_hardware
)

add_executable(test_invariants
    test_invariants.cpp
)

target_link_libraries(test_invariants
    PRIVATE
        tinysketch-lib
        tinysketch-hardware
)

add_test(
    NAME test_invariants
    COMMAND test_invariants
)