#include "input.hpp"

#include <unistd.h>
#include <string>
#include <algorithm>
#include <iostream>

namespace picosystem {
struct termios orig_termios;
std::vector<TerminalKey> prevKeys;
std::vector<TerminalKey> curKeys;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

TerminalKey readKey() {
    char c[4] = {0};
    int n = read(STDIN_FILENO, &c, 4);

    int code = 0;
    for (int i = 0; i < n; i++) {
        if(c[i] == 0) {
            break;
        }
        code <<= 8;
        code |= c[i];
    }
    TerminalKey kbd = *reinterpret_cast<TerminalKey*>(&code);
    return kbd;
}

void updateKeys() {
    prevKeys = std::vector<TerminalKey>(curKeys);
    curKeys.clear();
    while (true) {
        TerminalKey kbd = readKey();
        if (kbd == TerminalKey::None) {
            break;
        }
        curKeys.push_back(kbd);
    }
}

bool button(TerminalKey key) {
    return std::find(curKeys.begin(), curKeys.end(), key) != curKeys.end();
}

bool keyUp(TerminalKey key) {
    return std::find(curKeys.begin(), curKeys.end(), key) == curKeys.end();
}

bool pressed(TerminalKey key) {
    return std::find(curKeys.begin(), curKeys.end(), key) != curKeys.end() && std::find(prevKeys.begin(), prevKeys.end(), key) == prevKeys.end();
}

bool keyReleased(TerminalKey key) {
    return std::find(prevKeys.begin(), prevKeys.end(), key) != prevKeys.end() && std::find(curKeys.begin(), curKeys.end(), key) == curKeys.end();
}
}