cmake_minimum_required(VERSION 3.8)
project(glider)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(Eigen3 REQUIRED)
find_package(GTSAM REQUIRED)
find_package(GTSAM_UNSTABLE REQUIRED)
find_package(GTest REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(pybind11 REQUIRED)

# Include directories
include_directories(
  include
  ${EIGEN3_INCLUDE_DIR}
  ${GTSAM_INCLUDE_DIR}
  ${yaml-cpp_INCLUDE_DIRS}
)

# Libraries  
add_library(${PROJECT_NAME} SHARED
  src/imu_buffer.cpp
  src/factor_manager.cpp
  src/params.cpp
  src/quaternion.cpp
  src/odometry.cpp
  src/state.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    gtsam
    gtsam_unstable
    yaml-cpp
    tbb
)

# TESTS
enable_testing()

# build utm test
add_executable(utm_test test/test_ll_to_utm.cpp)
target_link_libraries(utm_test
  GTest::GTest
  GTest::Main
  ${PROJECT_NAME}
)

# build params test
add_executable(params_test test/test_params.cpp)
target_link_libraries(params_test
  GTest::GTest
  GTest::Main
  ${PROJECT_NAME}
)

# build factor tests
add_executable(factor_test test/test_factors.cpp)
target_link_libraries(factor_test
  GTest::GTest
  GTest::Main
  ${PROJECT_NAME}
)

add_executable(quaternion_test test/test_quaternion.cpp)
target_link_libraries(quaternion_test
  GTest::GTest
  GTest::Main
  ${PROJECT_NAME}
)

add_executable(heading_test test/test_heading.cpp)
target_link_libraries(heading_test
  GTest::GTest
  GTest::Main
  ${PROJECT_NAME}
)

include(GoogleTest)
gtest_discover_tests(utm_test)
gtest_discover_tests(params_test)
gtest_discover_tests(factor_test)
gtest_discover_tests(quaternion_test)
gtest_discover_tests(heading_test)


# Create Python bindings for FactorManager
add_library(glider_bindings MODULE src/factor_manager_bindings.cpp)
target_link_libraries(glider_bindings PRIVATE pybind11::module ${PROJECT_NAME} gtsam)

# Ensure the shared object is copied to the Python package directory
set(PYBINDINGS_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glider_bindings")

# Make sure the directory exists
file(MAKE_DIRECTORY ${PYBINDINGS_INSTALL_DIR})

# Copy the generated .so file to the directory
add_custom_command(TARGET glider_bindings POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:glider_bindings> ${PYBINDINGS_INSTALL_DIR}/glider_bindings.so)

# Generate dynamic __init__.py that imports everything
set(INIT_PY_CONTENT 
"try:\n"
"    from .glider_bindings import *\n"
"    try:\n"
"        from .glider_bindings import __all__\n"
"    except ImportError:\n"
"        pass\n"
"except ImportError as e:\n"
"    import warnings\n"
"    warnings.warn(f\"Failed to import native module: {e}\")\n"
"    __all__ = []\n")

file(WRITE "${PYBINDINGS_INSTALL_DIR}/__init__.py" ${INIT_PY_CONTENT})

# More robust installation
install(
    DIRECTORY "${PYBINDINGS_INSTALL_DIR}/"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}"
    FILES_MATCHING PATTERN "*.py"
    PATTERN "__pycache__" EXCLUDE
)