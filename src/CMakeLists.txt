cmake_minimum_required(VERSION 3.14)

project(gen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty")

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Xml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml
  REQUIRED)

include(ExternalProject)
include(xz)

add_library(
  gen STATIC
  settings.h
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
  xmlparser.cpp
  xmlparser.h
  module_kxx.h
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
  )

target_link_libraries(
  gen
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Xml
  PRIVATE Qt${QT_VERSION_MAJOR}::Network)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
