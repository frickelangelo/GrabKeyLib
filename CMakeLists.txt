cmake_minimum_required(VERSION 3.0.0)
project(grabkey VERSION 0.1.0)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

include(CTest)
enable_testing()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(LINUX_HEADERS
    include/epoll_kbd_poller.h
)

set(LINUX_SOURCES
    src/epoll_kbd_poller.cpp
)

set(MACOS_HEADERS
    include/kqueue_kbd_poller.h
)

set(MACOS_SOURCES
    src/kqueue_kbd_poller.cpp
)

if (WIN32)
    message("Unsupported")
elseif (APPLE)
    set(PLATFORM_SPECIFIC_HEADERS ${MACOS_HEADERS})
    set(PLATFORM_SPECIFIC_SOURCES ${MACOS_SOURCES})
elseif (UNIX)
    set(PLATFORM_SPECIFIC_HEADERS ${LINUX_HEADERS})
    set(PLATFORM_SPECIFIC_SOURCES ${LINUX_SOURCES})
endif()

set(HEADERS
    ${PLATFORM_SPECIFIC_HEADERS}
    include/events_config.h
    include/events_filter.h
    include/events_processor_impl.h
    include/events_processor.h
    include/kbd_key_parser.h
    include/kbd_keys.h
    include/kbd_poller.h
    include/kbd_reader_impl.h
    include/kbd_reader.h
    include/raw_mode.h
)

set(SOURCES
    ${PLATFORM_SPECIFIC_SOURCES}

    src/raw_mode.cpp
    
    src/kbd_reader.cpp
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
set_property(TARGET grabKey_shared PROPERTY OUTPUT_NAME GrabKeyShared)

add_library(grabKey_static STATIC $<TARGET_OBJECTS:pureObjects>)
set_property(TARGET grabKey_static PROPERTY OUTPUT_NAME GrabKeyStatic)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

install(TARGETS grabKey_shared DESTINATION "lib/")
install(TARGETS grabKey_static DESTINATION "lib/")
install(FILES ${HEADERS} DESTINATION "include/grab-key")