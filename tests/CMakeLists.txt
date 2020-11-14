#################################
# Тесты библиотеки SkyScrapers  #
#   (c) Николай Егоров, 2020    #
#################################

cmake_minimum_required(VERSION 2.7)

project(SkyScrapersTests LANGUAGES C)

add_executable(tests
    test_solver.c)

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(skyscrapers PRIVATE -g -O3 -fPIC)
    target_compile_options(skyscrapers PRIVATE -Wall -Wextra)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
    target_compile_options(skyscrapers PRIVATE /W4 /WX)
else()
    message(WARNING "Unknown compiler with id=\"${CMAKE_C_COMPILER_ID}\".")
endif ()

target_link_libraries(tests skyscrapers criterion)

add_test(TestSuite tests --ascii --full-stats)
