cmake_minimum_required(VERSION 3.14)

project(cpp_apx_lib LANGUAGES CXX VERSION 0.0.2)

option(QT_API "Build against QT APIs?" OFF)

### Library cpp_apx_common
set (CPP_APX_COMMON_LIB_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/attribute_parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/compiler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/data_element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/data_signature.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/data_type.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/decoder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/error.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/file_client.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/file_info.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/file_map.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/node_data.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/node_instance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/node_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/pack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/port_attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/port_instance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/program.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/sha256.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/signature_parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/type_attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/vm.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/vmdefs.h
)

set (CPP_APX_COMMON_LIB_SOURCE_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/attribute_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/computation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data_element.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data_signature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data_type.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/decoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/node_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/node_instance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/node_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/numheader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/port_instance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/port.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/program.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sha256.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/signature_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm.cpp
)

add_library(cpp_apx_common ${CPP_APX_COMMON_LIB_HEADER_LIST} ${CPP_APX_COMMON_LIB_SOURCE_LIST})

target_include_directories(cpp_apx_common PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set (CPP_APX_COMMON_LINK_LIBS bstr dtl)
if (QT_API)
    target_compile_definitions(cpp_apx_common PRIVATE QT_API QT_API_VER=5)
    list(APPEND CPP_APX_COMMON_LINK_LIBS Qt5::Core)
endif()
target_link_libraries(cpp_apx_common PUBLIC "${CPP_APX_COMMON_LINK_LIBS}")

### Library cpp_apx_dtl

set (CPP_APX_DTL_LIB_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/deserializer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/serializer.h
)

set (CPP_APX_DTL_LIB_SOURCE_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/serializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/deserializer.cpp
)

add_library(cpp_apx_dtl ${CPP_APX_DTL_LIB_HEADER_LIST} ${CPP_APX_DTL_LIB_SOURCE_LIST})

target_include_directories(cpp_apx_dtl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(cpp_apx_dtl PUBLIC dtl)

if (QT_API)
    set (CPP_APX_QT_LIB_HEADER_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp-apx/qt_serializer.h
    )

    set (CPP_APX_QT_LIB_SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/src/qt_serializer.cpp
    )

    add_library(cpp_apx_qt ${CPP_APX_QT_LIB_HEADER_LIST} ${CPP_APX_QT_LIB_SOURCE_LIST})

    target_include_directories(cpp_apx_qt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_link_libraries(cpp_apx_qt PUBLIC Qt5::Core)
endif()

