project(bfc)
cmake_minimum_required(VERSION 3.6)

# Disable in source build.
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
  message(FATAL_ERROR "In-source build is not allowed.")
endif()

# Set default build type as Debug.
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_STABLE "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3")

find_package(LLVM 8.0 REQUIRED)

add_library(bf
  CodeGen.cpp
  Instruction.cpp
  Interpreter.cpp
  ObjGen.cpp
  Parser.cpp
  )

# bfc: the compiler

add_executable(bfc
  bfc.cpp
  )

target_link_libraries(bfc
  LLVM-8
  bf
  )

# bfrt: the object runtime

add_library(bfrt SHARED
  libbrainfuck.cpp
  )

# bfi: the interpreter

add_executable(bfi
  bfc.cpp
  )

target_link_libraries(bfi
  bf
  )

target_compile_definitions(bfi
  PUBLIC DETAILED=false
  )

# bfdb: the debug interpreter with detailed output

add_executable(bfdb
  bfc.cpp
  )

target_link_libraries(bfdb
  bf
  )

target_compile_definitions(bfdb
  PUBLIC DETAILED=true
  )

# install:

install(
  TARGETS bf bfc bfi bfdb bfrt
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib64
  ARCHIVE DESTINATION lib/bf
  )
