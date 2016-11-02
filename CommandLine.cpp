#include "PreCompile.h"
#include "CommandLine.h"            // Pick up forward declarations to ensure correctness.
#include <PortableRuntime/CheckException.h>

namespace Parsing {

#if 0
std::vector<std::string> args_from_argv(int argc, _In_reads_(argc) wchar_t** argv)
{
    std::vector<std::string> args;

    std::for_each(argv + 1, argv + argc, [&args](PCWSTR arg)
    {
        args.push_back(PortableRuntime::utf8_from_utf16(arg));
    });

    return args;
}
#endif

static std::string argument_name_from_long_name(const std::string& long_name)
{
    // Strip leading "--" from long_name.
    return long_name.substr(2, std::string::npos);
}

void validate_argument_map(const std::vector<Argument_descriptor>& argument_map)
{
    const auto size = argument_map.size();
    for(size_t ix = 0; ix < size; ++ix)
    {
        // argument_map kind must match it's index, to guarantee that the enum
        // for the kind also matches the index.  This also guarantees the enum
        // monotonically increases, and that there are no gaps.  This may be
        // helpful for future optimizations.
        assert(argument_map[ix].key == ix);
    }

    // Validate short name arguments do not dupe.  Arguments are case sensitive.
    static_assert(sizeof(Argument_descriptor::short_name) == 1, "'used' array size depends on short_name being 1 byte.");
    bool used[256] {};
    for(size_t ix = 0; ix < size; ++ix)
    {
        if(argument_map[ix].short_name != 0)
        {
            assert(!used[argument_map[ix].short_name]);
            used[argument_map[ix].short_name] = true;
        }
    }
}

// Allows arguments to be specified more than once, with the last argument to take priority.
// Output is a map from ID to parameter (or "true" if no parameter required).
// Only arguments passed in the argument_map are allowed.
// Parameter validation must be done by client, as required parameters might have complex invariants,
// such as mutual exclusion, which cannot easily be represented in a table.
std::unordered_map<unsigned int, std::string> options_from_allowed_args(const std::vector<std::string>& arguments, const std::vector<Argument_descriptor>& argument_map)
{
    std::unordered_map<unsigned int, std::string> options;

    // Skip the first argument, as that is the executable name.
    const auto end = std::cend(arguments);
    for(auto argument = std::cbegin(arguments) + 1; argument != end; ++argument)
    {
        CHECK_EXCEPTION((argument->length() >= 2) && ((*argument)[0] == u8'-'), u8"Unrecognized argument: " + *argument);

        std::function<bool(const Argument_descriptor&)> predicate;
        if((*argument)[1] == u8'-')
        {
            // Handle long arguments, which are multi-character arguments prefixed with "--".
            const std::string argument_name = argument_name_from_long_name(*argument);
            predicate = [argument_name](const Argument_descriptor& descriptor)
            {
                return argument_name == descriptor.long_name;
            };
        }
        else
        {
            // Handle single character arguments, which are single character arguments prefixed with '-'.
            CHECK_EXCEPTION(argument->length() == 2, u8"Unrecognized argument: " + *argument);

            const char argument_character = (*argument)[1];
            predicate = [argument_character](const Argument_descriptor& descriptor)
            {
                return argument_character == descriptor.short_name;
            };
        }

        const auto& descriptor = std::find_if(std::cbegin(argument_map), std::cend(argument_map), predicate);

        // Validate that the argument was found in the passed in argument_map.
        CHECK_EXCEPTION(descriptor != std::cend(argument_map), u8"Unrecognized argument: " + *argument);

        // Get the key.
        unsigned int key = descriptor->key;

        // Get parameter for the argument.
        const bool requires_parameter = argument_map[key].requires_parameter;
        if(requires_parameter)
        {
            CHECK_EXCEPTION((argument + 1) != std::cend(arguments), u8"Argument missing required parameter: " + *argument);
            ++argument;

            options[key] = *argument;
        }
        else
        {
            options[key] = u8"true";
        }
    }

    return options;
}

std::string Options_help_text(const std::vector<Argument_descriptor>& argument_map)
{
    std::string help_text;

    size_t allocation_size = 0;
    size_t tab_index = 0;
    const auto size = argument_map.size();
    for(size_t ix = 0; ix < size; ++ix)
    {
        if(argument_map[ix].description != nullptr)
        {
            const size_t long_name_length = strlen(argument_map[ix].long_name);

            allocation_size += 9;   // "  -X, --\n".
            allocation_size += long_name_length;
            allocation_size += strlen(argument_map[ix].description);

            if(long_name_length > tab_index)
            {
                tab_index = long_name_length;
            }
        }
    }
    tab_index += 2; // Default tab distance.

    help_text.reserve(allocation_size);

    for(size_t ix = 0; ix < size; ++ix)
    {
        if(argument_map[ix].description != nullptr)
        {
            help_text += "  -";
            help_text += argument_map[ix].short_name;
            help_text += ", --";
            help_text += argument_map[ix].long_name;

            size_t space_count = tab_index;
            space_count -= strlen(argument_map[ix].long_name);
            for(size_t count = 0; count < space_count; ++count)
            {
                help_text += " ";
            }
            help_text += argument_map[ix].description;
            help_text += "\n";
        }
    }

    return help_text;
}

}

