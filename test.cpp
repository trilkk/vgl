#include "dnload.h"

#include "vgl_array.hpp"

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
    // TODO

    // Replacement for std::array.
    vgl::array<char, 21> char_array =
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
