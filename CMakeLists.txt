cmake_minimum_required(VERSION 3.1.0)
project(beldex-lws)


enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BELDEX_LIBRARIES
  common
  epee
  easylogging
  oxenmq
)


set(BELDEX_SEARCH_PATHS
  "/contrib/epee/src"
  "/external/easylogging++"
  "/src"
)

if (NOT BELDEX_SOURCE_DIR)
  message(FATAL_ERROR "The argument -DBELDEX_SOURCE_DIR must specify a location of a beldex source tree")
endif()

if (NOT BELDEX_BUILD_DIR)
  message(FATAL_ERROR "The argument -DBELDEX_BUILD_DIR must specify a location of an existing beldex build")
endif()

load_cache(${BELDEX_BUILD_DIR} READ_WITH_PREFIX beldex_
  Boost_THREAD_LIBRARY_RELEASE
  CMAKE_CXX_COMPILER
  beldex_SOURCE_DIR    
)

if (NOT (beldex_beldex_SOURCE_DIR MATCHES "${BELDEX_SOURCE_DIR}(/src/cryptonote_protocol)"))
  message(FATAL_ERROR "Invalid Beldex source dir - does not appear to match source used for build directory")
endif()

if (NOT (CMAKE_CXX_COMPILER STREQUAL beldex_CMAKE_CXX_COMPILER))
  message(FATAL_ERROR "Compiler for beldex build differs from this project")
endif()

#
# Dependencies specific to beldex-lws
#

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.58 QUIET REQUIRED COMPONENTS chrono filesystem program_options regex serialization system thread)

if (NOT (Boost_THREAD_LIBRARY STREQUAL beldex_Boost_THREAD_LIBRARY_RELEASE))
  message(STATUS "Found Boost_THREAD_LIBRARY: ${Boost_THREAD_LIBRARY}")
  message(STATUS "Found beldex_Boost_THREAD_LIBRARY_RELEASE: ${beldex_Boost_THREAD_LIBRARY_RELEASE}")
  message(FATAL_ERROR "Boost libraries for beldex build differs from this project")
endif()

foreach (LIB ${BELDEX_LIBRARIES})
  find_library(LIB_PATH NAMES "${LIB}" PATHS ${BELDEX_BUILD_DIR} PATH_SUFFIXES "/src/${LIB}" "external/${LIB}" ${MONERO_SEARCH_PATHS} NO_DEFAULT_PATH)

  list(FIND BELDEX_OPTIONAL "${LIB}" LIB_OPTIONAL)
  if (NOT LIB_PATH)
    set(LIB_NAME "beldex::${LIB}")
    add_library(${LIB_NAME} STATIC IMPORTED)
    set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${LIB_PATH})
    list(APPEND IMPORTED_BELDEX_LIBRARIES "${LIB_NAME}")
  endif()
  
  unset(LIB_PATH CACHE)
endforeach()

add_library(beldex::libraries INTERFACE IMPORTED)
set_property(TARGET beldex::libraries PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIR}
    "${BELDEX_SOURCE_DIR}/contrib/epee/include"
    "${BELDEX_SOURCE_DIR}/external/easylogging++"
    "${BELDEX_SOURCE_DIR}/external/loki-mq"
    "${BELDEX_SOURCE_DIR}/src"
)


set_property(TARGET beldex::libraries PROPERTY
  INTERFACE_LINK_LIBRARIES
    ${IMPORTED_BELDEX_LIBRARIES}
    ${Boost_CHRONO_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${CMAKE_DL_LIBS}
)

add_subdirectory(external)
add_subdirectory(src)