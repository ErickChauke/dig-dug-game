# DO NOT MODIFY THIS FILE! - THIS WILL BE USED TO BUILD YOUR PROJECT

# ------------------------------------------------------------------------------
# LICENCE (MIT):
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
# ------------------------------------------------------------------------------
# Parts of this CMakeLists.txt file come from these sources:
# https://github.com/itsYakub/RaylibGameTemplate and
# https://github.com/robloach/raylib-cpp
# ------------------------------------------------------------------------------
message("Processing CMakeLists.txt")
cmake_minimum_required(VERSION 4.0.3) # the currently stable version of CMake is required
project(elen3009-project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    # Check for minimum MinGW version
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15.1.0")
            message(FATAL_ERROR "MinGW version 15.1.0 or higher is required on Windows.")
        endif()
    endif()
endif()

# Put all generated executables and docs in a top-level "release" folder
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/bin")

# ====================== Download and Build Dependencies ======================
include(FetchContent)

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_SHALLOW TRUE # only clone latest commit, restricted to branch names and tags
    )
    FetchContent_MakeAvailable(raylib)
endif()

# raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG v5.5.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

# doctest - modified doctest.h compatible with raylib
# see issue: https://github.com/raysan5/raylib/issues/1217
find_package(doctest QUIET)
if (NOT doctest_FOUND)
    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/stephenlevitt/doctest-with-raylib.git
        GIT_TAG v1.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(doctest)
endif()

# =========================== Select Source Files for Compilation ============================

set(MAIN_CPP "main.cpp") # the cpp file that runs the game and contains the entry point main() function

# CONFIGURE_DEPENDS is used to make sure that the globbing is re-run when a new source or test file is added
file(GLOB GAME_SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/game-source-code/*.cpp)
file(GLOB TESTS_SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/game-source-code/*.cpp ${CMAKE_SOURCE_DIR}/test-source-code/*.cpp) # select all cpp files in game-source-code and test-source-code directories for the test executable
list(REMOVE_ITEM TESTS_SRC "${CMAKE_SOURCE_DIR}/game-source-code/${MAIN_CPP}") # remove MAIN_CPP from the list of test source files: doctest provides its own main function

# ==================================== Setup Targets =========================================

# Game executable
set(GAME_EXE "game") # name of the game executable
add_executable(${GAME_EXE} ${GAME_SRC}) # shows the console output

# To prevent the console output from appearing when the game is run, replace the above line with this one:
# add_executable(${GAME_EXE} WIN32 ${GAME_SRC})

#  Test executable
set(TESTS_EXE "tests") # name of the test executable
set(SRC_PATH "${CMAKE_SOURCE_DIR}/game-source-code") # path to game source code folder
add_executable(${TESTS_EXE} ${TESTS_SRC})
target_include_directories(${TESTS_EXE} PRIVATE ${SRC_PATH})
target_include_directories(${TESTS_EXE} PRIVATE "${doctest_SOURCE_DIR}/doctest") # include doctest header


# ================================= Linker Settings ==========================================

if (WIN32)
    message("Producing targets for Windows")

    # Statically link libraries
    # (Source: https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html)
    target_link_options(${GAME_EXE} PRIVATE -static)
    target_link_libraries(${GAME_EXE} PRIVATE raylib_cpp raylib)
    target_link_options(${TESTS_EXE} PRIVATE -static)
    target_link_libraries(${TESTS_EXE} PRIVATE raylib_cpp raylib)
endif()

if (LINUX)
    message("Producing targets for Linux")
    message("NB: your releases have to build and run on Windows using the toolchain given in Lab 0")

    # LINUX: Telling the linker to statically link the libgcc and libstdc++ to out project.
    # (Source: https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html)
    target_link_options(${GAME_EXE} PRIVATE -static-libgcc -static-libstdc++)
    target_link_libraries(${GAME_EXE} PRIVATE raylib_cpp raylib) # CMAKE generates the linker flags
    target_link_options(${TESTS_EXE} PRIVATE -static-libgcc -static-libstdc++)
    target_link_libraries(${TESTS_EXE} PRIVATE raylib_cpp raylib) # CMAKE generates the linker flags
endif()

if (APPLE)
    message("Producing targets for macOS")
    message("NB: your releases have to build and run on Windows using the toolchain given in Lab 0")

    # Include paths for IntelliSense and compiling
    include_directories("/path/to/raylib-cpp/include")
    include_directories("/path/to/raylib/include")
    # Link the necessary macOS frameworks
    target_link_libraries(${GAME_EXE}
        "-framework IOKit"  # Access hardware devices and drivers from your apps and services.
        "-framework Cocoa"  # Native object-oriented application programming interface.
        "-framework OpenGL" # Cross-language, cross-platform application programming interface for rendering 2D and 3D vector graphics.
        raylib_cpp
        raylib
    )
    target_link_libraries(${TESTS_EXE}
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
        raylib_cpp
        raylib
    )
endif()

# ====================================== Doxygen ==========================================

# Extract Doxygen documentation from the game source code
# Documentation is placed in a folder called "doxygen" in the "upload" directory.
# Open the index.html file in a browser to view the documentation.
# NB: Doxygen is not a dependency of the project, it is assumed that it is already installed on your system
find_package(Doxygen REQUIRED doxygen)

# Doxygen options: see https://www.doxygen.nl/manual/config.html
set(DOXYGEN_GENERATE_TREEVIEW YES)
# Place doxygen output in release/doxygen (HTML will be under release/doxygen/html)
set(DOXYGEN_OUTPUT_DIRECTORY "${OUTPUT_DIR}/doxygen")

doxygen_add_docs(doxygen_docs
    "${SRC_PATH}"
    ALL
    COMMENT "Generate HTML documentation")

# ==================================== Copy resources ==================================

# Copy resources into the "release" folder
add_custom_target(
    package_for_upload ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources ${OUTPUT_DIR}/resources
)

# ======================================== CTest ============================================

include(CTest)
enable_testing()
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
# automatically add doctest tests to CTest; specify WORKING_DIRECTORY to ensure that relative paths are correct for CTest
doctest_discover_tests(${TESTS_EXE} WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


