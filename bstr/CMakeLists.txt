cmake_minimum_required(VERSION 3.14)

project(bstr LANGUAGES CXX VERSION 0.0.1)

### Library bstr
set (BSTR_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/include/bstr/bstr.hpp
)

set (BSTR_SOURCE_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/bstr.cpp
)

add_library(bstr ${BSTR_HEADER_LIST} ${BSTR_SOURCE_LIST})

target_include_directories(bstr PUBLIC
"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
set_target_properties(bstr PROPERTIES CXX_STANDARD 20)
###

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    configure_file(../cmake/pch.h.in pch.h)
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
    endif()
    add_subdirectory("../../googletest" ${CMAKE_CURRENT_BINARY_DIR}/googletest)
    enable_testing()
    include(GoogleTest)
    include("../cmake/test_help.cmake")
    package_add_test_with_libraries(test_bstr test/cpp/test_bstr.cpp bstr)
endif()