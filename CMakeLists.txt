cmake_minimum_required(VERSION 3.9.4)

project(modulo_lsystem LANGUAGES CXX)

include(cmake/project.cmake)

set(libname "${PROJECT_NAME}static")
set(lib_src modulo_int_system.cpp)

add_library(${libname} STATIC ${lib_src})
lsystem_configure_target(${libname})

set(exe_src main.cpp)

add_executable(${PROJECT_NAME} ${exe_src})
lsystem_configure_target(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE ${libname})

## Tests

set(testsname "${PROJECT_NAME}_tests")

set(tests_src
  tests/tests.cpp
  tests/test_modulo_materialiser.cpp)

add_executable(${testsname} ${tests_src})

set_property(TARGET ${libname} PROPERTY CXX_STANDARD 11)

target_link_libraries(${testsname} PUBLIC ${libname})

set(runtestsname test)

add_custom_target(${runtestsname}
  COMMAND $<TARGET_FILE:${testsname}>)
add_dependencies(${runtestsname} ${testsname})
