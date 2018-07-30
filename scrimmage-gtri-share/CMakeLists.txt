cmake_minimum_required (VERSION 2.6)
project (scrimmage-gtri-share)

set(SCRIMMAGE_GTRI_VERSION_MAJOR "0")
set(SCRIMMAGE_GTRI_VERSION_MINOR "1")
set(SCRIMMAGE_GTRI_VERSION_PATCH "0")
set(SCRIMMAGE_GTRI_VERSION "${SCRIMMAGE_GTRI_VERSION_MAJOR}.${SCRIMMAGE_GTRI_VERSION_MINOR}.${SCRIMMAGE_GTRI_VERSION_PATCH}")
set(SCRIMMAGE_GTRI_VERSION_DATE "2017.12.30")

set(PROJECT_VERSION ${SCRIMMAGE_GTRI_VERSION})
set(PROJECT_DESCRIPTION "A collection of shared libs and plugins from GTRI.")

# give SCRIMMAGE_GTRI 1 series a few more soversions
set(SCRIMMAGE_GTRI_SOVERSION "00")

if(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr/local)
endif()

# fetch all the local directories for generated code
get_filename_component(SCRIMMAGE_GTRI_LIB_DIR ./lib ABSOLUTE)
get_filename_component(SCRIMMAGE_GTRI_PLUGIN_LIBS_DIR ./plugin_libs ABSOLUTE)
get_filename_component(SCRIMMAGE_GTRI_MSG_LIBS_DIR ./msg_libs ABSOLUTE)
get_filename_component(SCRIMMAGE_GTRI_BIN_DIR ./bin ABSOLUTE)
get_filename_component(SCRIMMAGE_GTRI_INC_DIR ./include ABSOLUTE)
get_filename_component(SCRIMMAGE_GTRI_SRC_DIR ./src ABSOLUTE)
get_filename_component(SCRIMMAGE_GTRI_SHARE_DIR ./share ABSOLUTE)
get_filename_component(SCRIMMAGE_GTRI_SCRIPTS_DIR ./scripts ABSOLUTE)
get_filename_component(SCRIMMAGE_GTRI_SETUP_DIR ./setup ABSOLUTE)
get_filename_component(PROJECT_MODULES_DIR ./cmake/Modules ABSOLUTE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_MODULES_DIR})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose the type of build, options are: Debug Release
      RelWithDebInfo MinSizeRel." FORCE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++14")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -std=c++14") # -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall -std=c++14") # -fprofile-arcs -ftest-coverage")

set(EXECUTABLE_OUTPUT_PATH ${SCRIMMAGE_GTRI_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SCRIMMAGE_GTRI_LIB_DIR})

## set the cmake defaults for libraries and binaries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SCRIMMAGE_GTRI_LIB_DIR} CACHE PATH
  "Output directory for the dynamic libraries" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SCRIMMAGE_GTRI_BIN_DIR} CACHE PATH
  "Output directory for the binaries" )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SCRIMMAGE_GTRI_LIB_DIR} CACHE PATH
  "Output directory for the static libraries (archives)" )

execute_process(COMMAND bash "-c" "${SCRIMMAGE_GTRI_SCRIPTS_DIR}/edit-bashrc.sh ${PROJECT_BINARY_DIR}" OUTPUT_VARIABLE result)

set(MSGS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/msgs)

include_directories(${SCRIMMAGE_GTRI_INC_DIR})
include_directories(${MSGS_INCLUDE_DIR})

###############################################################################
# Find Scrimmage
###############################################################################
find_package(scrimmage REQUIRED)
include_directories(${SCRIMMAGE_INCLUDE_DIRS})

###############################################################################
# Find Python / PyBind
###############################################################################
option(ENABLE_PYTHON_BINDINGS "ENABLE_PYTHON_BINDINGS" ON)
option(USE_SCRIMMAGE_PYTHON "USE_SCRIMMAGE_PYTHON" ON)
if (ENABLE_PYTHON_BINDINGS)
  add_definitions(-DENABLE_PYTHON_BINDINGS=1)

  if (USE_SCRIMMAGE_PYTHON)
    if(NOT SCRIMMAGE_PYTHON_FOUND)
      message(FATAL_ERROR "SCRIMMAGE_PYTHON not found.")
    endif()

    set(PYTHONLIBS_VERSION_STRING ${SCRIMMAGE_PYTHONLIBS_VERSION_STRING})
    set(PYTHON_LIBRARIES ${SCRIMMAGE_PYTHON_LIBRARIES})
    set(PYTHON_INCLUDE_DIRS ${SCRIMMAGE_PYTHON_INCLUDE_DIRS})
    set(PYBIND11_INCLUDE_DIRS ${SCRIMMAGE_PYBIND11_INCLUDE_DIRS})
    include_directories("${PYBIND11_INCLUDE_DIRS}")

  else()
    find_package(PythonLibs 2.7 REQUIRED)
    find_package(PythonInterp 2.7 REQUIRED)
  endif()

  include_directories("${PYTHON_INCLUDE_DIRS}")
  message(STATUS "Python Versions Found: ${PYTHONLIBS_VERSION_STRING}")
  message(STATUS "PYTHON_LIBRARIES : ${PYTHON_LIBRARIES}")
  message(STATUS "PYTHON_INCLUDE_DIRS: ${PYTHON_INCLUDE_DIRS}")
else()
  add_definitions(-DENABLE_PYTHON_BINDINGS=0)
endif()

###############################################################################
# Find jsbsim (optional)
###############################################################################
option(USE_SCRIMMAGE_JSBSIM "USE_SCRIMMAGE_JSBSIM" ON)
if (USE_SCRIMMAGE_JSBSIM AND SCRIMMAGE_JSBSIM_FOUND)
  set(JSBSIM_FOUND ${SCRIMMAGE_JSBSIM_FOUND})
  set(JSBSIM_INCLUDE_DIR ${SCRIMMAGE_JSBSIM_INCLUDE_DIR})
  set(JSBSIM_LIBRARIES ${SCRIMMAGE_JSBSIM_LIBRARIES})
else()
  find_package (JSBSIM QUIET)
endif()
if (JSBSIM_FOUND)
  include_directories(${JSBSIM_INCLUDE_DIR})
endif()

###############################################################################
# Find geographiclib (required)
###############################################################################
option(USE_SCRIMMAGE_GEOGRAPHICLIB "USE_SCRIMMAGE_GEOGRAPHICLIB" ON)
if (USE_SCRIMMAGE_GEOGRAPHICLIB)
  if (NOT SCRIMMAGE_GeographicLib_FOUND)
    message(FATAL_ERROR "SCRIMMAGE_GeographicLib not found.")
  endif()
  set(GeographicLib_INCLUDE_DIRS ${SCRIMMAGE_GeographicLib_INCLUDE_DIRS})
  set(GeographicLib_LIBRARIES ${SCRIMMAGE_GeographicLib_LIBRARIES})
else()
  find_package (GeographicLib 1.34 REQUIRED)
  add_definitions (${GeographicLib_DEFINITIONS})
endif()
include_directories(${GeographicLib_INCLUDE_DIRS})

###############################################################################
# Find Protobuf (required)
###############################################################################
option(USE_SCRIMMAGE_PROTOBUF "USE_SCRIMMAGE_PROTOBUF" ON)
if (USE_SCRIMMAGE_PROTOBUF)
  if (NOT SCRIMMAGE_PROTOBUF_FOUND)
    message(FATAL_ERROR "SCRIMMAGE_PROTOBUF not found.")
  endif()

  set(PROTOBUF_INCLUDE ${SCRIMMAGE_PROTOBUF_INCLUDE})
  set(PROTOBUF_LIBRARIES ${SCRIMMAGE_PROTOBUF_LIBRARIES})
  set(PROTOBUF_PROTOC_EXECUTABLE ${SCRIMMAGE_PROTOBUF_PROTOC_EXECUTABLE})
else()
  set(PROTOC "" CACHE STRING "External protoc executable")
  set(PROTOBUF_LIBS "" CACHE STRING "External protobuf lib (libprotobuf.so)")
  set(PROTOBUF_INCLUDE "" CACHE STRING "External protobuf include directory")
  if (PROTOC STREQUAL "" OR PROTOBUF_LIBS STREQUAL "" OR PROTOBUF_INCLUDE STREQUAL "")
    find_package(Protobuf REQUIRED)
  else()
    set(PROTOBUF_PROTOC_EXECUTABLE "${PROTOC}")
    set(PROTOBUF_LIBRARIES "${PROTOBUF_LIBS}")
    set(PROTOBUF_INCLUDE "${PROTOBUF_INCLUDE}")
  endif()
endif()
include_directories(${PROTOBUF_INCLUDE})

###############################################################################
# Find GRPC (optional)
###############################################################################
option(USE_SCRIMMAGE_GRPC "USE_SCRIMMAGE_GRPC" ON)
if (USE_SCRIMMAGE_GRPC AND SCRIMMAGE_GRPC_FOUND)
  set(GRPC_CPP_PLUGIN ${SCRIMMAGE_GRPC_CPP_PLUGIN})
  set(GRPC_PYTHON_PLUGIN ${SCRIMMAGE_GRPC_PYTHON_PLUGIN})
  set(GRPC_INCLUDE_DIRS ${SCRIMMAGE_GRPC_INCLUDE_DIRS})
  set(GRPC_LIBRARIES ${SCRIMMAGE_GRPC_LIBRARIES})
  set(GRPC_FOUND TRUE)
else()
  find_package (GRPC QUIET)
  if (GRPC_FOUND)
    find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
  endif()
endif()
if (GRPC_FOUND)
  include_directories(${GRPC_INCLUDE_DIRS})
endif()

########################################################
# Find Eigen
########################################################
if(NOT "${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
  # 32 bit check, see https://cmake.org/pipermail/cmake/2013-December/056587.html
  # see also: http://eigen.tuxfamily.org/dox/group__TopicUnalignedArrayAssert.html
  add_definitions(-DEIGEN_DONT_VECTORIZE=1)
  add_definitions(-DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT=1)
endif()
find_package(Eigen3 QUIET)

if (NOT Eigen3_FOUND)
  include(LibFindMacros)
  libfind_pkg_check_modules(EIGEN_PKGCONFIG eigen3)
  if (EIGEN_PKGCONFIG_INCLUDE_DIRS STREQUAL "")
    message(FATAL_ERROR "could not find eigen either through find_package or pkgconfig")
  endif() 
  set(EIGEN3_INCLUDE_DIR ${EIGEN_PKGCONFIG_INCLUDE_DIRS})
  message("Found Eigen with pkg-config, EIGEN3_INCLUDE_DIR: ${EIGEN3_INCLUDE_DIR}")
endif() 
include_directories(${EIGEN3_INCLUDE_DIR})
########################################################
# Find Boost
########################################################
#set(Boost_DEBUG ON)
find_package(Boost COMPONENTS thread date_time iostreams program_options regex
  filesystem system REQUIRED)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

########################################################
# Recurse into src, share, and plugins
########################################################
add_subdirectory(src)
add_subdirectory(msgs)
add_subdirectory(plugins)

###################################################################
# Add gtest
###################################################################
if (NOT DEFINED ADD_GTEST)
  SET(ADD_GTEST 0)
endif()

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE AND ADD_GTEST EQUAL 1)
  # test does not link properly on cross compile machines yet

  ##################################################
  # see here for details
  # http://crascit.com/2015/07/25/cmake-gtest/
  # Download and unpack googletest at configure time
  enable_testing()
  configure_file(${CMAKE_SOURCE_DIR}/cmake/Modules/CMakeLists.txt.gtest.in
    googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

  # Prevent GoogleTest from overriding our compiler/linker options
  # when building with Visual Studio
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This adds
  # the following targets: gtest, gtest_main, gmock
  # and gmock_main
  add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
    ${CMAKE_BINARY_DIR}/googletest-build)

  # The gtest/gmock targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
      "${gmock_SOURCE_DIR}/include")
  endif()
  ##################################################
  add_subdirectory(test)
endif()

###############################################################################
# Installation
###############################################################################
IF(UNIX AND NOT APPLE)
  # - Add each library to LIBS_STRING
  # SCRIMMAGE_GTRI_SIM_LIBS is a "global" variable that is passed
  # down to each sub directory and every library that is
  # generated in this project appends its name to this
  # variable.
  foreach(I ${PROJECT_LIBS})
    set (LIBS_STRING "${LIBS_STRING} -l${I}")
  endforeach()

  # Add all library targets to the build-tree export set
  export(TARGETS ${PROJECT_LIBS}
    FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake")

  # Export the package for use from the build-tree
  # (this registers the build-tree with a global CMake-registry ~/.cmake)
  export(PACKAGE ${PROJECT_NAME})

  # Generate the cmake config file for the source/build tree
  set(PROJECT_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${MSGS_INCLUDE_DIR}    
    )
  
  # Generate the config file that sets *_INCLUDE_DIRS and *_LIBRARIES
  configure_file(${CMAKE_SOURCE_DIR}/cmake/Modules/project-config.cmake.in
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake" @ONLY)

  # Generate the cmake config file for the install tree
  set(PROJECT_INCLUDE_DIRS
    ${CMAKE_INSTALL_PREFIX}/include
    )
  
  # Generate the config file that sets *_INCLUDE_DIRS and *_LIBRARIES
  configure_file(${CMAKE_SOURCE_DIR}/cmake/Modules/project-config.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}-config.cmake" @ONLY)

  # Generate the package config file for the install tree
  configure_file (
    "${PROJECT_MODULES_DIR}/project.pc.in"
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}.pc"
    @ONLY)

  # Generate the cmake version file for both build/source and install trees
  configure_file (
    "${PROJECT_MODULES_DIR}/project-config-version.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    @ONLY
    )

  # Install all plugin XML files under etc
  install(
    DIRECTORY plugins
    DESTINATION etc/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.xml"
    )

  # Install all plugin header files under include/PROJECT_NAME
  install(
    DIRECTORY plugins
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.h"
    )

  # Install all library headers
  install(
    DIRECTORY include/${PROJECT_NAME}
    DESTINATION include
    PATTERN "*.pyc" EXCLUDE
    PATTERN "*__pycache__*" EXCLUDE
    )

  # Install all project message proto headers  
  install(
    DIRECTORY ${MSGS_INCLUDE_DIR}/${PROJECT_NAME}
    DESTINATION include
    FILES_MATCHING PATTERN "*.pb.h"
    )

  # Install the export set for use with the install-tree
  # Each library needs to export to this same export target name
  install(
    EXPORT ${PROJECT_NAME}-targets
    DESTINATION lib/cmake/${PROJECT_NAME}
    )

  # Install the cmake config
  install (FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}-config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
    )

  # Install the ${PROJECT_NAME}.pc file to lib/pkgconfig
  install (FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}.pc"
    DESTINATION lib/pkgconfig
    )

ENDIF()

###############################################################################
# Binary installation generation
###############################################################################
set(VERSION "0.0.1")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SCRIMMAGE GTRI Share")
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Kevin DeMarco")
set(CPACK_PACKAGE_VENDOR "GTRI")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)
