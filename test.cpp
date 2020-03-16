#include "dnload.h"

#include "vgl_array.hpp"
#include "vgl_string.hpp"
#include "vgl_vector.hpp"

using vgl::array;
using vgl::string;
using vgl::vector;

/// Main entry point.
///
/// To understand the significance of USE_LD and dnload(), see:
/// http://faemiyah.fi/demoscene/dnload
#if defined(USE_LD)
int main(int argc, char** argv)
#else
void _start()
#endif
{
    dnload();

    // Replacement for std::vector and std::string.
    vector<string> string_vector;
    string_vector.push_back(string("This"));
    string_vector.push_back(string("is"));
    string_vector.push_back(string("a"));
    string_vector.push_back(string("string"));
    string_vector.push_back(string("vector."));

    for(const auto& str : string_vector)
    {
        for(const auto& character : str)
        {
            dnload_putchar(character);
        }
        dnload_putchar(' ');
    }
    dnload_putchar('\n');

    // Replacement for std::array.
    array<char, 21> char_array =
    {
        'T', 'h', 'i', 's', ' ',
        'i', 's', ' ',
        'a', ' ',
        'c', 'h', 'a', 'r', ' ',
        'a', 'r', 'r', 'a', 'y', '.'
    };

    for(const auto& character : char_array)
    {
        dnload_putchar(character);
    }
    dnload_putchar('\n');

#if defined(USE_LD)
    (void)argc;
    (void)argv;
    return 0;
#else
    asm_exit();
#endif
}
