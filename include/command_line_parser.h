#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include "command_line_options.h"
#include <boost/program_options.hpp>
#include <iosfwd>
#include<iostream>
namespace po = boost::program_options;
class CommandLineParser {
public:
    CommandLineParser();
    CommandLineOptions parse(int argc, char* argv[]);
    void print_help(std::ostream& os = std::cout) const;
    void print_version(std::ostream& os = std::cout) const;
private:
    void setup_options();
    po::options_description m_desc;
};

#endif
