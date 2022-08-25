#include <math.h>
#include <chrono>
#include <string.h>
#include <thread>
#include <iostream>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <queue>
#include <cmath>

#include "picosystem.hpp"
#include "mock.hpp"
#include "timer.hpp"

#define FREQUENCY 44100
#define AMPLITUDE 32767

namespace picosystem {


  void audio_callback(void*, Uint8*, int);

  Beeper::Beeper()
  {
      SDL_AudioSpec desiredSpec;

      desiredSpec.freq = FREQUENCY;
      desiredSpec.format = AUDIO_S16SYS;
      desiredSpec.channels = 1;
      desiredSpec.samples = 2048;
      desiredSpec.callback = audio_callback;
      desiredSpec.userdata = this;

      SDL_AudioSpec obtainedSpec;

      // you might want to look for errors here
      SDL_OpenAudio(&desiredSpec, &obtainedSpec);

      // start play audio
      SDL_PauseAudio(0);
  }

  Beeper::~Beeper()
  {
      SDL_CloseAudio();
  }

  void Beeper::generateSamples(Sint16 *stream, int length)
  {
      memset(stream, 0, length * sizeof(Sint16));
      int i = 0;
      while (i < length) {

          if (beeps.empty()) {
              while (i < length) {
                  stream[i] = 0;
                  i++;
              }
              return;
          }
          BeepObject& bo = beeps.front();

          int samplesToDo = std::min(i + bo.samplesLeft, length);
          bo.samplesLeft -= samplesToDo - i;

          while (i < samplesToDo) {
              stream[i] = bo.vol * AMPLITUDE * std::sin(v * 2 * M_PI / FREQUENCY);
              i++;
              v += bo.freq;
          }

              beeps.pop();
          // if (bo.samplesLeft == 0) {
          // }
      }
  }

  // void Beeper::beepRaw(uint32_t freq, uint32_t sam) {
  //     BeepObject bo;
  //     bo.freq = freq;
  //     bo.samplesLeft = duration * FREQUENCY / 1000;

  //     SDL_LockAudio();
  //     beeps.push(bo);
  //     SDL_UnlockAudio();
  // }
  void Beeper::beep(double freq, int volume, int duration)
  {
      BeepObject bo;
      bo.freq = freq;
      bo.vol = volume;
      bo.samplesLeft = duration * FREQUENCY / 1000;

      SDL_LockAudio();
      beeps.push(bo);
      SDL_UnlockAudio();
  }

  void Beeper::wait()
  {
      int size;
      do {
          // SDL_Delay(0);
          SDL_LockAudio();
          size = beeps.size();
          SDL_UnlockAudio();
      } while (size > 0);

  }

  void audio_callback(void *_beeper, Uint8 *_stream, int _length)
  {
      Sint16 *stream = (Sint16*) _stream;
      int length = _length / 2;
      Beeper* beeper = (Beeper*) _beeper;

      beeper->generateSamples(stream, length);
  }



  Beeper b;


  // PIO               screen_pio  = pio0;
  uint              screen_sm   = 0;
  uint32_t          dma_channel;
  volatile int16_t  dma_scanline = -1;

  uint32_t         _audio_pwm_wrap = 5000;
  CallBackTimer _audio_update_timer;

  enum pin {
    RED = 14, GREEN = 13, BLUE = 15,                  // user rgb led
    CS = 5, SCK = 6, MOSI  = 7,                       // spi
    VSYNC = 8, DC = 9, LCD_RESET = 4, BACKLIGHT = 12, // screen
    AUDIO = 11,                                       // audio
    CHARGE_LED = 2, CHARGING = 24, BATTERY_LEVEL = 26 // battery / charging
  };

  void init_inputs(uint32_t pin_mask) {
  }

  void init_outputs(uint32_t pin_mask) {
  }

  bool pressed(uint32_t b) {
    switch(b) {
      case UP:    return pressed(TerminalKey::W);
      case DOWN:  return pressed(TerminalKey::S);
      case LEFT:  return pressed(TerminalKey::A);
      case RIGHT: return pressed(TerminalKey::D);
      case A:     return pressed(TerminalKey::Down);
      case B:     return pressed(TerminalKey::Right);
      case X:     return pressed(TerminalKey::Left);
      case Y:     return pressed(TerminalKey::Up);
      default:    return false;
    }
  }

  bool button(uint32_t b) {
    switch(b) {
      case UP:    return button(TerminalKey::W);
      case DOWN:  return button(TerminalKey::S);
      case LEFT:  return button(TerminalKey::A);
      case RIGHT: return button(TerminalKey::D);
      case A:     return button(TerminalKey::Down);
      case B:     return button(TerminalKey::Right);
      case X:     return button(TerminalKey::Left);
      case Y:     return button(TerminalKey::Up);
      default:    return false;
    }
  }

  void _reset_to_dfu() {
  }

  float _battery_voltage() {
    return 3.0f; // correct for voltage divider on board
  }
  uint32_t t;
  uint32_t time() {
    
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  }

  uint32_t time_us() {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  }

  void sleep(uint32_t d) {
    std::this_thread::sleep_for(std::chrono::milliseconds(d));
  }

  void sleep_us(uint32_t d) {
    std::this_thread::sleep_for(std::chrono::microseconds(d));
  }

  uint32_t battery() {
    float c = (_battery_voltage() - 2.8f) / 1.3f;
    return std::max(0.0f, std::min(1.0f, c)) * 100;
  }
  uint32_t lastVsync = time();
  void _wait_vsync() {
    int delta = time()-lastVsync;
    while((delta = time()-lastVsync) < 33) {
      sleep(34 - delta);
    }
    lastVsync = time();
  }

  bool _in_flip = false;
  bool _is_flipping() {
    return _in_flip;
  }

  void transmit_scanline() {
    uint32_t *s = (uint32_t *)&SCREEN->data[
      ((dma_scanline - 1) < 0 ? 0 : (dma_scanline - 1)) * 120
    ];
    uint16_t c = (dma_scanline == 0 || dma_scanline == 120) ? 60 : 120;

    dma_channel_transfer_from_buffer_now(dma_channel, s, c);
  }

  void __isr dma_complete() {
    _in_flip = false;
  }

  void _flip() {
    if(!_is_flipping()) {
    _in_flip = true;
      uint32_t c = SCREEN->w * SCREEN->h / 2;

      dma_channel_transfer_from_buffer_now(dma_channel, SCREEN->data, c);
    }
  }

  uint16_t _gamma_correct(uint8_t v) {
    float gamma = 2.8;
    return (uint16_t)(pow((float)(v) / 100.0f, gamma) * 65535.0f + 0.5f);
  }

  void backlight(uint8_t b) {
  }

  void _play_note(uint32_t f, uint32_t v, uint32_t d) {
    
    b.beep(f, v, d);
    b.wait();
  }

  void led(uint8_t r, uint8_t g, uint8_t b) {
  }

  uint32_t _gpio_get() {
    return 0;
  }

  void _start_audio() {
    _audio_update_timer.start(1, std::bind(_update_audio));
  }

  void _finalize_hardware() {
    _audio_update_timer.stop();
    disableRawMode();
  }

  void _init_hardware() {
    atexit(_finalize_hardware);
    enableRawMode();
  }
}