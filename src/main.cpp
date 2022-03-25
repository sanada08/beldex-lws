#include "command_line.h"
#include "beldex_logger.h"
#include "utils.hpp"
#include "beldexd_rpc.h"
#include "beldex_logger.h"
#include "beldexd_key.h"
#include "mn_record.h"

#include <sodium/core.h>

#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <vector>

extern "C" {
#include <sys/types.h>
#include <pwd.h>

#ifdef ENABLE_SYSTEMD
#include <systemd/sd-daemon.h>
#endif
}

namespace fs = std::filesystem;

std::atomic<int> signalled = 0;
extern "C" void handle_signal(int sig) {
    signalled = sig;
}

int main(int argc, char* argv[]) {

    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    beldex::command_line_parser parser;

    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        parser.print_usage();
        return EXIT_FAILURE;
    }

    auto options = parser.get_options();

    if (options.print_help) {
        parser.print_usage();
        return EXIT_SUCCESS;
    }

    if (options.print_version) {
        std::cout << "version";
        return EXIT_SUCCESS;
    }

    std::filesystem::path data_dir;
    if (options.data_dir.empty()) {
        if (auto home_dir = util::get_home_dir()) {
            data_dir = options.testnet
                ? *home_dir / ".beldex" / "testnet" / "lws"
                : *home_dir / ".beldex" / "lws";
        } else {
            std::cerr << "Could not determine your home directory; please use --data-dir to specify a data directory\n";
            return EXIT_FAILURE;
        }
    } else {
        data_dir = std::filesystem::u8path(options.data_dir);
    }

    if (!fs::exists(data_dir))
        fs::create_directories(data_dir);

    beldex::LogLevel log_level;
    if (!beldex::parse_log_level(options.log_level, log_level)) {
        std::cerr << "Incorrect log level: " << options.log_level << std::endl;
        beldex::print_log_levels();
        return EXIT_FAILURE;
    }

    beldex::init_logging(data_dir, log_level);

    if (options.testnet) {
        BELDEX_LOG(warn,
                 "Starting in testnet mode, make sure this is intentional!");
    }

    if (options.ip == "127.0.0.1") {
        BELDEX_LOG(critical,
                 "Tried to bind beldex-storage to localhost, please bind "
                 "to outward facing address");
        return EXIT_FAILURE;
    }

    BELDEX_LOG(info, "Setting log level to {}", options.log_level);
    BELDEX_LOG(info, "Setting database location to {}", data_dir);
    BELDEX_LOG(info, "Connecting to beldexd @ {}", options.beldexd_omq_rpc);

    if (sodium_init() != 0) {
        BELDEX_LOG(err, "Could not initialize libsodium");
        return EXIT_FAILURE;
    }

    {
        const auto fd_limit = util::get_fd_limit();
        if (fd_limit != -1) {
            BELDEX_LOG(debug, "Open file descriptor limit: {}", fd_limit);
        } else {
            BELDEX_LOG(debug, "Open descriptor limit: N/A");
        }
    }

    try {
        using namespace beldex;

        std::vector<x25519_pubkey> stats_access_keys;
        for (const auto& key : options.stats_access_keys) {
            stats_access_keys.push_back(x25519_pubkey::from_hex(key));
            BELDEX_LOG(info, "Stats access key: {}", key);
        }

#ifndef INTEGRATION_TEST
        const auto [private_key, private_key_ed25519, private_key_x25519] =
            get_mn_privkeys(options.beldexd_omq_rpc, [] { return signalled == 0; });
#else
        // Normally we request the key from daemon, but in integrations/swarm
        // testing we are not able to do that, so we extract the key as a
        // command line option:
        legacy_seckey private_key{};
        ed25519_seckey private_key_ed25519{};
        x25519_seckey private_key_x25519{};
        try {
            private_key = legacy_seckey::from_hex(options.beldexd_key);
            private_key_ed25519 = ed25519_seckey::from_hex(options.beldexd_ed25519_key);
            private_key_x25519 = x25519_seckey::from_hex(options.beldexd_x25519_key);
        } catch (...) {
            BELDEX_LOG(critical, "This storage server binary is compiled in integration test mode: "
                "--beldexd-key, --beldexd-x25519-key, and --beldexd-ed25519-key are required");
            throw;
        }
#endif
        if (signalled) {
            BELDEX_LOG(err, "Received signal {}, aborting startup", signalled.load());
            return EXIT_FAILURE;
        }

        mn_record me{private_key.pubkey(), private_key_ed25519.pubkey(), private_key_x25519.pubkey()};

        BELDEX_LOG(info, "Retrieved keys from beldexd; our MN pubkeys are:");
        BELDEX_LOG(info, "- legacy:  {}", me.pubkey_legacy);
        BELDEX_LOG(info, "- ed25519: {}", me.pubkey_ed25519);
        BELDEX_LOG(info, "- x25519:  {}", me.pubkey_x25519);
        BELDEX_LOG(info, "- beldexnet: {}", me.pubkey_ed25519.mnode_address());

    } catch (const std::exception& e) {
        // It seems possible for logging to throw its own exception,
        // in which case it will be propagated to libc...
        std::cerr << "Exception caught in main: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
