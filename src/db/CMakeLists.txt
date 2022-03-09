set(beldex-lws-db_sources data.cpp storage.cpp )
set(beldex-lws-db_headers data.h storage.h )

add_library(beldex-lws-db ${beldex-lws-db_sources} ${beldex-lws-db_headers})
target_include_directories(beldex-lws-db PUBLIC "${LMDB_INCLUDE}")
target_link_libraries(beldex-lws-db beldex::libraries ${LMDB_LIB_PATH})
