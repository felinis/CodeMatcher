cmake_minimum_required(VERSION 3.12)
project(CodeMatcher)

# Find required packages
find_package(Boost REQUIRED COMPONENTS filesystem iostreams)

# Set source files
file(GLOB SOURCES "src/*.cpp" "src/demangler/*.c")

# Add executable
add_executable(CodeMatcher ${SOURCES})

# Include Boost headers
target_include_directories(CodeMatcher PRIVATE ${Boost_INCLUDE_DIRS})

# Link libraries
target_link_libraries(CodeMatcher ${Boost_LIBRARIES})
