add_executable(beldex_lws  client.cpp)

target_link_libraries(beldex_lws
  PUBLIC
    bmq::bmq
    nlohmann_json::nlohmann_json
)


# install(TARGETS beldex-lws DESTINATION bin)