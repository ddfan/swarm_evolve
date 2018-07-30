cmake_minimum_required (VERSION 3.5)

# see : https://crascit.com/2016/04/09/using-ccache-with-cmake/
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
else() 
  message("ccache not found, repeated builds will be faster if this program is installed")
endif()

project (scrimmage)

set(SCRIMMAGE_VERSION_MAJOR "0")
set(SCRIMMAGE_VERSION_MINOR "0")
set(SCRIMMAGE_VERSION_PATCH "0")
set(SCRIMMAGE_VERSION "${SCRIMMAGE_VERSION_MAJOR}.${SCRIMMAGE_VERSION_MINOR}.${SCRIMMAGE_VERSION_PATCH}")
set(SCRIMMAGE_VERSION_DATE "2012.12.30")

# give SCRIMMAGE 1 series a few more soversions
set(SCRIMMAGE_SOVERSION "00")

if(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr/local)
endif()

# fetch all the local directories for generated code
get_filename_component(SCRIMMAGE_LIB_DIR ./lib ABSOLUTE)
get_filename_component(SCRIMMAGE_PLUGIN_LIBS_DIR ./plugin_libs ABSOLUTE)
get_filename_component(SCRIMMAGE_MSG_LIBS_DIR ./msg_libs ABSOLUTE)
get_filename_component(SCRIMMAGE_BIN_DIR ./bin ABSOLUTE)
get_filename_component(SCRIMMAGE_INC_DIR ./include ABSOLUTE)
get_filename_component(SCRIMMAGE_SRC_DIR ./src ABSOLUTE)
get_filename_component(SCRIMMAGE_SHARE_DIR ./share ABSOLUTE)
get_filename_component(SCRIMMAGE_BUILD_DIR ./build ABSOLUTE)
get_filename_component(SCRIMMAGE_SCRIPTS_DIR ./scripts ABSOLUTE)
get_filename_component(SCRIMMAGE_SETUP_DIR ./setup ABSOLUTE)
get_filename_component(PROJECT_MODULES_DIR ./cmake/Modules ABSOLUTE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_MODULES_DIR})

set(SCRIMMAGE_PYTHON_DIR ${CMAKE_SOURCE_DIR}/python)

include(MacroAddExternalTarget)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose the type of build, options are: Debug Release
      RelWithDebInfo MinSizeRel." FORCE)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall") # -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall") # -fprofile-arcs -ftest-coverage")

set(EXECUTABLE_OUTPUT_PATH ${SCRIMMAGE_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SCRIMMAGE_LIB_DIR})

## set the cmake defaults for libraries and binaries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SCRIMMAGE_LIB_DIR} CACHE PATH
  "Output directory for the dynamic libraries" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SCRIMMAGE_BIN_DIR} CACHE PATH
  "Output directory for the binaries" )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SCRIMMAGE_LIB_DIR} CACHE PATH
  "Output directory for the static libraries (archives)" )

execute_process(COMMAND bash "-c" "${SCRIMMAGE_SETUP_DIR}/edit-bashrc.sh ${CMAKE_CURRENT_BINARY_DIR}" OUTPUT_VARIABLE result)

set(MSGS_INCLUDE_DIR ${CMAKE_BINARY_DIR}/msgs)
set(PROTO_INCLUDE_DIR ${CMAKE_BINARY_DIR}/src/proto)

include_directories(${SCRIMMAGE_INC_DIR})
include_directories(${MSGS_INCLUDE_DIR})
include_directories(${PROTO_INCLUDE_DIR})

option(BUILD_ROS_PLUGINS "BUILD_ROS_PLUGINS" OFF)

option(EXTERNAL "EXTERNAL" 0)
include(ExternalProject)

option(ENABLE_PYTHON_BINDINGS "ENABLE_PYTHON_BINDINGS" ON)
if (ENABLE_PYTHON_BINDINGS)
  add_definitions(-DENABLE_PYTHON_BINDINGS=1)

  ########################################################
  # Download PyBind on configure time
  #######################################################
  configure_file(${PROJECT_MODULES_DIR}/CMakeLists.txt.pybind.in
    ${CMAKE_CURRENT_BINARY_DIR}/3rd-party/pybind-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/3rd-party/pybind-download )
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/3rd-party/pybind-download )
  if (NOT PYTHON_VERSION)
    set(PYTHON_VERSION 2.7)
  endif()

  set(PYBIND11_PYTHON_VERSION ${PYTHON_VERSION})
  set(PythonLibsNew_FIND_VERSION ${PYTHON_VERSION})
  add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/3rd-party/pybind-src
    ${CMAKE_CURRENT_BINARY_DIR}/3rd-party/pybind-build)

  set(PYBIND11_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/3rd-party/pybind-src/include)
  include_directories(${PYBIND11_INCLUDE_DIRS})

  # Find Python 2.7
  #The order of the followin find_package calls is important!
  find_package(PythonLibs ${PYTHON_VERSION} REQUIRED)
  find_package(PythonInterp ${PYTHON_VERSION} REQUIRED)

  message(STATUS "Python Versions Found: ${PYTHONLIBS_VERSION_STRING}")
  message(STATUS "PYTHON_LIBRARIES : ${PYTHON_LIBRARIES}")
  message(STATUS "PYTHON_INCLUDE_DIRS: ${PYTHON_INCLUDE_DIRS}")

  set(PYTHON_FOUND TRUE)

  include_directories("${PYTHON_INCLUDE_DIRS}")

else()
  add_definitions(-DENABLE_PYTHON_BINDINGS=0)
endif()

option(BUILD_JSBSIM "Build_JSBSIM" ON)
if (BUILD_JSBSIM)
  ########################################################
  # Download and build JSBSim
  ########################################################
  set(EXTERNAL_TARGET_JSBSIM jsbsim)
  ExternalProject_Add (
    jsbsim
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rd-party
    CMAKE_COMMAND cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/3rd-party ..
    GIT_REPOSITORY "https://git.code.sf.net/p/jsbsim/code"
    GIT_TAG "36de9ac63c959cef5d7b2c56fb49c1a57fd46369"
    PATCH_COMMAND ${SCRIMMAGE_SETUP_DIR}/apply-patch.sh ${CMAKE_SOURCE_DIR}/setup/patches/jsbsim.patch
    UPDATE_COMMAND ${SCRIMMAGE_SETUP_DIR}/install-aircraft.sh "${CMAKE_CURRENT_BINARY_DIR}/3rd-party/src/jsbsim"
    INSTALL_COMMAND make install
    )

  ExternalProject_Get_Property(jsbsim install_dir)
  set(JSBSIM_INCLUDE_DIR ${install_dir}/include/JSBSim)
  include_directories(${JSBSIM_INCLUDE_DIR})

  set(JSBSIM_LIBRARIES
    ${install_dir}/lib/libJSBSim.a
    ${install_dir}/lib/libAeromatic++.a
    )
  add_definitions(-DENABLE_JSBSIM=1)
  set(JSBSIM_FOUND 1)
else()
  ########################################################
  # Find installed version of JSBSim
  ########################################################
  set(EXTERNAL_TARGET_JSBSIM "")
  find_package(JSBSIM QUIET)
  string(FIND "${JSBSIM_INCLUDE_DIRS}" "NOTFOUND" POS)
  if (JSBSIM_FOUND AND (POS EQUAL -1))      
    include_directories(${JSBSIM_INCLUDE_DIR})
    add_definitions(-DENABLE_JSBSIM=1)
  else()
    add_definitions(-DENABLE_JSBSIM=0)
    message(--------------------------)
    message("JSBSIM_NOT_FOUND, plugins that use this library will not be built")
    message(--------------------------)
  endif()
endif()

set(PROTOBUF_VERSION v3.3.0)
option(BUILD_PROTOBUF "Build_PROTOBUF" ON)
if (BUILD_PROTOBUF)
  ########################################################
  # Download and build protobuf
  ########################################################
  set(EXTERNAL_PROJECT_PROTOBUF protobuf)
  ExternalProject_Add (
    ${EXTERNAL_PROJECT_PROTOBUF}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rd-party
    GIT_REPOSITORY "https://github.com/google/protobuf.git"
    GIT_TAG ${Protobuf_VERSION}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./autogen.sh COMMAND ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/3rd-party
    PATCH_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_COMMAND make install && cd ${CMAKE_CURRENT_BINARY_DIR}/3rd-party/src/protobuf/python && ${PYTHON_EXECUTABLE} setup.py build
    )

  ExternalProject_Get_Property(protobuf install_dir)
  set(PROTOBUF_INCLUDE ${install_dir}/include)
  
  set(PROTOBUF_LIBRARIES    
    ${install_dir}/lib/libprotobuf.so
    )

  set(PROTOBUF_PROTOC_EXECUTABLE
    ${install_dir}/bin/protoc
    )
  
else()

  set(EXTERNAL_PROJECT_PROTOBUF "")
  set(PROTOC "" CACHE STRING "External protoc executable")
  set(PROTOBUF_LIBS "" CACHE STRING "External protobuf lib (libprotobuf.so)")
  set(PROTOBUF_INCLUDE "" CACHE STRING "External protobuf include directory")
  if (PROTOC STREQUAL "" OR PROTOBUF_LIBS STREQUAL "" OR PROTOBUF_INCLUDE STREQUAL "")
    ########################################################
    # Find Google Protocol Buffers
    ########################################################
    find_package(Protobuf REQUIRED)
    message(---)
    message(${PROTOBUF_LIBRARIES})
    message(---)
  else()
    set(PROTOBUF_PROTOC_EXECUTABLE "${PROTOC}")
    set(PROTOBUF_LIBRARIES "${PROTOBUF_LIBS}")
    set(PROTOBUF_INCLUDE "${PROTOBUF_INCLUDE}")    
  endif()
endif()

include_directories(${PROTOBUF_INCLUDE})

message(STATUS "Protocol Buffer Libraries: ${PROTOBUF_LIBRARIES}")
message(STATUS "Protocol Exec: ${PROTOBUF_PROTOC_EXECUTABLE}")
message(STATUS "Protocol Include: ${PROTOBUF_INCLUDE}")

set(GRPC_VERSION v1.2.1)
option(BUILD_GRPC "Build_GRPC" ON)
if (BUILD_GRPC)
  ########################################################
  # Download and build grpc
  ########################################################
  set(EXTERNAL_PROJECT_GRPC grpc)
  ExternalProject_Add (
    ${EXTERNAL_PROJECT_GRPC}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rd-party
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make HAS_SYSTEM_PROTOBUF=true PROTOC=${CMAKE_CURRENT_BINARY_DIR}/3rd-party/bin/protoc
    GIT_REPOSITORY "https://github.com/grpc/grpc"
    #GIT_TAG a03a8c4d78b8aebc530411e93d867a6e2ce76125
    GIT_TAG ${GRPC_VERSION}
    PATCH_COMMAND ${SCRIMMAGE_SETUP_DIR}/apply-patch.sh ${CMAKE_SOURCE_DIR}/setup/patches/grpc.patch
    UPDATE_COMMAND ""
    INSTALL_COMMAND make install prefix=${CMAKE_CURRENT_BINARY_DIR}/3rd-party
    )

  ExternalProject_Get_Property(grpc install_dir)
  set(GRPC_INCLUDE_DIRS ${install_dir}/include) 
  include_directories(${GRPC_INCLUDE_DIRS})

  set(GRPC_LIBRARIES
    ${install_dir}/lib/libgrpc++.so
    ${install_dir}/lib/libgrpc.so
    ${install_dir}/lib/libgpr.so
    #${install_dir}/lib/libgrpc++_reflection.so
    #${install_dir}/lib/libgrpc++_unsecure.so
    #${install_dir}/lib/libgrpc_cronet.so
    #${install_dir}/lib/libgrpc_unsecure.so
    #${install_dir}/lib/libgrpc_unsecure.so
    )

  set(GRPC_CPP_PLUGIN ${install_dir}/bin/grpc_cpp_plugin)
  set(GRPC_PYTHON_PLUGIN ${install_dir}/bin/grpc_python_plugin)

  # Make sure protobuf is built before grpc
  add_dependencies(grpc protobuf)
  set(GRPC_FOUND TRUE)

else()
  ########################################################
  # Find grpc
  ########################################################
  set(EXTERNAL_PROJECT_GRPC "")
  find_package(GRPC)

  if (NOT GRPC_FOUND)
    message("GRPC not found, disabling GRPC code segments.")
  endif()
endif()

if (GRPC_FOUND)
  add_definitions(-DENABLE_GRPC=1)
else()
  add_definitions(-DENABLE_GRPC=0)
endif()

option(BUILD_GEOGRAPHICLIB "Build_GEOGRAPHICLIB" ON)
if (BUILD_GEOGRAPHICLIB)
  ########################################################
  # Download and build geographiclib
  ########################################################
  set(EXTERNAL_PROJECT_GEOGRAPHICLIB geographiclib)
  ExternalProject_Add (
    ${EXTERNAL_PROJECT_GEOGRAPHICLIB} 
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rd-party
    CMAKE_COMMAND cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/3rd-party ..
    GIT_REPOSITORY "https://git.code.sf.net/p/geographiclib/code"
    PATCH_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_COMMAND make install
    )

  ExternalProject_Get_Property(geographiclib install_dir)
  set(GeographicLib_INCLUDE_DIRS ${install_dir}/include)  
  
  set(GeographicLib_LIBRARIES
    ${install_dir}/lib/libGeographic.so
    )
else()
  ########################################################
  # Find GeographicLib
  ########################################################
  set(EXTERNAL_PROJECT_GEOGRAPHICLIB "")
  find_package (GeographicLib 1.34 REQUIRED)  
  add_definitions (${GeographicLib_DEFINITIONS})
endif()
include_directories (${GeographicLib_INCLUDE_DIRS})


########################################################
# Find VTK
########################################################
find_package(VTK QUIET)
if (NOT EXTERNAL AND ${VTK_FOUND})
  message(STATUS "Found VTK Version: ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}.${VTK_BUILD_VERSION}")
  include(${VTK_USE_FILE})
  add_definitions(-DENABLE_VIEWER=1)
else()
  add_definitions(-DENABLE_VIEWER=0)
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
add_subdirectory(msgs)
add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(share)
if (NOT EXTERNAL)  
  add_subdirectory(tools)
endif()
add_subdirectory(${SCRIMMAGE_PYTHON_DIR})

###################################################################
# Add gtest
###################################################################
option(BUILD_TESTS "BUILD_TESTS" OFF)
option(BUILD_GTEST "Build_GTEST" ON)
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE AND BUILD_TESTS)
  enable_testing()
  
  if (BUILD_GTEST)
    ########################################################
    # Download and build googletest
    ########################################################
    set(EXTERNAL_PROJECT_GTEST googletest)
    ExternalProject_Add (
      ${EXTERNAL_PROJECT_GTEST} 
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rd-party
      CMAKE_COMMAND cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/3rd-party ..
      GIT_REPOSITORY "https://github.com/google/googletest.git"
      PATCH_COMMAND ""
      UPDATE_COMMAND ""
      INSTALL_COMMAND make install
      )

    ExternalProject_Get_Property(${EXTERNAL_PROJECT_GTEST} install_dir)
    set(GTEST_INCLUDE_DIRS ${install_dir}/include)
    set(GMOCK_INCLUDE_DIRS ${install_dir}/include)
    
    set(GTEST_LIBRARIES
      ${install_dir}/lib/libgtest.so
      )
    set(GTEST_MAIN_LIBRARIES
      ${install_dir}/lib/libgtest_main.so
      )
    set(GTEST_BOTH_LIBRARIES
      ${GTEST_LIBRARIES}
      ${GTEST_MAIN_LIBRARIES}
      )

    set(GMOCK_LIBRARIES
      ${install_dir}/lib/libgmock.so
      )
    set(GMOCK_MAIN_LIBRARIES
      ${install_dir}/lib/libgmock_main.so
      )
    set(GMOCK_BOTH_LIBRARIES
      ${GMOCK_LIBRARIES}
      ${GMOCK_MAIN_LIBRARIES}
      )
    
  else()
    ########################################################
    # Find GTest
    ########################################################
    set(EXTERNAL_PROJECT_GTEST "")
    find_package (GTest REQUIRED)
    find_package (GMock REQUIRED)
  endif()
  
  include_directories(${GTEST_INCLUDE_DIRS})
  include_directories(${GMOCK_INCLUDE_DIRS})

  add_subdirectory(test)  
endif()

set(SCRIMMAGE_CLEAN "${SCRIMMAGE_LIB_DIR};${SCRIMMAGE_BIN_DIR};${SCRIMMAGE_PLUGIN_LIBS_DIR}")
#set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
#  "${SCRIMMAGE_CLEAN}"
#  )

# Add a distclean target to the Makefile
set (DIST_CLEAN_DIRS "${SCRIMMAGE_CLEAN};${SCRIMMAGE_BUILD_DIR}")
ADD_CUSTOM_TARGET(distclean
  COMMAND ${CMAKE_COMMAND} -DDIRS_TO_REMOVE="${DIST_CLEAN_DIRS}" -P ${CMAKE_SOURCE_DIR}/cmake/Modules/distclean.cmake
  )

ADD_CUSTOM_TARGET(binaryclean
  COMMAND ${CMAKE_COMMAND} -DDIRS_TO_REMOVE="${SCRIMMAGE_LIB_DIR};${SCRIMMAGE_BIN_DIR};${SCRIMMAGE_PLUGIN_LIBS_DIR};${SCRIMMAGE_MSG_LIBS_DIR}" -P ${CMAKE_SOURCE_DIR}/cmake/Modules/distclean.cmake
  )

###############################################################################
# Installation
###############################################################################
set(PROJECT_VERSION ${SCRIMMAGE_VERSION})
set(PROJECT_DESCRIPTION "A multi-agent simulator")

IF(UNIX AND NOT APPLE)
  # - Add each library to LIBS_STRING
  # SCRIMMAGE_GTRI_SIM_LIBS is a "global" variable that is passed
  # down to each sub directory and every library that is
  # generated in this project appends its name to this
  # variable.
  foreach(I ${PROJECT_LIBS})
    set (LIBS_STRING "${LIBS_STRING} -l${I}")
    #message("=======> ${I}")
  endforeach()

  # Add all library targets to the build-tree export set
  export(TARGETS ${PROJECT_LIBS} ${PROJECT_PLUGINS}
    FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake")

  # Export the package for use from the build-tree
  # (this registers the build-tree with a global CMake-registry ~/.cmake)
  export(PACKAGE ${PROJECT_NAME})
  
  # Generate the cmake config file for the source/build tree
  set(PROJECT_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${MSGS_INCLUDE_DIR}
    ${PROTO_INCLUDE_DIR}
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

  # Install all scrimmage proto headers  
  install(
    DIRECTORY ${PROTO_INCLUDE_DIR}/${PROJECT_NAME}
    DESTINATION include
    FILES_MATCHING PATTERN "*.pb.h"
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
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SCRIMMAGE")
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Kevin DeMarco")
set(CPACK_PACKAGE_VENDOR "GTRI")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)
