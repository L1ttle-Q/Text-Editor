# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set the project name
project (text-editor)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
    src/SearchAlgorithm.cpp
    src/text-editor.cpp
    src/main.cpp
    src/linkedlist.cpp
)

# Add an executable with the above sources
add_executable(text-editor ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(text-editor
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(text-editor PRIVATE
    -std=c++11
)
