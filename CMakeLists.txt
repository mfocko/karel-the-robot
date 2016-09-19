cmake_minimum_required(VERSION 2.8.9)

# Set project 
project("Karel the Robot C Library" C)
set(PROJECT_VERSION_MAJOR 2016)
set(PROJECT_VERSION_MINOR 1)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# set compiler flags
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Werror -pedantic")
set(GCC_COVERAGE_LINK_FLAGS    "-lm -lcurses")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

# Set build features
set(CMAKE_BUILD_TYPE Release)

#set(CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

include_directories(include)
add_library(karel STATIC src/karel.c src/internals.c)
set_property(TARGET karel PROPERTY CXX_STANDARD 11)

# install library and header files to /usr/local/{lib,include}
install(TARGETS karel DESTINATION lib)
install(FILES "include/karel.h" DESTINATION include)


# add subdirectories
add_subdirectory(doc/examples)
#add_subdirectory(tests)
