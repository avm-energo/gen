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
file(GLOB_RECURSE cpp_sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE private_headers ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE public_headers ${PROJECT_SOURCE_DIR}/include/*.h)

include_directories(${PROJECT_SOURCE_DIR}/include)

if (GEN_STATIC)
    message(STATUS "GEN STATIC LIBRARY")
    add_library(${PROJECT_NAME} STATIC ${public_headers} ${private_headers} ${cpp_sources})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DHAVE_STATIC_BUILD)
else()
    message(STATUS "GEN SHARED LIBRARY")
    add_library(${PROJECT_NAME} SHARED ${public_headers} ${private_headers} ${cpp_sources})
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

# --- Install libs and binaries ---
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# --- Install include headers ---
install(FILES
  ${public_headers}
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
