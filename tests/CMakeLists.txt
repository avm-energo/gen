project(gen_tests)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Test REQUIRED)
include(CTest)

# Collecting sources
set(GEN_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gen_test_class.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gen_test_class.h
    ${CMAKE_CURRENT_SOURCE_DIR}/binary_file_tests.cpp
)

# Adding executable file for gen tests
add_executable(${PROJECT_NAME} ${GEN_TEST_SOURCES})
# Adding test in ctest
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

# Linking all necessary libraries
target_link_libraries(${PROJECT_NAME}
  gen
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Test)
