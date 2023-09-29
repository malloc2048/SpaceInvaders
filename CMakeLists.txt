cmake_minimum_required(VERSION 3.22)
project(space_invaders)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_LIST_DIR}/source)

file(GLOB invaders-game-src ${CMAKE_CURRENT_LIST_DIR}/source/game/*)
file(GLOB invaders-cabinet-src ${CMAKE_CURRENT_LIST_DIR}/source/cabinet/*)
file(GLOB_RECURSE invaders-cpu-src ${CMAKE_CURRENT_LIST_DIR}/source/intel8080/*)
file(GLOB invaders-disassembler-src ${CMAKE_CURRENT_LIST_DIR}/source/disassembler/*)

file(GLOB invaders_tests ${CMAKE_CURRENT_LIST_DIR}/source/test/*)
add_executable(invaders_test ${invaders_tests} ${invaders-game-src} ${invaders-cpu-src} ${invaders-cabinet-src})
target_link_libraries(invaders_test SDL2 gtest_main gtest)

file(GLOB invaders-src ${CMAKE_CURRENT_LIST_DIR}/source/*)
add_executable(invaders
    ${invaders-src}
    ${invaders-cpu-src}
    ${invaders-game-src}
    ${invaders-cabinet-src}
    ${invaders-disassembler-src}
)
target_link_libraries(invaders SDL2)
