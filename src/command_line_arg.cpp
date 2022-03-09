#include "command_line_arg.h"

#include <filesystem>
#include <iostream>

namespace lws
{

    namespace po = boost::program_options;
    namespace fs = std::filesystem;

    const command_line_options &command_line_parser::get_options() const
    {
        return options_;
    }

    void command_line_parser::parse_args(std::vector<const char *> args)
    {
        parse_args(args.size(), const_cast<char **>(args.data()));
    }
    void command_line_parser::parse_args(int argc, char *argv[])
    {
        po::options_description all;
        desc_.add_options()
        ("data-dir", po::value(&options_.data_dir),"Folder for LMDB files")
        ("daemon-rpc", po::value(&options_.daemon_rpc), "OMQ RPC address on which beldexd is available; tcp://localhost:22025")
        ("testnet", po::bool_switch(&options_.testnet), "Start Beldex LWS in testnet mode")
        ;

        all.add(desc_);

        po::variables_map vm;

        po::store(po::command_line_parser(argc, argv)
                      .options(all)
                      .run(),
                  vm);
        po::notify(vm);
    }

    void command_line_parser::print_usage() const
    {
        std::cerr << "Usage: " << binary_name_ << " <address> <port> [...]\n\n";

        desc_.print(std::cerr);

        std::cerr << std::endl;

        // print_log_levels();
    }
} // namespace lws
