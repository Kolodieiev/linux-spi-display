cmake_minimum_required(VERSION 3.20)
project(LinuxTFT LANGUAGES CXX C)

file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE C_SOURCES "src/*.c")

add_executable(app ${CPP_SOURCES} ${C_SOURCES}) 

set_target_properties(app PROPERTIES 
    CXX_STANDARD 17 
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_compile_options(app PRIVATE -Wno-unknown-pragmas)
