cmake_minimum_required(VERSION 3.14)


project(cpp_apx LANGUAGES CXX VERSION 0.0.1)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
    endif()
    add_subdirectory(../bstr ${CMAKE_CURRENT_BINARY_DIR}/bstr)
    add_subdirectory(../dtl ${CMAKE_CURRENT_BINARY_DIR}/dtl)
endif()

### Library bstr
set (CPP_APX_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/attribute_parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/data_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/data_signature.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/data_type.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/error.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/port_attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/port_instance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/signature_parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/type_attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/types.h
)

set (CPP_APX_SOURCE_LIST
${CMAKE_CURRENT_SOURCE_DIR}/src/attribute_parser.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data_element.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/data_type.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/node.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/port_attribute.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/signature_parser.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/type_attribute.cpp
)

add_library(cpp_apx ${CPP_APX_HEADER_LIST} ${CPP_APX_SOURCE_LIST})

target_include_directories(cpp_apx PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_target_properties(cpp_apx PROPERTIES CXX_STANDARD 17)
target_link_libraries(cpp_apx PUBLIC bstr)
target_link_libraries(cpp_apx PUBLIC dtl)

###

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    configure_file("../cmake/pch.h.in" pch.h)
    add_subdirectory("../../googletest" ${CMAKE_CURRENT_BINARY_DIR}/googletest)
    enable_testing()
    include(GoogleTest)
    include("../cmake/test_help.cmake")
    set (CPP_APX_TESTS
        test/test_attribute_parser.cpp
        test/test_data_element.cpp
        test/test_node.cpp
        test/test_parser.cpp
        test/test_signature_parser.cpp
    )
    package_add_test_with_libraries(apx_test "${CPP_APX_TESTS}" cpp_apx)
endif()