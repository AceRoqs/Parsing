#pragma once

namespace Parsing
{

struct Argument_descriptor
{
    unsigned int key;
    const char* long_name;
    char short_name;
    bool requires_parameter;
    const char* description;
};

void validate_argument_map(const std::vector<Argument_descriptor>& argument_map);
std::unordered_map<unsigned int, std::string> options_from_allowed_args(const std::vector<std::string>& arguments, const std::vector<Argument_descriptor>& argument_map);
std::string Options_help_text(const std::vector<Parsing::Argument_descriptor>& argument_map);

}

