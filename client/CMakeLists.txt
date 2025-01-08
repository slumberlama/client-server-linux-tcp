cmake_minimum_required(VERSION 3.10)
project(client LANGUAGES C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../erproc)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../msg)

if(EXAMPLE_MSGPACK_EMBEDDED)
    add_subdirectory(msgpack-c)
    set(msgpack-c_DIR ${CMAKE_CURRENT_BINARY_DIR}/msgpack-c)
endif()

find_package(msgpack-c REQUIRED)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/client.c ${CMAKE_CURRENT_LIST_DIR}/../erproc/erproc.c ${CMAKE_CURRENT_LIST_DIR}/../msg/send.c)
target_link_libraries(${PROJECT_NAME} msgpack-c)

if(TARGET msgpack-c-static)
    add_executable(${PROJECT_NAME}-static ${CMAKE_CURRENT_LIST_DIR}/client.c ${CMAKE_CURRENT_LIST_DIR}/../erproc/erproc.c ${CMAKE_CURRENT_LIST_DIR}/../msg/send.c)
    target_link_libraries(${PROJECT_NAME}-static msgpack-c-static)
endif()
