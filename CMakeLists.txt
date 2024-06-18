cmake_minimum_required(VERSION 3.20)

project(qq)

add_library(ElectronJs SHARED
    ElectronJs.cpp
    HookStuff.cpp
    decoder/fetchdecode64.cc 
    decoder/fetchdecode32.cc
    )

set_target_properties(ElectronJs PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

#set_target_properties(ElectronJs PROPERTIES OUTPUT_NAME ElectronJs$<${CMAKE_CL_64}:64>)

target_compile_definitions(ElectronJs PRIVATE 
    $<$<CONFIG:Debug>:_DEBUG> 
    UNICODE
    _UNICODE
    _USRDLL
    BX_STANDALONE_DECODER
    BX_INSTR_STORE_OPCODE_BYTES
    _CRT_SECURE_NO_WARNINGS
    )

target_include_directories(ElectronJs 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )

set(NODE_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(test SHARED
    test.cpp
    HookStuff.cpp
    decoder/fetchdecode64.cc 
    decoder/fetchdecode32.cc
    )

target_compile_definitions(test PRIVATE 
    $<$<CONFIG:Debug>:_DEBUG> 
    UNICODE
    _UNICODE
    _USRDLL
    BX_STANDALONE_DECODER
    BX_INSTR_STORE_OPCODE_BYTES
    NODE_GYP_MODULE_NAME=test
    )

set_target_properties(test PROPERTIES
    OUTPUT_NAME "test.node"
    PREFIX ""
    SUFFIX ""
    )

target_compile_features(test PRIVATE cxx_std_14)

target_include_directories(test 
    PRIVATE ${NODE_SDK_PATH}/include/node)

# if (CMAKE_HOST_APPLE)
#     target_link_options(test PRIVATE -undefined dynamic_lookup)
# endif()

target_link_directories(test PRIVATE lib)

set(Machine $<IF:${CMAKE_CL_64},x64,x86>)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_custom_command(TARGET test
        PRE_BUILD
        COMMAND lib /def:node.def /machine:${Machine} /out:lib/node.lib
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(test PRIVATE node.lib)
else()
    add_custom_command(TARGET test
        PRE_BUILD
        COMMAND lib /def:qq.def /machine:${Machine} /out:lib/qq.lib
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(test PRIVATE qq.lib)
endif()
