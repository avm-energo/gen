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

message(STATUS "Current target arch: " ${PROJECT_TARGET_NAME})

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

add_library(
  ${PROJECT_NAME} SHARED
  )

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

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)

message(STATUS ${PROJECT_NAME} " include dir: " ${CMAKE_INSTALL_INCLUDEDIR})
message(STATUS ${PROJECT_NAME} " data dir: " ${CMAKE_INSTALL_DATADIR})
message(STATUS ${PROJECT_NAME} " lib dir: " ${CMAKE_INSTALL_LIBDIR})

# --- install libs and bins ---

install(TARGETS ${PROJECT_NAME}
        EXPORT CustomTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(EXPORT CustomTargets DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)

# --- install includes ---

set(public_headers
    ctti/detail/algorithm.hpp
    ctti/detail/cstring.hpp
    ctti/detail/entity_name.hpp
    ctti/detail/hash.hpp
    ctti/detail/language_features.hpp
    ctti/detail/meta.hpp
    ctti/detail/name_filters.hpp
    ctti/detail/preprocessor.hpp
    ctti/detail/pretty_function.hpp
    ctti/detailed_nameof.hpp
    ctti/hash_literal.hpp
    ctti/map.hpp
    ctti/model.hpp
    ctti/name.hpp
    ctti/nameof.hpp
    ctti/serialization.hpp
    ctti/static_value.hpp
    ctti/symbol.hpp
    ctti/symbol_from_hash.hpp
    ctti/tie.hpp
    ctti/type_id.hpp
    ctti/type_tag.hpp
    files/files.h
    files/xz.h
    settings/iec104settings.h
    settings/interfacesettings.h
    settings/modbussettings.h
    settings/protocomsettings.h
    settings/settings.h
    settings/usbhidsettings.h
    xmlparse/interfaceparser.h
    xmlparse/xmlparser.h
    board.h
    colors.h
    configv.h
    datamanager.h
    datarecv.h
    datatypes.h
    delegate_check.h
    delegate_widget.h
    error.h
    errorqueue.h
    gen_export.h
    helper.h
    logclass.h
    logger.h
    modules.h
    pch.h
    registers.h
    s2.h
    s2helper.h
    singleton.h
    std_ext.h
    stdfunc.h
    timefunc.h
    uint24.h
)

# the variant with PUBLIC_HEADER property unfortunately does not preserve the folder structure
# set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")
# so instead we iterate through public headers and install them "manually"
foreach(header ${public_headers})
    file(RELATIVE_PATH header_file_path "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
    get_filename_component(header_directory_path "${header_file_path}" DIRECTORY)
    install(
        FILES ${header}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/${header_directory_path}"
    )
endforeach()
