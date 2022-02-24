#include "bmq/bmq.h"
#include "bmq/connections.h"
#include <iostream>
#include <string_view>
#include <vector>
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>
#include "scanner.h"
#include "error.h"

int main()
{
    bmq::address rpcURL("tcp://127.0.0.1:4567");
    bmq::LogLevel logLevel = bmq::LogLevel::warn;

    bmq::BMQ lmq{
      [](bmq::LogLevel lvl, const char* file, int line, std::string msg) {
        std::cout << lvl << " [" << file << ":" << line << "] " << msg << std::endl;
      },
      logLevel};

    lmq.start();

    std::promise<bool> connectPromise;

    const auto connID = lmq.connect_remote(
      rpcURL,
      [&connectPromise](auto) { connectPromise.set_value(true); { std::cout << "connected: ";}},
      [&connectPromise](auto, std::string_view msg) {
        std::cout << "failed to connect to belnet RPC: " << msg << std::endl;
        connectPromise.set_value(false);
      });

    // auto data{lws::error::daemon_timeout};
    // bool success;


    lmq.request(connID, "rpc.get_height", [](bool success, auto data){
        if ( success == 1 && data[0] == "200"){
            std::cout<< "data[1]::::" << data[1]<< std::endl;
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(5));

    while(!success){
      (!scanner::is_running())
          return {lws::error::signal_abort_process};

        if (sync_rpc_timeout <= (std::chrono::steady_clock::now() - start))
          return {lws::error::daemon_timeout};

        if (!resp.matches(std::errc::timed_out))
          return resp.error();
      }
    }
    
}