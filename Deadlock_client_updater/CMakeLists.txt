cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(Deadlock_updater)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE SOURCES
    "sources/*.cpp"
    "sources/*.h"
)

set(RC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/updater_icon.rc")

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${RC_FILE}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
    $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
)

if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    endif()
    
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ws2_32
        shell32
    )
endif()