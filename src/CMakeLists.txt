add_definitions(-DDISABLE_ENCRYPTION)

add_library(beldex_lws_lib STATIC
    main.cpp
    command_line.cpp
    omq_server.cpp
    beldexd_rpc.cpp
    )

target_compile_options(beldex_lws_lib PRIVATE -Wall -Werror)

target_link_libraries(beldex_lws_lib PUBLIC
    common utils crypto
    uWebSockets
    cpr::cpr
    jemalloc::jemalloc
    OpenSSL::SSL OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    bmq::bmq
    Boost::boost Boost::program_options)

find_library(RESOLV resolv)
if(RESOLV)
    target_link_libraries(beldex_lws_lib PUBLIC ${RESOLV})
endif()

set(BIN_NAME beldex-light-wallet)

add_executable(beldex_lws main.cpp)
target_compile_options(beldex_lws PRIVATE -Wall -Wextra -Werror)
set_target_properties(beldex_lws PROPERTIES OUTPUT_NAME ${BIN_NAME})
target_link_libraries(beldex_lws PRIVATE beldex_lws_lib)
install(TARGETS beldex_lws DESTINATION bin)
# Build Info
find_package(Git)
if(GIT_FOUND)
    execute_process(
        COMMAND
            git rev-parse --short HEAD
        OUTPUT_VARIABLE
            SHORT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    set(SHORT_HASH "unknown")
endif()
string(TIMESTAMP BUILD_TIME UTC)
message(STATUS "using git commit hash ${SHORT_HASH}")
message(STATUS "using UTC build time ${BUILD_TIME}")
# configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")
# target_sources(beldex_lws_lib PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")
target_include_directories(beldex_lws_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if(NOT BUILD_STATIC_DEPS)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(SYSTEMD libsystemd)
        # Default ENABLE_SYSTEMD to true if we found it
        option(ENABLE_SYSTEMD "enable systemd integration for sd_notify" ${SYSTEMD_FOUND})

        if(ENABLE_SYSTEMD)
            if(NOT SYSTEMD_FOUND)
                message(FATAL_ERROR "libsystemd not found")
            endif()
            target_compile_definitions(beldex_lws PRIVATE ENABLE_SYSTEMD)
            target_include_directories(beldex_lws PRIVATE ${SYSTEMD_INCLUDE_DIRS})
            target_link_libraries(beldex_lws PRIVATE ${SYSTEMD_LIBRARIES})
        endif()
    endif()
endif()

