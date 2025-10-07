#include <Arduino.h>
#include "SerialReadLine.hpp"

void SerialReadLine::reset() {
    bufferLength = cursorPosition = 0;
}

// Helper function to send ANSI escape codes to the terminal
// Example: moveCursor(3, 'D') sends ESC[3D to move the cursor left 3 columns
void SerialReadLine::moveCursor(int count, char direction) {
    if (count > 0) {
        Serial.print(ASCII_ESC);
        Serial.print('[');
        Serial.print(count);
        Serial.print(direction); // 'C' for right, 'D' for left
    }
}

// Helper to redraw the line when characters are inserted or deleted in the middle
void SerialReadLine::redrawLine() {
    // 1. Print the rest of the string from the cursor position
    Serial.print(&lineBuffer[cursorPosition]);
    
    // 2. Overwrite the final character position with a space to clear any residue
    Serial.print(' ');
    
    // 3. Move the cursor back to its correct logical position
    //    We moved (bufferLength - cursorPosition) + 1 positions forward, so we move back the same amount.
    int moves = (bufferLength - cursorPosition) + 1;
    moveCursor(moves, 'D'); // 'D' is for left
}

const char* SerialReadLine::update() {
    if (!Serial.available()) {
        return nullptr;
    }
    
    char incomingChar = Serial.read();

    // --- 1. Handle ENTER Key ---
    if (incomingChar == ASCII_CR || incomingChar == ASCII_NL) {
        // Go to the end of the line if we're not there (visual consistency)
        moveCursor(bufferLength - cursorPosition, 'C'); 
        
        lineBuffer[bufferLength] = '\0'; // Null-terminate the string
        bufferLength = 0;
        cursorPosition = 0;
        return lineBuffer; // Return the completed line

    // --- 2. Handle BACKSPACE / DELETE Key ---
    } else if (incomingChar == ASCII_BACKSPACE || incomingChar == ASCII_DELETE) {
        if (cursorPosition > 0) {
            // Shift remaining characters left
            for (int i = cursorPosition - 1; i < bufferLength; i++) {
                lineBuffer[i] = lineBuffer[i + 1];
            }
            bufferLength--;
            cursorPosition--;

            // Move cursor back and redraw
            Serial.print('\b'); // Move physical cursor back 1
            //redrawLine();
            Serial.print(' '); // overwrite last
            Serial.print('\b');
        }
        return nullptr;

    // --- 3. Handle ARROW KEYS (Escape Sequences) ---
    } else if (incomingChar == ASCII_ESC) {
        // Must read the next two characters for the full sequence (e.g., [A, [B, [C, [D)
        
        // Wait up to 5ms for the next char of the sequence
        long startTime = millis();
        while (Serial.available() < 2 && (millis() - startTime) < 5) {} 

        if (Serial.available() >= 2) {
            char nextChar1 = Serial.read(); // Should be '['
            char nextChar2 = Serial.read(); // Should be A, B, C, or D

            if (nextChar1 == '[') {
                if (nextChar2 == 'D') { // LEFT Arrow
                    if (cursorPosition > 0) {
                        cursorPosition--;
                        moveCursor(1, 'D'); // Move physical cursor left 1
                    }
                } else if (nextChar2 == 'C') { // RIGHT Arrow
                    if (cursorPosition < bufferLength) {
                        cursorPosition++;
                        moveCursor(1, 'C'); // Move physical cursor right 1
                    }
                }
                // UP/DOWN arrow ('A'/'B') logic for history would go here
            }
        }
        return nullptr;

    // --- 4. Handle PRINTABLE CHARACTER ---
    } else if (incomingChar >= 32 && bufferLength < MAX_LINE_LENGTH) {
        // Insert and shift the rest of the string right
        if (cursorPosition < bufferLength) {
            for (int i = bufferLength; i > cursorPosition; i--) {
                lineBuffer[i] = lineBuffer[i - 1];
            }
            lineBuffer[cursorPosition] = incomingChar;
            bufferLength++;
            cursorPosition++;

            // Print the character and redraw the rest of the line
            Serial.print(incomingChar);
            redrawLine();
        } else {
            // Simple append at the end
            lineBuffer[cursorPosition] = incomingChar;
            bufferLength++;
            cursorPosition++;
            Serial.print(incomingChar);
        }
        return nullptr;
    }
    
    // Ignore all other characters
    return nullptr;
}