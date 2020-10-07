cmake_minimum_required(VERSION 3.14)

project(dtl LANGUAGES CXX VERSION 0.0.1)

### Library bstr
set (DTL_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/include/dtl/dtl.hpp
)

set (DTL_SOURCE_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/dtl.cpp
)

add_library(dtl ${DTL_HEADER_LIST} ${DTL_SOURCE_LIST})

target_include_directories(dtl PUBLIC
"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(dtl PROPERTIES CXX_STANDARD 20)
###

### Executable bstr_unit
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    configure_file(../cmake/pch.h.in pch.h)
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
    endif()
    add_subdirectory("../../googletest" ${CMAKE_CURRENT_BINARY_DIR}/googletest)
    enable_testing()
    include(GoogleTest)
    include("../cmake/test_help.cmake")
    package_add_test_with_libraries(test_sv test/cpp/test_dtl_sv.cpp dtl)
    package_add_test_with_libraries(test_av test/cpp/test_dtl_av.cpp dtl)
    package_add_test_with_libraries(test_hv test/cpp/test_dtl_hv.cpp dtl)
endif()
###