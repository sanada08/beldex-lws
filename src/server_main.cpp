// Copyright (c) 2018-2020, The Monero Project
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "command_line_arg.h"
#include "utils.hpp"

#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <vector>

extern "C"
{
#include <sys/types.h>
#include <pwd.h>

#ifdef ENABLE_SYSTEMD
#include <systemd/sd-daemon.h>
#endif
}

namespace fs = std::filesystem;

namespace
{
  void run(lws::command_line_options options)
  {
    std::filesystem::path data_dir;

    if (options.data_dir.empty())
    {
      if (auto home_dir = util::get_home_dir())
      {
        data_dir = options.testnet
                       ? *home_dir / ".beldex" / "testnet" / "lws"
                       : *home_dir / ".beldex" / "lws";
        std::cout << "data_dir::" << data_dir << std::endl;
      }
      else
      {
        std::cerr << "Could not determine your home directory; please use --data-dir to specify a data directory\n";
      }
    }
    else
    {
      data_dir = std::filesystem::u8path(options.data_dir);
    }

    if (!fs::exists(data_dir))
      fs::create_directories(data_dir);

    if (options.testnet)
    {
      "Starting in testnet mode, make sure this is intentional!");
    }

    auto bmq_server_ptr = std::make_unique<BeldexmqServer

    // auto ctx = lws::rpc::context::make(std::move(options.daemon_rpc));
  }
}

std::atomic<int> signalled = 0;
extern "C" void handle_signal(int sig)
{
  signalled = sig;
}

int main(int argc, char **argv)
{
  std::signal(SIGINT, handle_signal);
  std::signal(SIGTERM, handle_signal);

  lws::command_line_parser parser;

  try
  {
    parser.parse_args(argc, argv);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    parser.print_usage();
    return EXIT_FAILURE;
  }

  auto options = parser.get_options();

  run(std::move(options));
}
