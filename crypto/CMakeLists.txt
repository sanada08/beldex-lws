
add_library(crypto STATIC
src/beldexd_key.cpp)

target_include_directories(crypto PUBLIC include)

find_package(Threads)

target_link_libraries(crypto
PUBLIC
common
utils
PRIVATE
OpenSSL::SSL
sodium
Boost::boost
Threads::Threads
${CMAKE_DL_LIBS})
