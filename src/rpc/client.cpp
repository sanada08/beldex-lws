#include "bmq/bmq.h"
#include "bmq/connections.h"
#include <iostream>
#include <string_view>
#include <vector>
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>

namespace lws
{
  void BeldexmqServer::connect_beldexd(const bmq::address &beldex_rpc)
  {
    auto start = std::chrono::steady_clock::now();
    while (true) {
        std::promise<bool> prom;
        BELDEX_LOG(info, "Establishing connection to beldexd...");
        omq_.connect_remote(beldexd_rpc,
            [this, &prom](auto cid) { beldexd_conn_ = cid; prom.set_value(true); },
            [&prom, &beldexd_rpc](auto&&, std::string_view reason) {
                BELDEX_LOG(warn, "failed to connect to local beldexd @ {}: {}; retrying", beldexd_rpc, reason);
                prom.set_value(false);
            },
            // Turn this off since we are using oxenmq's own key and don't want to replace some existing
            // connection to it that might also be using that pubkey:
            oxenmq::connect_option::ephemeral_routing_id{},
            oxenmq::AuthLevel::admin);

        if (prom.get_future().get()) {
            BELDEX_LOG(info, "Connected to beldexd in {}",
                    util::short_duration(std::chrono::steady_clock::now() - start));
            break;
        }
        std::this_thread::sleep_for(500ms);
    }
  }
}

/******************************************************************************************************
 * Basic program for connecting BMQ
 /*****************************************************************************************************
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

*******************************************************************************************************/