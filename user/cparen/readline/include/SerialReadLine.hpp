#ifndef SERIALREADLINE_H
#define SERIALREADLINE_H

#include <Arduino.h>

class SerialReadLine {
public:
    // Returns the completed line buffer (const char*) if ENTER is pressed, otherwise returns nullptr.
    char* update();
    void reset();

private:
    static const int MAX_LINE_LENGTH = 160;
    char lineBuffer[MAX_LINE_LENGTH + 1]; // +1 for null terminator
    int bufferLength = 0;   // The total number of characters in the buffer
    int cursorPosition = 0; // The index where the next character will be inserted

    // ASCII control codes
    const char ASCII_BACKSPACE = 8;
    const char ASCII_CR = 13;   // Carriage Return
    const char ASCII_NL = '\n'; // Newline
    const char ASCII_ESC = 27;  // Escape (Start of an arrow key sequence)
    const char ASCII_DELETE = 127;

    // Helper to send ANSI escape codes for cursor movement
    void moveCursor(int count, char direction);

    // Helper to redraw the entire line from the cursor position
    void redrawLine();
};

#endif