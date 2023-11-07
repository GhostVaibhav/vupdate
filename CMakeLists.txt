CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
include(ExternalProject)
include(FindOpenSSL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR} CACHE STRING "Final install location")
set(CMAKE ${CMAKE_COMMAND})

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR “${BoldRed}In-source build detected!${ColourReset}”)
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "MinSizeRel")
endif()

PROJECT("vupdater")

if((DEFINED INFOEX) AND (INFOEX EQUAL 1))
	set(CMAKE_C_FLAGS "-DHAVE_NO_INFOEX")
	set(CMAKE_CXX_FLAGS "-DHAVE_NO_INFOEX")
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

include_directories("include" "${CMAKE_BINARY_DIR}/include")

if(${COMPILE} EQUAL 0)
	if(WIN32)
		ExternalProject_Add(
			curl_project
			URL "https://github.com/curl/curl/releases/download/curl-8_4_0/curl-8.4.0.zip"
			CMAKE_ARGS
			-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
			-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
			-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
			-DBUILD_CURL_EXE=OFF
			-DBUILD_SHARED_LIBS=ON
			-DCURL_USE_OPENSSL=ON
			-DBUILD_TESTING=OFF
			-DUSE_WIN32_LDAP=ON
			-DENABLE_THREADED_RESOLVER=OFF
			-DUSE_LIBIDN2=OFF
		)
	else()
		if(APPLE)
		else()
			ExternalProject_Add(
				curl_project
				URL "https://github.com/curl/curl/releases/download/curl-8_4_0/curl-8.4.0.zip"
				CMAKE_ARGS
				-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
				-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
				-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
				-DBUILD_CURL_EXE=OFF
				-DBUILD_SHARED_LIBS=ON
				-DBUILD_TESTING=OFF
				-DCMAKE_USE_SCHANNEL=ON
				-DUSE_WIN32_LDAP=ON
				-DENABLE_THREADED_RESOLVER=OFF
				-DUSE_LIBIDN2=OFF
				BUILD_COMMAND ../curl_project/configure --with-openssl --prefix=${CMAKE_BINARY_DIR}
				COMMAND ${CMAKE_COMMAND} --build .
				INSTALL_COMMAND ${CMAKE_COMMAND} --build . -- install
			)
		endif()
	endif()
	unset(COMPILE CACHE)
	message(STATUS "${Green}Build the project to generate the dependencies${ColourReset}")
	message(STATUS "${Cyan}And then compile the project without any args${ColourReset}")
	message(STATUS "${Cyan}for building the main project${ColourReset}")
else()
    file(
        GLOB
        VUPDATE_SOURCES
        src/vupdate/*.cpp
		src/vdownload/*.cpp
		src/vconsolid/*.cpp
    )
	file(
		GLOB
		VPACK_SOURCES
		src/vpack/*.cpp
		src/vconsolid/*.cpp
	)
	file(
		GLOB
		INCLUDE_HEADERS
		include/vconsolid/*.h
		include/vdownload/*.h
	)
	file(
		DOWNLOAD
		https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.6/CPM.cmake
		${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
	)
	include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)
	CPMAddPackage(
		NAME nlohmann_json
		VERSION 3.11.2
		URL https://github.com/nlohmann/json/releases/download/v3.11.2/include.zip
	)
	CPMAddPackage("gh:gabime/spdlog@1.12.0")
	CPMAddPackage(
		NAME indicators
		GITHUB_REPOSITORY p-ranav/indicators
		VERSION 2.3
	)

	if (nlohmann_json_ADDED)
		include_directories("${CPM_PACKAGE_nlohmann_json_SOURCE_DIR}/single_include")
	endif()

	add_executable (${PROJECT_NAME} ${VUPDATE_SOURCES})
	add_executable (vpack ${VPACK_SOURCES})
	
	find_program(CLANG_FORMAT "clang-format")
	if(CLANG_FORMAT)
		add_custom_target(
			clang-format
			COMMAND clang-format
			-i
			-style=file
			${VUPDATE_SOURCES}
			${VPACK_SOURCES}
			${INCLUDE_HEADERS}
		)
		add_dependencies(${PROJECT_NAME} clang-format)
	endif()

	if(APPLE)
		find_library (
			CURL_LIBRARY
			NAMES curl curllib libcurl_imp curllib_static libcurl libcurl_imp-d libcurl-d libcurl-d_imp
			HINTS "usr/lib"
			REQUIRED
		)
	else()
		find_library (
			CURL_LIBRARY
			NAMES curl curllib libcurl_imp curllib_static libcurl libcurl_imp-d libcurl-d libcurl-d_imp
			HINTS "${CMAKE_BINARY_DIR}/lib"
			REQUIRED
		)
	endif()
	if(APPLE)
		include_directories("usr/include" "include" "${OPENSSL_INCLUDE_DIR}" "${CMAKE_BINARY_DIR}/include" "${indicators_SOURCE_DIR}")
		target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENSSL_CRYPTO_LIBRARY} ${CURL_LIBRARY} spdlog::spdlog indicators)
		target_link_libraries(vpack PUBLIC ${OPENSSL_CRYPTO_LIBRARY} spdlog::spdlog)
	else()
		include_directories("include" "${OPENSSL_INCLUDE_DIR}" "${CMAKE_BINARY_DIR}/include" "${indicators_SOURCE_DIR}")
		target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENSSL_CRYPTO_LIBRARY} ${CURL_LIBRARY} spdlog::spdlog indicators)
		target_link_libraries(vpack PUBLIC ${OPENSSL_CRYPTO_LIBRARY} spdlog::spdlog)
	endif()
    
	unset(COMPILE CACHE)
	message(STATUS "${}Project Name: ${PROJECT_NAME}")
	message(STATUS "${}Build Type: ${CMAKE_BUILD_TYPE}")
	message(STATUS "${}Build Directory: ${CMAKE_BINARY_DIR}")
	message(STATUS "${}Source Directory: ${PROJECT_SOURCE_DIR}")
	message(STATUS "${}INFOEX = ${INFOEX}")
	message(STATUS "${}Curl library path: ${CURL_LIBRARY}")
endif()
