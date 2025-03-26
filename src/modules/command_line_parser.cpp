#include "command_line_parser.h"
#include <stdexcept>
CommandLineParser::CommandLineParser() {
    setup_options();
}

void CommandLineParser::setup_options() {
    m_desc.add_options()
        ("help,h", "Show help message")
        ("version,v", "Show version information");
        }

CommandLineOptions CommandLineParser::parse(int argc, char* argv[]) {
    CommandLineOptions options;
    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, m_desc), vm);
        po::notify(vm);

        options.show_help = vm.count("help");
        options.show_version = vm.count("version");


    } catch (const po::error& e) {
        throw std::runtime_error("Command line error: " + std::string(e.what()));
    }

    return options;
}

void CommandLineParser::print_help(std::ostream& os) const {
    os << m_desc << "\n";
}

void CommandLineParser::print_version(std::ostream& os) const {
    os << "Version: 0.0.1\n";
}
