#pragma once 

#include <cstdint>
#include <queue>
#include <SDL2/SDL.h>

namespace picosystem {
// #define PIXEL_DOUBLE
#define __isr


#define __TOP_OFFSET 13
#define __BOTTOM_OFFSET 10
#define __LEFT_OFFSET 5

#define __SET_CURSOR(x,y) "\x1b["<<y<<";"<<x<<"H"

#define __RESET_CURSOR __SET_CURSOR(0, __TOP_OFFSET)
#define __RESET_COLOR "\x1b[0m"
#define __RESET __RESET_COLOR << __RESET_CURSOR

#define __A_MASK 0x00f0
#define __A_OFFSET 4
#define __R_MASK 0x000f
#define __R_OFFSET 0
#define __G_MASK 0xf000
#define __G_OFFSET 12
#define __B_MASK 0x0f00
#define __B_OFFSET 8
#define __A(x) (((x) & __A_MASK) >> __A_OFFSET)
#define __R(x) (((x) & __R_MASK) >> __R_OFFSET)
#define __G(x) (((x) & __G_MASK) >> __G_OFFSET)
#define __B(x) (((x) & __B_MASK) >> __B_OFFSET)



#define __FG(color) "\x1b[38;2;"<<__R(color)*16<<";"<<__G(color)*16<<";"<<__B(color)*16<<"m"
#define __BG(color) "\x1b[48;2;"<<__R(color)*16<<";"<<__G(color)*16<<";"<<__B(color)*16<<"m"
#define __colorize(bg, fg) __BG(bg) << __FG(fg)



void dma_channel_transfer_from_buffer_now(uint32_t channel, const volatile void * buffer, uint32_t size);
void __isr dma_complete();

struct BeepObject{
    double freq;
    int vol;
    int samplesLeft;
};

class Beeper{
private:
    double v;
    std::queue<BeepObject> beeps;
public:
    Beeper();
    ~Beeper();
    void beep(double freq, int volume, int duration);
    void generateSamples(Sint16 *stream, int length);
    void wait();
};

}
