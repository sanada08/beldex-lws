
include_directories(.)
# add_subdirectory(rpc)

add_library(beldex_lws_lib STATIC
server_main.cpp
command_line_arg.cpp
)


target_link_libraries(beldex_lws_lib
  PUBLIC
    beldex::libraries
    utils
    bmq::bmq
    nlohmann_json::nlohmann_json
    lmdb
    ${Boost_CHRONO_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTRA_LIBRARIES}
    Threads::Threads
)


add_executable(beldex_lws server_main.cpp )

target_link_libraries(beldex_lws PRIVATE beldex_lws_lib)