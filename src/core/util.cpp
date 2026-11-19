#include "util.h"

// Primitive helper utilities, clamping, and screen control
#include <iostream>
using namespace std;


// Returns val limited to the range [minVal, maxVal]
int clampInt(int val, int minVal, int maxVal) {
    if (val < minVal) {
        return minVal;
    }
    if (val > maxVal) {
        return maxVal;
    }
    return val;
}

// Removes leading and trailing whitespace from str, in place
void trimWhitespace(char* str) {
    if (str == nullptr) {
        return;
    }

    // Find first non-whitespace character
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' ||
           str[start] == '\n' || str[start] == '\r') {
        start++;
    }

    // Find end of string
    int end = start;
    while (str[end] != '\0') {
        end++;
    }

    // Walk back over trailing whitespace
    while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t' ||
                           str[end - 1] == '\n' || str[end - 1] == '\r')) {
        end--;
    }

    // Shift the trimmed content to the front and terminate
    int i = 0;
    while (start + i < end) {
        str[i] = str[start + i];
        i++;
    }
    str[i] = '\0';
}

// Returns true if both C-strings have exactly the same characters
bool stringEquals(const char* a, const char* b) {
    if (a == nullptr || b == nullptr) {
        return a == b;
    }

    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return false;
        }
        i++;
    }
    return a[i] == b[i];   // both must end at the same position
}

// Prints a title surrounded by a border line
void printHeader(const char* title) {
    int len = 0;
    if (title != nullptr) {
        while (title[len] != '\0') {
            len++;
        }
    }

    int width = len + 4;

    cout << "\n";
    for (int i = 0; i < width; i++) {
        cout << '=';
    }
    cout << "\n";

    cout << "  " << (title != nullptr ? title : "") << "\n";

    for (int i = 0; i < width; i++) {
        cout << '=';
    }
    cout << "\n\n";
}

// Clears the terminal using ANSI escape codes (clear screen + cursor to top-left)
void clearScreen() {
    cout << "\033[2J\033[H";
    cout.flush();
}