cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(tiny2-containers VERSION 0.1.0 LANGUAGES C)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX /usr/ CACHE PATH "Default install prefix" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
endif()

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_search_module(T2Object REQUIRED tiny2-object)

add_library(tiny2-containers SHARED tiny2-containers.c)
target_link_libraries(tiny2-containers ${T2Object_LIBRARIES})
target_include_directories(tiny2-containers PRIVATE ${T2Object_INCLUDE_DIRS})

add_executable(tc-test test.c)
target_link_libraries(tc-test ${T2Object_LIBRARIES} tiny2-containers)
target_include_directories(tc-test PRIVATE ${T2Object_INCLUDE_DIRS})


configure_file(${CMAKE_SOURCE_DIR}/tiny2-containers.pc.in ${CMAKE_BINARY_DIR}/tiny2-containers.pc)
install(FILES ${CMAKE_BINARY_DIR}/tiny2-containers.pc DESTINATION lib/pkgconfig)

install(
  TARGETS tiny2-containers
  LIBRARY DESTINATION lib
)

install(FILES tiny2-containers.h DESTINATION include)