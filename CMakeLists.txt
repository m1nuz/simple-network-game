cmake_minimum_required(VERSION 2.8)

project(networking)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

list(APPEND CMAKE_CXX_FLAGS "-Wall -w -pedantic")
list(APPEND CMAKE_C_FLAGS "-std=c11 -Wall -w -pedantic")

set(client_headers
include/framework.h
include/client.h
include/physics.h
include/messages.h
include/circularbuffer.h
include/history.h
include/collision.h
include/main.h
include/extmath.h
include/queue.h
)

set(client_sources
src/client.c
src/windows.c
src/threads.c
src/physics.c
src/circularbuffer.c
src/history.c
src/collision.c
src/queue.c
)

set(server_headers
include/framework.h
include/server.h
include/physics.h
include/messages.h
include/main.h
include/collision.h
)

set(server_sources
src/server.c
src/threads.c
src/physics.c
src/windows.c
src/collision.c
)

include_directories("./include")
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/win64)

# client
add_executable(client ${client_headers} ${client_sources})
target_link_libraries(client -lopengl32 -lglu32 -lmingw32 -lenet -lws2_32 -lwinmm)
# server
add_executable(server ${server_headers} ${server_sources})
target_link_libraries(server -lenet -lws2_32 -lwinmm -lopengl32 -lglu32)
