# This file is part of the "recommendify" project
#   Copyright (c) 2014 Paul Asmuth <paul@paulasmuth.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(recommendify)
enable_testing()
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
include_directories(src/)

# librecommendify
file(GLOB LIBRECOMMENDIFY_SOURCES src/recommendify/*.cc src/recommendify/*.h)
file(GLOB LIBRECOMMENDIFY_TEST_SOURCES src/recommendify/*_test.cc)
list(REMOVE_ITEM LIBRECOMMENDIFY_SOURCES ${LIBRECOMMENDIFY_TEST_SOURCES})
add_library(recommendify SHARED ${LIBRECOMMENDIFY_SOURCES})

# librecommendify tests
foreach(f ${LIBRECOMMENDIFY_TEST_SOURCES})
    string(REGEX REPLACE ".*recommendify/(.*)(_test\\.cc)$" "\\1" TEST_NAME ${f})
    string(REGEX REPLACE "[^a-z]" "" TEST_NAME ${TEST_NAME})
    set(TEST_TARGET recommendify_test_${TEST_NAME})
    add_executable(${TEST_TARGET} ${LIBRECOMMENDIFY_TEST_SOURCES})
    target_link_libraries(${TEST_TARGET} recommendify)
    add_test(${TEST_NAME} ${TEST_TARGET})
endforeach(f)

# example/movielens_minhash
add_executable(example_movielens_minhash example/movielens_minhash/movielens_minhash.cc)
