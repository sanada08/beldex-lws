set(beldex-lws-rpc_sources client.cpp )
set(beldex-lws-rpc_headers client.h )

add_library(beldex-lws-rpc ${beldex-lws-rpc_sources} ${beldex-lws-rpc_headers})
target_link_libraries(beldex-lws-rpc beldex::libraries bmq::bmq nlohmann_json::nlohmann_json)