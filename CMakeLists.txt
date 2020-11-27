cmake_minimum_required(VERSION 3.14)

project(cpp_apx LANGUAGES CXX VERSION 0.0.2)

option(QT_API "Build against QT APIs?" OFF)

if (MSVC)
    set(BUILD_SHARED_LIB_DEFAULT OFF)
else()
    set(BUILD_SHARED_LIB_DEFAULT ON)
endif()

if(BUILD_SHARED_LIBS)
    set(LIBRARY_TYPE SHARED)
else()
    set(LIBRARY_TYPE STATIC)
endif()
message(STATUS "LIBRARY_TYPE=${LIBRARY_TYPE}")

option(UNIT_TEST "Unit Test Build" OFF)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "QT_API=${QT_API}")
if (QT_API)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5Core REQUIRED)
endif()

# Libs
add_subdirectory(bstr ${CMAKE_CURRENT_BINARY_DIR}/bstr)
add_subdirectory(dtl ${CMAKE_CURRENT_BINARY_DIR}/dtl)
add_subdirectory(msocket ${CMAKE_CURRENT_BINARY_DIR}/msocket)
add_subdirectory(apx ${CMAKE_CURRENT_BINARY_DIR}/apx)


# Applications
add_subdirectory(app/apx_file_demo)

### Unit Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND UNIT_TEST)
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
    endif()

    configure_file("cmake/pch.h.in" pch.h)
    add_subdirectory("../googletest" ${CMAKE_CURRENT_BINARY_DIR}/googletest)
    enable_testing()
    include(GoogleTest)
    include("cmake/test_help.cmake")
    set (CPP_APX_TESTS
        apx/test/client_spy.cpp
        apx/test/client_spy.h
        apx/test/test_attribute_parser.cpp
        apx/test/test_client_connection.cpp
        apx/test/test_client.cpp
        apx/test/test_compiler.cpp
        apx/test/test_computation.cpp
        apx/test/test_data_element.cpp
        apx/test/test_data_signature.cpp
        apx/test/test_decoder.cpp
        apx/test/test_file.cpp
        apx/test/test_file_cache.cpp
        apx/test/test_file_client.cpp
        apx/test/test_file_cache.cpp
        apx/test/test_file_info.cpp
        apx/test/test_file_manager_receiver.cpp
        apx/test/test_file_manager_shared.cpp
        apx/test/test_file_map.cpp
        apx/test/test_node_data.cpp
        apx/test/test_node_manager.cpp
        apx/test/test_node.cpp
        apx/test/test_numheader.cpp
        apx/test/test_pack.cpp
        apx/test/test_parser.cpp
        apx/test/test_port_instance.cpp
        apx/test/test_program.cpp
        apx/test/test_remotefile.cpp
        apx/test/test_signature_parser.cpp
        apx/test/test_socket_client_connection.cpp
        apx/test/test_vm.cpp
    )
    if (QT_API)
        list(APPEND CPP_APX_TESTS apx/test/test_qt_serializer.cpp)
        set(APX_LIBS cpp_apx_common cpp_apx_dtl cpp_apx_qt Qt5::Core)
        package_add_test_with_libraries(apx_test "${CPP_APX_TESTS}" "${APX_LIBS}")
    else()
        list(APPEND CPP_APX_TESTS apx/test/test_serializer.cpp apx/test/test_deserializer.cpp)
        set(APX_LIBS cpp_apx_common cpp_apx_dtl)
        package_add_test_with_libraries(apx_test "${CPP_APX_TESTS}" "${APX_LIBS}")
    endif()
endif()
