#include "dnload.h"

#include "vgl_array.hpp"
#include "vgl_string.hpp"
#include "vgl_vector.hpp"

using vgl::array;
using vgl::string;
using vgl::vector;

#if defined(USE_LD)
using vgl::to_string;
#endif

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
    /// Number strings.
    vector<string> number_vector;
    number_vector.emplace_back("int8_t number: ");
    number_vector.emplace_back(to_string(static_cast<int8_t>(0x80)));
    number_vector.emplace_back("\nuint8_t number: ");
    number_vector.emplace_back(to_string(static_cast<uint8_t>(0xFF)));
    number_vector.emplace_back("\nint16_t number: ");
    number_vector.emplace_back(to_string(static_cast<int16_t>(0x8000)));
    number_vector.emplace_back("\nuint16_t number: ");
    number_vector.emplace_back(to_string(static_cast<uint16_t>(0xFFFF)));
    number_vector.emplace_back("\nint32_t number: ");
    number_vector.emplace_back(to_string(static_cast<int32_t>(0x80000000)));
    number_vector.emplace_back("\nuint32_t number: ");
    number_vector.emplace_back(to_string(static_cast<uint32_t>(0xFFFFFFFF)));
    number_vector.emplace_back("\nint64_t number: ");
    number_vector.emplace_back(to_string(static_cast<int64_t>(0x8000000000000000)));
    number_vector.emplace_back("\nuint64_t number: ");
    number_vector.emplace_back(to_string(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)));

    for(const auto& str : number_vector)
    {
        for(const auto& character : str)
        {
            dnload_putchar(character);
        }
    }
    dnload_putchar('\n');
#endif

#if defined(USE_LD)
    (void)argc;
    (void)argv;
    return 0;
#else
    asm_exit();
#endif
}
