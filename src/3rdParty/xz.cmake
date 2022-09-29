find_package(
  Qt5
  COMPONENTS Core REQUIRED
  )

set(XZ_SRC_DIR ${CMAKE_SOURCE_DIR}/../include/xz)

### MAIN PART

set(XZ_BINARY_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/bin)
set(XZ_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/include)
set(XZ_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/lib)

string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_LOWER)

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  set(CHANGING_FILE ${XZ_LIBRARY_DIR}/liblzma.lib)
else()
  set(CHANGING_FILE ${XZ_LIBRARY_DIR}/liblzma.so)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

ExternalProject_Add(
  xzBuild
  PREFIX xz
  SOURCE_DIR ${XZ_SRC_DIR}
  CONFIGURE_COMMAND
    ${CMAKE_COMMAND} ${CUSTOM_TARGET_GENERATOR} ${CUSTOM_TARGET_PLATFORM_ARG} -S ${XZ_SRC_DIR}
    -DQT5_DIR:STRING=${QT_DIR}
    -DQT_DIR:STRING=${QT_DIR}
    -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
    -DCMAKE_BUILD_TYPE:String=${CMAKE_BUILD_TYPE}
    -DBUILD_SHARED_LIBS=ON
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}
    -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
    -DCMAKE_MODULE_PATH=${CMAKE_BINARY_DIR}
  BUILD_COMMAND ${CMAKE_COMMAND} --build . -j${NCPU} --config ${CMAKE_BUILD_TYPE}
  INSTALL_COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
  BUILD_BYPRODUCTS ${CHANGING_FILE}
  USES_TERMINAL_BUILD TRUE
  USES_TERMINAL_CONFIGURE TRUE)

# Hack to make it work, otherwise INTERFACE_INCLUDE_DIRECTORIES will not be
# propagated
get_filename_component(_fullpath "${XZ_BINARY_DIR}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${XZ_INCLUDE_DIRS}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${XZ_LIBRARY_DIR}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()

add_library(xz SHARED IMPORTED GLOBAL)
set_target_properties(xz PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                            ${XZ_INCLUDE_DIRS})

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  set_target_properties(
    xz
    PROPERTIES IMPORTED_LOCATION ${XZ_BINARY_DIR}
               IMPORTED_IMPLIB ${XZ_LIBRARY_DIR}/liblzma.lib)
else()
  set_target_properties(
    xz PROPERTIES IMPORTED_LOCATION
                          ${XZ_LIBRARY_DIR}/liblzma.so)
endif()

set(XZ_LIBS xz)
set(XZ_LIBRARY_DIRS ${XZ_BINARY_DIR} ${XZ_LIBRARY_DIR})

add_dependencies(xz xzBuild)
