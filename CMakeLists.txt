cmake_minimum_required(VERSION 3.8)

# =========================================
# Setting up some variables
# =========================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_PREFIX
    ${CMAKE_BINARY_DIR}
    CACHE STRING "Final install location")
set(CMAKE ${CMAKE_COMMAND})

# =========================================
# Including the cmake modules
# =========================================

include(ExternalProject)

# =========================================
# Defining variables for colored output
# =========================================

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold "${Esc}[1m")
  set(Red "${Esc}[31m")
  set(Green "${Esc}[32m")
  set(Yellow "${Esc}[33m")
  set(Blue "${Esc}[34m")
  set(Magenta "${Esc}[35m")
  set(Cyan "${Esc}[36m")
  set(White "${Esc}[37m")
  set(BoldRed "${Esc}[1;31m")
  set(BoldGreen "${Esc}[1;32m")
  set(BoldYellow "${Esc}[1;33m")
  set(BoldBlue "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan "${Esc}[1;36m")
  set(BoldWhite "${Esc}[1;37m")
endif()

# =========================================
# Setting up the project
# =========================================

project("vupdater")

# =========================================
# Setting up some compiler variables
# =========================================

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR “${BoldRed}In-source build detected!${ColourReset}”)
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "MinSizeRel")
endif()

if((DEFINED INFOEX) AND (INFOEX EQUAL 1))
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_NO_INFOEX")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_NO_INFOEX")
else()
  set(INFOEX 0)
endif()

if(DEFINED COMPILE)
  message(STATUS "${Blue}COMPILE = ${COMPILE}${ColourReset}")
else()
  message(STATUS "${BoldGreen}COMPILE not supplied${ColourReset}")
  message(STATUS "${BoldGreen}Defaulting to COMPILE = 1${ColourReset}")
  set(COMPILE 1)
endif()

# =========================================
# Including the root directories
# =========================================

include_directories("include" "${CMAKE_BINARY_DIR}/include")

# =========================================
# Compiling the dependencies (COMPILE = 0)
# =========================================

if(${COMPILE} EQUAL 0)
  # =========================================
  # Adding the curl project for building
  # =========================================

  if(WIN32)
    ExternalProject_Add(
      curl_project
      URL "https://github.com/curl/curl/releases/download/curl-8_4_0/curl-8.4.0.zip"
      CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                 -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                 -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}/vlib
                 -DBUILD_CURL_EXE=OFF
                 -DBUILD_SHARED_LIBS=ON
                 -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                 -DOPENSSL_USE_STATIC_LIBS=ON
                 -DCURL_USE_OPENSSL=ON
                 -DBUILD_TESTING=OFF
                 -DUSE_WIN32_LDAP=OFF
                 -DENABLE_THREADED_RESOLVER=OFF
                 -DUSE_LIBIDN2=OFF)
  else()
    ExternalProject_Add(
      curl_project
      URL "https://github.com/curl/curl/releases/download/curl-8_4_0/curl-8.4.0.zip"
      CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                 -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                 -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}/vlib
                 -DBUILD_CURL_EXE=OFF
                 -DBUILD_SHARED_LIBS=ON
                 -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                 -DOPENSSL_USE_STATIC_LIBS=ON
                 -DCURL_USE_OPENSSL=ON
                 -DBUILD_TESTING=OFF
                 -DUSE_WIN32_LDAP=OFF
                 -DENABLE_THREADED_RESOLVER=OFF
                 -DUSE_LIBIDN2=OFF
      BUILD_COMMAND ../curl_project/configure --with-openssl
                    --prefix=${CMAKE_BINARY_DIR}
      COMMAND ${CMAKE_COMMAND} --build .
      INSTALL_COMMAND ${CMAKE_COMMAND} --build . -- install)
  endif()

  # =========================================
  # Adding the spdlog project for building
  # =========================================

  ExternalProject_Add(
    spdlog_project
    URL "https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.zip"
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
               -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}/vlib
               -DCMAKE_POSITION_INDEPENDENT_CODE=ON
               -DSPDLOG_INSTALL=ON
               -DSPDLOG_BUILD_EXAMPLE=OFF)

  # =========================================
  # Adding the json header file in the tree
  # =========================================

  ExternalProject_Add(
    json_project
    URL "https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp"
    DOWNLOAD_NO_EXTRACT TRUE
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND "")

  # =========================================
  # Adding the indicators file in the tree
  # =========================================

  ExternalProject_Add(
    indicators_project
    URL "https://github.com/p-ranav/indicators/archive/refs/tags/v2.3.zip"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND "")

  # =========================================
  # Displaying further info about the build
  # =========================================

  unset(COMPILE CACHE)
  message(
    STATUS
      "${Green}Build the project to generate the dependencies${ColourReset}")
  message(
    STATUS "${Cyan}And then compile the project without any args${ColourReset}")
  message(STATUS "${Cyan}for building the main project${ColourReset}")

  # =========================================
  # Compiling the main project (COMPILE = 1)
  # =========================================

else()

  # =========================================
  # Including VUPDATE sources
  # =========================================

  file(GLOB VUPDATE_SOURCES src/vupdate/*.cpp src/vdownload/*.cpp
       src/vconsolid/*.cpp)

  # =========================================
  # Including VPACK sources
  # =========================================

  file(GLOB VPACK_SOURCES src/vpack/*.cpp src/vconsolid/*.cpp)

  # =========================================
  # Including VLIB sources
  # =========================================

  file(GLOB VLIB_SOURCES src/vlib/*.cpp src/vconsolid/*.cpp src/vdownload/*.cpp)

  # =========================================
  # Including all the headers
  # =========================================

  file(GLOB INCLUDE_HEADERS include/vconsolid/*.h include/vdownload/*.h
       include/vlib/*.h)

  # =========================================
  # Including OpenSSL library
  # =========================================

  set(OPENSSL_USE_STATIC_LIBS TRUE)
  find_package(OpenSSL REQUIRED)

  # =========================================
  # Adding all the projects
  # =========================================

  add_executable(${PROJECT_NAME} ${VUPDATE_SOURCES})
  add_executable(vpack ${VPACK_SOURCES})
  add_library(vlib STATIC ${VLIB_SOURCES})

  # =========================================
  # Setting target properties for vlib
  # =========================================

  set_target_properties(vlib PROPERTIES PUBLIC_HEADER include/vlib/vlib.h)

  # =========================================
  # Formatting the code using clang-format
  # =========================================

  find_program(CLANG_FORMAT "clang-format")
  if(WIN32 AND CLANG_FORMAT)
    add_custom_target(
      clang-format COMMAND clang-format -i -style=file ${VUPDATE_SOURCES}
                           ${VPACK_SOURCES} ${VLIB_SOURCES} ${INCLUDE_HEADERS})
    add_dependencies(${PROJECT_NAME} clang-format)
  else()
    add_custom_target(
      clang-format COMMAND clang-format-15 -i -style=file ${VUPDATE_SOURCES}
                           ${VPACK_SOURCES} ${VLIB_SOURCES} ${INCLUDE_HEADERS})
    add_dependencies(${PROJECT_NAME} clang-format)
  endif()

  # =========================================
  # Finding the libraries
  # =========================================

  find_library(
    CURL_LIBRARY
    NAMES curl
          curllib
          libcurl_imp
          curllib_static
          libcurl
          libcurl_imp-d
          libcurl-d
          libcurl-d_imp
    HINTS "${CMAKE_BINARY_DIR}/vlib/lib" REQUIRED)

  find_library(
    SPDLOG_LIBRARY
    NAMES spdlogd spdlog
    HINTS "${CMAKE_BINARY_DIR}/vlib/lib" REQUIRED)

  # =========================================
  # Including the directories and libraries
  # =========================================

  include_directories(
    "usr/include"
    "include"
    "${OPENSSL_INCLUDE_DIR}"
    "${CMAKE_BINARY_DIR}/vlib/include"
    "${CMAKE_BINARY_DIR}/indicators_project-prefix/src/indicators_project/single_include"
    "${CMAKE_BINARY_DIR}/json_project-prefix/src")

  # =========================================
  # Linking with the respective libraries
  # =========================================

  target_link_libraries(${PROJECT_NAME} PUBLIC OpenSSL::Crypto ${CURL_LIBRARY}
                                               ${SPDLOG_LIBRARY})
  target_link_libraries(vpack PUBLIC OpenSSL::Crypto ${SPDLOG_LIBRARY})
  target_link_libraries(vlib PRIVATE OpenSSL::Crypto ${CURL_LIBRARY}
                                     ${SPDLOG_LIBRARY})

  # =========================================
  # Setting the install targets
  # =========================================

  install(
    TARGETS vlib
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/vlib/include/vlib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/vlib/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/vlib/lib)
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL
                                             "RelWithDebInfo")
    install(FILES ${LIB_EAY_DEBUG} DESTINATION ${CMAKE_INSTALL_PREFIX}/vlib/lib)
    install(FILES ${SSL_EAY_DEBUG} DESTINATION ${CMAKE_INSTALL_PREFIX}/vlib/lib)
  else()
    install(FILES ${LIB_EAY_RELEASE}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/vlib/lib)
    install(FILES ${SSL_EAY_RELEASE}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/vlib/lib)
  endif()

  # =========================================
  # Removing the "cmake" and "pkgconfig" dirs
  # =========================================

  file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/vlib/lib/cmake)
  file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/vlib/lib/pkgconfig)
  file(REMOVE ${CMAKE_INSTALL_PREFIX}/vlib/bin/curl-config)

  # =========================================
  # Showing some information about the build
  # =========================================

  unset(COMPILE CACHE)
  message(STATUS "${}Project Name: ${PROJECT_NAME}")
  message(STATUS "${}Build Type: ${CMAKE_BUILD_TYPE}")
  message(STATUS "${}Build Directory: ${CMAKE_BINARY_DIR}")
  message(STATUS "${}Source Directory: ${PROJECT_SOURCE_DIR}")
  message(STATUS "${}INFOEX = ${INFOEX}")
  message(STATUS "${}Curl library path: ${CURL_LIBRARY}")
endif()
