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

#include <boost/filesystem/operations.hpp>
#include <boost/optional/optional.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/thread/thread.hpp>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "option.h"

namespace
{
  struct options : lws::options
  {
    const command_line::arg_descriptor<std::string> daemon_rpc;
    
    options()
      : lws::options()
      , daemon_rpc{"daemon", "<protocol>://<address>:<port> of a beldexd ZMQ RPC", ""}
    {}

    void prepare(boost::program_options::options_description& description) const
    {
      static constexpr const char rest_default[] = "https://0.0.0.0:8443";

      lws::options::prepare(description);
      command_line::add_arg(description, daemon_rpc);
    }
  };

  struct program
  {
    std::string daemon_rpc;
  };

  void print_help(std::ostream& out)
  {
    boost::program_options::options_description description{"Options"};
    options{}.prepare(description);

    out << "Usage: [options]" << std::endl;
    out << description;
  }

  boost::optional<program> get_program(int argc, char** argv)
  {
    namespace po = boost::program_options;

    const options opts{};
    po::variables_map args{};
    {
      po::options_description description{"Options"};
      opts.prepare(description);

      po::store(
        po::command_line_parser(argc, argv).options(description).run(), args
      );
      po::notify(args);
    }

    if (command_line::get_arg(args, command_line::arg_help))
    {
      print_help(std::cout);
      return boost::none;
    }

    opts.set_network(args); // do this first, sets global variable :/
    mlog_set_log_level(command_line::get_arg(args, opts.log_level));

    program prog{
      command_line::get_arg(args, opts.daemon_rpc)
    };

    return prog;
  }

  void run(program prog)
  {
    std::signal(SIGINT, [] (int) { lws::scanner::stop(); });

    boost::filesystem::create_directories(prog.db_path);
    auto ctx = lws::rpc::context::make(std::move(prog.daemon_rpc));

    MINFO("Using beldexd BMQ RPC at " << ctx.daemon_address());
  }
} // anonymous

int main(int argc, char** argv)
{
  tools::on_startup(); // if it throws, don't use MERROR just print default msg

  try
  {
    boost::optional<program> prog;

    try
    {
      prog = get_program(argc, argv);
    }
    catch (std::exception const& e)
    {
      std::cerr << e.what() << std::endl << std::endl;
      print_help(std::cerr);
      return EXIT_FAILURE;
    }

    if (prog)
      run(std::move(*prog));
  }
  catch (std::exception const& e)
  {
    MERROR(e.what());
    return EXIT_FAILURE;
  }
  catch (...)
  {
    MERROR("Unknown exception");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
