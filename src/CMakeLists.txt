cmake_minimum_required(VERSION 3.14)

project(gen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty")

include(DetermineTargetArchitecture)
target_architecture(PROJECT_ARCHITECTURE)
message(STATUS "MSVC_C_ARCHITECTURE_ID: " ${MSVC_C_ARCHITECTURE_ID})

message(STATUS "Target architecture - ${PROJECT_ARCHITECTURE}")

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  message(STATUS "Windows")
  ADD_DEFINITIONS(-DUNICODE)
  ADD_DEFINITIONS(-D_UNICODE)
  if (NOT USER_DIRECTORY)
    set(USER_DIRECTORY $ENV{USERPROFILE})
  endif()
  if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(PROJECT_TARGET_NAME "win64")
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
    set(PROJECT_TARGET_NAME "win32")
  else()
    message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
  endif()
elseif(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  message(STATUS "Linux")

  if (NOT USER_DIRECTORY)
    set(USER_DIRECTORY $ENV{HOME})
  endif()

  if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(PROJECT_TARGET_NAME "linux64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
    set(PROJECT_TARGET_NAME "linux32")
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "armv7")
    set(PROJECT_TARGET_NAME "linux-armhf")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)
  else()
    message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
  endif()

elseif(CMAKE_SYSTEM_NAME_LOWER STREQUAL "unix")
  message(STATUS "Unix")
  if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
    set(PROJECT_TARGET_NAME "unix64")
  elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
    set(PROJECT_TARGET_NAME "unix32")
  else()
    message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
  endif()
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

if (MSVC)
    add_compile_options(/permissive-)
    string(APPEND CMAKE_CXX_FLAGS " /w14456 /w14189 /w14458 /w14100 /w14996")
else()
    #TODO make the same as msvc
endif()

string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer -fstack-protector")
string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fstack-protector")

set(CMAKE_EXPORT_COMPILE_COMMANDS             ON)

message(STATUS "Cached target arch: " ${CACHED_PROJECT_TARGET_NAME})
message(STATUS "Current target arch: " ${PROJECT_TARGET_NAME})
message(STATUS "Sysroot: " ${CMAKE_SYSROOT})
if(NOT DEFINED CACHED_PROJECT_TARGET_NAME)
    set(CACHED_PROJECT_TARGET_NAME "dummy")
endif()

string(REPLACE ";" "  " _pathNamesSpaces "${CMAKE_MODULE_PATH}")
message(STATUS "Modules dir: " "${_pathNamesSpaces}")

message(STATUS "${CMAKE_BUILD_TYPE_LOWER} mode")
message(STATUS "Target name - ${PROJECT_TARGET_NAME}")

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Xml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml
  REQUIRED)
set(QT_VERSION ${Qt5Core_VERSION})
message(STATUS "QT version - ${QT_VERSION}")

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
  message(STATUS "debug mode")
  set(MODE_DIR Debug)
else()
  message(STATUS "not a debug mode")
  set(MODE_DIR Release)
endif(CMAKE_BUILD_TYPE EQUAL "DEBUG")

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(ExternalProject)

message(STATUS "CMAKE_GENERATOR: " ${CMAKE_GENERATOR})

if(${PROJECT_ARCHITECTURE}} MATCHES "x86_64")
  set(WINDOWS_TARGET_NAME "x64")
elseif(${PROJECT_ARCHITECTURE}} MATCHES "i386")
  set(WINDOWS_TARGET_NAME "Win32")
else()
  message(FATAL_ERROR "Invalid arch name: ${PROJECT_ARCHITECTURE}")
endif()

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CUSTOM_TARGET_PLATFORM_ARG -A${WINDOWS_TARGET_NAME})
else()
    set(CUSTOM_TARGET_GENERATOR "-G${CMAKE_GENERATOR}")
  # Other stuff
endif()


message(STATUS "CUSTOM_TARGET_PLATFORM_ARG: " ${CUSTOM_TARGET_PLATFORM_ARG})
message(STATUS "CUSTOM_TARGET_GENERATOR: " ${CUSTOM_TARGET_GENERATOR})

include(xz)

add_library(
  gen STATIC
  board.cpp
  board.h
  colors.h
  stdfunc.h
  s2.h
  s2helper.h
  datamanager.h
  datarecv.h
  datarecv.cpp
  datatypes.h
  error.h
  error.cpp
  errorqueue.cpp
  helper.cpp
  helper.h
  logclass.cpp
  logclass.h
  logger.h
  logger.cpp
  pch.h
  uint24.h
  singleton.h
  timefunc.h
  timefunc.cpp
  datamanager.cpp
  stdfunc.cpp
  modules.h
  registers.h
  errorqueue.h
  std_ext.h
  s2.cpp
  configv.cpp
  configv.h
  delegate_check.h
  delegate_widget.h
  ctti/type_id.hpp
  files/files.cpp
  files/files.h
  files/xz.cpp
  files/xz.h
  settings/settings.h
  settings/usbhidsettings.h
  settings/usbhidsettings.cpp
  settings/interfacesettings.h
  settings/modbusprivate.cpp
  settings/modbusprivate.h
  settings/iec104private.h
  settings/protocomprivate.h
  xmlparse/xmlparser.cpp
  xmlparse/xmlparser.h
  xmlparse/interfaceparser.h
  )

target_link_libraries(
  gen
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PUBLIC xz
  )

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
