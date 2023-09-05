#include "types.h"

bool isLower(char chr) {
    return chr >= 'a' && chr <= 'z';
}

bool isUpper(char chr) {
    return chr >= 'A' && chr <= 'Z';
}

char toUpper(char chr) {
    return isLower(chr) ? (chr - 'a' + 'A') : chr;
}

char toLower(char chr) {
    return isUpper(chr) ? (chr - 'A' + 'a') : chr;
}
