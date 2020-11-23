#################################
# SkyScrapers                   #
#    - библиотека               #
#    - тесты                    #
#                               #
#   (c) Николай Егоров, 2020    #
#################################

cmake_minimum_required(VERSION 3.0)

project(SkyScrapers LANGUAGES C)

# API библиотеки
include_directories(include)
# Это нужно, что бы заголовочные файлы появились в панели "Проекты" Qt Creator
add_custom_target(nop SOURCES
    include/skyskrapers/city.h
    include/skyskrapers/methods.h
    include/skyskrapers/street.h
    include/skyskrapers/tower.h)
# Папка с исходниками библиотеки.
add_subdirectory(src)

# Модуль Criterion при установке не виден для CMake, поэтому указываем CMake,
# что в папке проекта ./cmake есть файл FindCriterion.cmake и просим проверить
# наличие Criterion.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_HOME_DIRECTORY}/cmake)
find_package(Criterion MODULE)

if(${Criterion_FOUND})
    set(BUILD_TESTS TRUE)
    include_directories(CRITERION_INCLUDE_DIRS)
else()
    set(BUILD_TESTS FALSE)
    message(STATUS "Criterion was not found, so tests are not available.")
endif()

# Тесты, если Criterion доступен.
if(${BUILD_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()
