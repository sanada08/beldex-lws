#pragma once

#include <boost/program_options.hpp>
#include <string>

namespace lws {

struct command_line_options {
    std::string data_dir;
    std::string daemon_rpc; 
    bool testnet = false;
};

class command_line_parser {
  public:
    void parse_args(int argc, char* argv[]);
    void parse_args(std::vector<const char*> args);
    bool early_exit() const;

    const command_line_options& get_options() const;
    void print_usage() const;

  private:
    boost::program_options::options_description desc_;
    command_line_options options_;
    std::string binary_name_;
};

} // namespace beldex
