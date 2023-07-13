#include "vCollideLoad.hpp"

#include <utilities/console/console.hpp>
hooks::VCollideLoad::value hooks::VCollideLoad::hook(FAST_ARGS, vcollide_t* output, int solidCount, const char* buffer, int size, bool swap)
{
    original(thisptr, output, solidCount, buffer, size, swap);

#if DUMP_WEATHER == true
    if (output && solidCount == 1 && output->descSize == 84) // see getvcollide dump
    {
        console::debug("(Possible) WEATHER BUFFER DUMP"); // because not always we will get the weather by that check
        console::detail::disablePrefix();

        console::debug("constexpr std::array<uint8_t, {}> weatherBuffer", size);
        console::debug("{");
        
        console::detail::disableNewLine();

        constexpr int limitPerLine = 16;

        for (int i = 0; i < size; i += limitPerLine)
        {
            console::debug("\t");

            for (int j = 0; j < limitPerLine && i + j < size; j++)
            {
                const uint8_t byte = buffer[i + j] & 0xFF;
                const std::string_view sign = (i + j + 1 < size) ? ", " : "";

                console::debug("0x{:02X}{}", byte, sign);
            }

            console::debug("\n");
        }

        console::debug("};\n");

        console::detail::enableNewLine();
        console::detail::enablePrefix();

        console::debug("END WEATHER DUMP");
    }
#endif
}