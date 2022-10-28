cmake_minimum_required(VERSION 3.14)

project(gen
    DESCRIPTION "gen"
    VERSION 1.0.0
    )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty")

include(GNUInstallDirs)
include(DetermineTargetArchitecture)
include(SetProjectTargetName)
include(ExternalProject)

if(NOT DEFINED PROJECT_ARCHITECTURE)
    target_architecture(PROJECT_ARCHITECTURE)
    set_project_target_name(${PROJECT_ARCHITECTURE} PROJECT_TARGET_NAME USER_DIRECTORY)
endif()

message(STATUS "Gen target architecture: ${PROJECT_ARCHITECTURE}")
message(STATUS "Gen target name: ${PROJECT_TARGET_NAME}")

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

if (MSVC)
    add_compile_options(/permissive-)
endif()

find_package(
  Qt5
  COMPONENTS Core Network Xml
  REQUIRED)

if(CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
  message(STATUS "debug mode")
  set(MODE_DIR Debug)
else()
  message(STATUS "not a debug mode")
  set(MODE_DIR Release)
endif()

message(STATUS "CMAKE_GENERATOR: " ${CMAKE_GENERATOR})

# Compile xz if we are on Windows
if(PROJECT_TARGET_NAME MATCHES "win")
    include(xz)
    message("windows")
else()
    message("not windows")
endif()

#------------------------------------------------------------------------------
# Target
#------------------------------------------------------------------------------
# Define target sources
file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} SHARED)
add_compile_definitions(${PROJECT_NAME} PRIVATE GEN_EXPORT_LIBRARY)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DCMAKE_CONFIG)
target_sources(${PROJECT_NAME} PRIVATE ${files_cpp} ${files_h})

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
)

if(PROJECT_TARGET_NAME MATCHES "win")
    target_link_libraries(${PROJECT_NAME} PUBLIC xz)
else()
    # using command: apt-get install liblzma-dev
    target_link_libraries(${PROJECT_NAME} PUBLIC lzma)
endif()

target_include_directories( ${PROJECT_NAME} PRIVATE . )

# --- install libs and bins ---
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# --- install includes ---
install(DIRECTORY . DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "3rdParty" EXCLUDE
    PATTERN "${PROJECT_TARGET_NAME}" EXCLUDE)

# --- install liblzma libs, bins, includes ---
if(PROJECT_TARGET_NAME MATCHES "win")
    set(LIBLZMA_PATH ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/bin/liblzma.dll)
    set(LIBLZMA_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/include)
    install(FILES ${LIBLZMA_PATH} DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(DIRECTORY "${LIBLZMA_INCLUDE_PATH}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/.." FILES_MATCHING PATTERN "*.h")
endif()

