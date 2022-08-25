#include "mock.hpp"

#include <iostream>
#include <sstream>

namespace picosystem {

int offset = 0;

void dma_channel_transfer_from_buffer_now(uint32_t channel, const volatile void * buffer, uint32_t size) {

    uint16_t* _buffer = (uint16_t*)buffer;

    #ifdef PIXEL_DOUBLE
        int width = 120;
        int height = 120;
    #else
        int width = 240;
        int height = 240;
    #endif
    std::cout << __RESET;
    int stride = width * sizeof(uint16_t);
    for (int y = 0; y<height/2; y++) {
        std::stringstream ss;
        ss << std::string(__LEFT_OFFSET, ' ');
        for (int x = 0; x<width; x++) {
            uint16_t upper = *(uint16_t*)((uint8_t*)_buffer + (offset = (y*2)*stride + x*sizeof(uint16_t)));
            uint16_t lower = *(uint16_t*)((uint8_t*)_buffer + (y*2+1)*stride + x*sizeof(uint16_t));
            ss << __colorize(upper, lower)<< "▄";
        }
        std::cout << ss.str() << __RESET_COLOR << "\r\n";
    }

    std::cout << __SET_CURSOR(0, height/2 + __TOP_OFFSET + __BOTTOM_OFFSET) << __RESET_COLOR << "\r\n";

    dma_complete();
}

}
