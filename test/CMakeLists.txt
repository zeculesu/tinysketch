add_executable(test_software TestSoftware.cpp)
target_link_libraries(test_software PRIVATE tinysketch-lib)
add_test(NAME software COMMAND test_software)

add_executable(test_hardware TestHardware.cpp)
target_link_libraries(test_hardware PRIVATE tinysketch-lib tinysketch-hardware)
add_test(NAME hardware COMMAND test_hardware)
