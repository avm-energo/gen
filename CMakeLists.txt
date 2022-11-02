cmake_minimum_required(VERSION 3.15)

project(gen
    DESCRIPTION "gen"
    VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(GEN_STATIC:BOOL "Build gen as static library" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
include(DetermineTargetArchitecture)
include(SetProjectTargetName)
include(CheckConan)

find_package(QT NAMES Qt6 Qt5)
find_package(
    Qt${QT_VERSION_MAJOR}
    COMPONENTS Core Network Xml REQUIRED
)

if(NOT DEFINED PROJECT_ARCHITECTURE)
    target_architecture(PROJECT_ARCHITECTURE)
    set_project_target_name(${PROJECT_ARCHITECTURE} PROJECT_TARGET_NAME USER_DIRECTORY)
endif()

if(PROJECT_TARGET_NAME MATCHES "win")
    conan_install_dependencies(PROJECT_ARCHITECTURE ${PROJECT_TARGET_NAME} ${USER_DIRECTORY})
endif()

#------------------------------------------------------------------------------#
############################## Target definition ###############################
#------------------------------------------------------------------------------#
# Define target sources
file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

include_directories(${PROJECT_SOURCE_DIR}/include)

if (GEN_STATIC)
    message(STATUS "GEN STATIC LIBRARY")
    add_library(${PROJECT_NAME} STATIC  ${files_h} ${files_cpp})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DHAVE_STATIC_BUILD)
else()
    message(STATUS "GEN SHARED LIBRARY")
    add_library(${PROJECT_NAME} SHARED  ${files_h} ${files_cpp})
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DGEN_EXPORTS)
    target_compile_definitions(${PROJECT_NAME} INTERFACE -DGEN_IMPORTS)
endif()
target_compile_definitions(${PROJECT_NAME} PUBLIC -DCMAKE_CONFIG)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::Network
)

if(PROJECT_TARGET_NAME MATCHES "win")
    # Getting liblzma from conan xz_utils packet
    find_package(LibLZMA)
    if(LibLZMA_FOUND)
        include_directories(${LibLZMA_INCLUDE_DIRS})
        #target_include_directories(${PROJECT_NAME} INTERFACE ${LibLZMA_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE LibLZMA::LibLZMA)
    else()
        message(FATAL_ERROR "liblzma not found, please install it manually!")
    endif()
else()
    # Getting liblzma from APT: apt-get install liblzma-dev
    target_link_libraries(${PROJECT_NAME} PUBLIC lzma)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/files)
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/include)

# --- install libs and bins ---
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# --- install includes ---
install(DIRECTORY . DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "3rdParty" EXCLUDE
    PATTERN "${PROJECT_TARGET_NAME}" EXCLUDE)

# --- install liblzma libs, bins, includes ---
#if(PROJECT_TARGET_NAME MATCHES "win")
#    set(LIBLZMA_PATH ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/bin/liblzma.dll)
#    set(LIBLZMA_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/include)
#    install(FILES ${LIBLZMA_PATH} DESTINATION ${CMAKE_INSTALL_BINDIR})
#    install(DIRECTORY "${LIBLZMA_INCLUDE_PATH}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/.." FILES_MATCHING PATTERN "*.h")
#endif()

