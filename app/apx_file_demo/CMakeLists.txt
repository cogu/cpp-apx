cmake_minimum_required(VERSION 3.14)

project(apx_file_demo LANGUAGES CXX)

add_executable(apx_file_demo src/apx_file_demo.cpp)

target_link_libraries(apx_file_demo PRIVATE cpp_apx_common cpp_apx_dtl)
target_include_directories(apx_file_demo PRIVATE ${PROJECT_BINARY_DIR})
