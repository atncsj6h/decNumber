#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Copyright Enrico Sorichetti 2022 - 2022
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file BOOST_LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
message( STATUS "CMake version  '${CMAKE_VERSION}'" )
message( STATUS "Processing     '${CMAKE_CURRENT_LIST_FILE}'" )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
if( CMAKE_VERSION VERSION_LESS  3.22 )
  message( FATAL_ERROR "
 CMake '3.22' or higher is required,
 You are running '${CMAKE_VERSION}'" )
endif()
cmake_minimum_required( VERSION ${CMAKE_VERSION} )

if(POLICY CMP0129)
  cmake_policy( SET CMP0129 NEW ) # CMake 3.23
endif()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
if( NOT DEFINED ENV{CMTK} )
  message( FATAL_ERROR "
 unable to locate the cmToolKit directory" )
endif()

set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} )
list( APPEND CMAKE_MODULE_PATH "$ENV{CMTK}/cmake/Modules" )
set( TEMPL_SOURCE_DIR "$ENV{CMTK}/cmake/Templates" )

include( vDump )
include( vDrop )
include( vSnap )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# deprecate 'in-source' builds ( source directory/subdirectories )
# if You accidentally try an 'in-source' build
# You will have to manually clean up the leftovers :
# file 'CMakeCache.txt' and directory 'CMakeFiles'

set( CMAKE_DISABLE_IN_SOURCE_BUILD ON )
set( CMAKE_DISABLE_SOURCE_CHANGES  ON )

get_filename_component( _s "${CMAKE_SOURCE_DIR}" REALPATH )
string( TOLOWER "${_s}" _s )

get_filename_component( _b "${CMAKE_BINARY_DIR}" REALPATH )
string( TOLOWER "${_b}" _b )

if( ( "${_b}" STREQUAL "${_s}" ) OR
    ( "${_b}" MATCHES  "${_s}/" ) )
  message( FATAL_ERROR "
 binary directory '${CMAKE_BINARY_DIR}'
 source directory '${CMAKE_SOURCE_DIR}'
 builds that modify the source directory/subdirectories are not allowed,
 remove file 'CMakeCache.txt' and directory 'CMakeFiles' " )
endif( )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# must come before the project command
# lowest macOS supported will be 10.13.6 High Sierra
if( APPLE AND BUILD_OSX_UNIVERSAL_BINARIES )
  set( CMAKE_OSX_DEPLOYMENT_TARGET 10.13.6 CACHE STRING  "" FORCE)
  add_compile_options( -Wunguarded-availability )
endif()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
set( PROJECT decNumber )
set( VERSION 3.68.0 )
set( DESCRIPTION "General Decimal Arithmetic" )
set( PROVIDES decNumber )
set( REQUIRES )
set( SOVERSION )

project( ${PROJECT}
  VERSION ${VERSION}
  LANGUAGES C CXX
)

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
set( VERSION_STRING "${VERSION}" )
set( VERSION_STRING "${VERSION_STRING}-${CMAKE_SYSTEM_PROCESSOR}" )

string( TIMESTAMP BUILD_DATE  %Y/%m/%d )
string( TIMESTAMP BUILD_TIME  %H:%M:%S )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
if( APPLE AND BUILD_OSX_UNIVERSAL_BINARIES )
  set( CMAKE_OSX_ARCHITECTURES arm64 x86_64 )
  set( VERSION_STRING "${VERSION_STRING}-universal++" )
endif()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string( TIMESTAMP  BUILD_TIMESTAMP %Y/%m/%d-%H:%M:%S )
set( VERSION_STRING "${VERSION_STRING} ${BUILD_DATE}" )

message( STATUS "Building ${PROJECT} ${VERSION_STRING}" )
add_compile_definitions(
  VERSION_STRING="${VERSION_STRING}" )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
set( CMAKE_C_STANDARD 11 )
set( CMAKE_C_STANDARD_REQUIRED YES )
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED YES )

set( CMAKE_POSITION_INDEPENDENT_CODE ON )
set( CMAKE_EXPORT_COMPILE_COMMANDS  ON )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
set( SYSTEM_NAME "${CMAKE_HOST_SYSTEM_NAME}" )
string( TOUPPER "${SYSTEM_NAME}" SYSTEM_NAME_UC  )
set( "${SYSTEM_NAME_UC}" 1 )
add_compile_definitions(
  ${SYSTEM_NAME_UC}
)

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# arch/bits
math( EXPR HOST_ARCHITECTURE "8 * ${CMAKE_SIZEOF_VOID_P}" OUTPUT_FORMAT DECIMAL)
if( NOT ( HOST_ARCHITECTURE MATCHES 64 ) )
  message( SEND_ERROR "
 unsupported Architecture '${HOST_ARCHITECTURE}'" )
endif()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# endian-ness
if( NOT ( CMAKE_C_BYTE_ORDER STREQUAL "LITTLE_ENDIAN" ) )
  message( SEND_ERROR "
 unsupported Endian-ness '${CMAKE_C_BYTE_ORDER}'" )
endif()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# setup the install paths according to the GNU standards
include( installDirectories )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${INST_BIN_DIR}" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${INST_LIB_DIR}" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${INST_LIB_DIR}" )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# the compiler flags
include( defaultCompilerFlags )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# rpath handling
set( CMAKE_SKIP_BUILD_RPATH  FALSE )
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
set( CMAKE_INSTALL_RPATH "@executable_path/../${INST_LIB_DIR}" )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
option( BUILD_SHARED  ""  ON )
option( BUILD_STATIC  ""  ON )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
option( WITH_CCACHE   ""  ON )
if( WITH_CCACHE )
  include( ccache )
endif()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
option( WITH_IPO      ""  ON )
if( WITH_IPO )
  include( CheckIPOSupported )
  check_ipo_supported( RESULT HAVE_IPO_SUPPORT )
  if ( HAVE_IPO_SUPPORT )
    message( STATUS "IPO is Supported and Enabled" )
  endif()
endif()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
include( CheckIncludeFile )
include( checkHeader )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# we assume that the system has the ISO/IEC headers and terminate if not
include( isoHeaders )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# check for __int128_t
include( checkType )
type_is_defined( __int128_t )
type_get_size( __int128_t )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# functions
include( CheckFunctionExists )
include( checkFunction )

# the builtins
include( __builtin_bswap )
include( __builtin_clz )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# just to warn about some odd definitions
include( haveBrokenSizeMax )
include( fastTypesLengthsMatch )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# attributes
include( inlineAttr )
include( threadLocalAttr )
include( weakAttr )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# the compiler flags
include( defaultCompilerFlags )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
include( ${PROJECT}_DEFS OPTIONAL )
include( ${PROJECT}_DIAG OPTIONAL )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
)

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# sources and headers
include( ${PROJECT}_SRCS )
include( ${PROJECT}_HDRS OPTIONAL )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# the libraries ( SHARED/STATIC )

include( buildLibraries )

foreach( library  ${PROVIDES} )

  if( BUILD_SHARED OR BUILD_STATIC )
    build_headers( ${library} )
  endif()

  build_objects( ${library} )

  if( BUILD_SHARED )
    build_shared_library( ${library} )
  endif()

  if( BUILD_STATIC )
    build_static_library( ${library} )
  endif()

endforeach()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# platform.h
if( EXISTS ${CMAKE_SOURCE_DIR}/platform.h.in )
  configure_file( ${CMAKE_SOURCE_DIR}/platform.h.in
    ${CMAKE_BINARY_DIR}/platform.h
  )
  add_compile_definitions(
    HAVE_PLATFORM_H )
endif()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# config.h
if( EXISTS ${CMAKE_SOURCE_DIR}/config.h.in )
  configure_file( ${CMAKE_SOURCE_DIR}/config.h.in
    ${CMAKE_BINARY_DIR}/config.h
  )
  add_compile_definitions(
    HAVE_CONFIG_H )
endif()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# the pkgconfig control file ${PROJECT}.pc

set( INST_LIBS "")
foreach( library  ${PROVIDES} )
  set( INST_LIBS "${INST_LIBS}-l${library} " )
endforeach()
string( STRIP "${INST_LIBS}"  INST_LIBS )

include( buildPkgconfigPc OPTIONAL)
if( COMMAND build_pkgconfig_pc )
  build_pkgconfig_pc( ${PROJECT} )
endif()
if( COMMAND build_pkgconfig_sh )
  build_pkgconfig_sh( ${PROJECT} )
endif()

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# the uninstaller
configure_file( $ENV{CMTK}/cmake/Templates/cmake_uninstall.cmake.in
  ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake
  IMMEDIATE @ONLY )
add_custom_target( uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
include( configurationReport )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
vdump( "main" "exit" )

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
include( cpack )
