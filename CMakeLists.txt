cmake_minimum_required(VERSION 3.0.0)
project(grabkey VERSION 0.1.0)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

include(CTest)
enable_testing()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/shared)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCES
    src/raw_mode.cpp
    
    src/kbd_reader.cpp
    src/kbd_poller.cpp
    src/kbd_reader_impl.cpp
    src/kbd_key_parser.cpp

    src/events_processor_impl.cpp
    src/events_processor.cpp
)

add_library(pureObjects OBJECT ${SOURCES})
set_property(TARGET pureObjects PROPERTY POSITION_INDEPENDENT_CODE 1)
set_property(TARGET pureObjects PROPERTY CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Werror")
set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

target_include_directories(pureObjects PUBLIC include)
target_link_libraries(pureObjects PUBLIC -lpthread)

add_library(grabKey_shared SHARED $<TARGET_OBJECTS:pureObjects>)
set_property(TARGET grabKey_shared PROPERTY OUTPUT_NAME GrabKey)

add_library(grabKey_static STATIC $<TARGET_OBJECTS:pureObjects>)
set_property(TARGET grabKey_static PROPERTY OUTPUT_NAME GrabKey)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
