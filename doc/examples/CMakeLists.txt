#cmake_minimum_required(VERSION 2.8.9)
#project (Examples C)

#SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Werror -pedantic -std=c11")
#SET(GCC_COVERAGE_LINK_FLAGS    "-lm -lcurses")

#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
#SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/doc/examples)
#For the shared library:
#set ( PROJECT_LINK_LIBS libkarel.a )
#link_directories(../../build)

#For the static library:
#set ( PROJECT_LINK_LIBS libtestStudent.a )
#link_directories( ~/exploringBB/extras/cmake/studentlib_static/build )

include_directories(../../include)

add_executable(stairs stairs.c)
target_link_libraries(stairs LINK_PUBLIC karel)
set_property(TARGET stairs PROPERTY CXX_STANDARD 11)
