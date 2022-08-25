#pragma once


#include <termios.h>
#include <vector>


namespace picosystem {
    
enum class TerminalKey {
    None = 0,
    Up = '\x1b[A',
    Down = '\x1b[B',
    Right = '\x1b[C',
    Left = '\x1b[D',
    Backtab = '\x1b[Z',
    Escape = '\x1b',
    Backspace = '\x7f',
    Enter = '\r',
    Tab = '\t',
    Space = ' ',
    Zero = '0',
    One = '1',
    Two = '2',
    Three = '3',
    Four = '4',
    Five = '5',
    Six = '6',
    Seven = '7',
    Eight = '8',
    Nine = '9',
    Colon = ':',
    Semicolon = ';',
    Less = '<',
    Equals = '=',
    Greater = '>',
    Question = '?',
    At = '@',
    A = 'a',
    B = 'b',
    C = 'c',
    D = 'd',
    E = 'e',
    F = 'f',
    G = 'g',
    H = 'h',
    I = 'i',
    J = 'j',
    K = 'k',
    L = 'l',
    M = 'm',
    N = 'n',
    O = 'o',
    P = 'p',
    Q = 'q',
    R = 'r',
    S = 's',
    T = 't',
    U = 'u',
    V = 'v',
    W = 'w',
    X = 'x',
    Y = 'y',
    Z = 'z',
    BracketLeft = '[',
    Backslash = '\\',
    BracketRight = ']',
    Quote = '\'',
    Comma = ',',
    Period = '.',
    Slash = '/',
    Backquote = '`',
};

void disableRawMode();
void enableRawMode();
TerminalKey readKey();
void updateKeys();

bool button(TerminalKey key);
bool keyUp(TerminalKey key);
bool pressed(TerminalKey key);
bool keyReleased(TerminalKey key);
}
