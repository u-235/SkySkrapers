#################################
#   Библиотека SkyScrapers      #
#   (c) Николай Егоров, 2020    #
#################################

cmake_minimum_required(VERSION 2.4)

project(SkyScrapersLib LANGUAGES C)

add_library(skyscrapers STATIC
   skyskrapers.c
   core/city.c
   core/street.c
   core/tower.c
   methods/exclude.c
   methods/obvious.c
   methods/first_of_two.c
   methods/staircase.c
   methods/step_down.c
   methods/bruteforce.c)

# Я не стал делать флаги компиляции в корневом CMakeLists, что бы снизить
# зависимости. Может быть и зря.
if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(skyscrapers PRIVATE -g -O3 -fPIC)
    target_compile_options(skyscrapers PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Wenum-compare
        -Wfloat-equal
        -Wredundant-decls
        -Wsign-conversion
    )
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
    # TODO настроить флаги компиляции.
    target_compile_options(skyscrapers PRIVATE /W4 /WX)
else()
    message(WARNING "Unknown compiler with id=\"${CMAKE_C_COMPILER_ID}\".")
endif ()
