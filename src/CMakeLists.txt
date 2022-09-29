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

target_architecture(PROJECT_ARCHITECTURE)
set_project_target_name(${PROJECT_ARCHITECTURE} PROJECT_TARGET_NAME USER_DIRECTORY)

message(STATUS "MSVC_C_ARCHITECTURE_ID: " ${MSVC_C_ARCHITECTURE_ID})
message(STATUS "Target architecture: ${PROJECT_ARCHITECTURE}")
message(STATUS "Target name: ${PROJECT_TARGET_NAME}")

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

if(${PROJECT_ARCHITECTURE} MATCHES "x86_64")
  set(WINDOWS_TARGET_NAME "x64")
elseif(${PROJECT_ARCHITECTURE} MATCHES "i386")
  set(WINDOWS_TARGET_NAME "Win32")
else()
  message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
endif()

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CUSTOM_TARGET_PLATFORM_ARG "-A${WINDOWS_TARGET_NAME}")
else()
    set(CUSTOM_TARGET_GENERATOR "-G${CMAKE_GENERATOR}")
  # Other stuff
endif()

message(STATUS "CUSTOM_TARGET_PLATFORM_ARG: " ${CUSTOM_TARGET_PLATFORM_ARG})
message(STATUS "CUSTOM_TARGET_GENERATOR: " ${CUSTOM_TARGET_GENERATOR})

#include("${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/${CMAKE_INSTALL_LIBDIR}/cmake/liblzma/liblzma-targets.cmake")
include(xz)

#------------------------------------------------------------------------------
# Target
#------------------------------------------------------------------------------
# Define target sources
file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

set(GLOBAL_HEADERS
    files/files.h
    files/xz.h
    colors.h
    comaexception.h
    datamanager/datamanager.h
    datamanager/typesproxy.h
    error.h
    errorqueue.h
    helper.h
    logclass.h
    logger.h
    pch.h
    registers.h
    settings.h
    singleton.h
    std_ext.h
    stdfunc.h
    timefunc.h
    uint24.h
)

add_library(
  ${PROJECT_NAME} SHARED
  )
target_compile_definitions( ${PROJECT_NAME} PUBLIC -DGEN_EXPORT_LIBRARY)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DCMAKE_CONFIG)

target_sources(${PROJECT_NAME} PRIVATE
  ${files_cpp} ${files_h}
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PUBLIC xz
  )

target_include_directories( ${PROJECT_NAME} PRIVATE . )

# --- install libs and bins ---

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# --- install includes ---

install(FILES
  ${GLOBAL_HEADERS}
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
