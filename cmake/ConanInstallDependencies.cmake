# Downloading conan CMake script and using for autodetecting
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=5cdb3042632da3efff558924eecefd580a0e786863a857ca097c3d1d43df5dcd
                TLS_VERIFY ON)
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

# Getting build type, compiler, etc... (other settings)
conan_cmake_autodetect(settings)

conan_cmake_configure(
  REQUIRES xz_utils/5.2.5
  GENERATORS cmake_find_package
  OPTIONS xz_utils:shared=False)

# Installing libraries
conan_cmake_install(
  PATH_OR_REFERENCE .
  BUILD missing
  REMOTE conancenter
  SETTINGS ${settings})
