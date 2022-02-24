set(beldex-lws-common_sources config.cpp error.cpp)
set(beldex-lws-common_headers config.h error.h )

add_library(beldex-lws-common ${beldex-lws-common_sources} ${beldex-lws-common_headers})
target_link_libraries(beldex-lws-common beldex::libraries)

add_executable(beldex_lws  server_main.cpp)

target_link_libraries(beldex_lws
  PRIVATE
    beldex::libraries
    bmq::bmq
    nlohmann_json::nlohmann_json
    ${Boost_CHRONO_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTRA_LIBRARIES}
    Threads::Threads
)
