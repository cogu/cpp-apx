cmake_minimum_required(VERSION 3.14)

project(apx_test_client LANGUAGES CXX)

add_executable(apx_test_client src/test_client.cpp)

target_link_libraries(apx_test_client PRIVATE cpp_apx_common cpp_apx_dtl)
target_include_directories(apx_test_client PRIVATE ${PROJECT_BINARY_DIR})
