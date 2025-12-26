cmake_minimum_required(VERSION 3.24)

project(AnimationEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(AnimationEngine
  src/main.cpp
)

target_compile_features(AnimationEngine PRIVATE cxx_std_20)

if (MSVC)
  target_compile_options(AnimationEngine PRIVATE /W4 /permissive-)
else()
  target_compile_options(AnimationEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()