# CMakeLists.txt --- CMake project settings
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# project name and language
project(WonDrag VERSION 1.0 LANGUAGES C)

# libWonDrag.a
add_library(WonDrag STATIC WonDrag.c)
