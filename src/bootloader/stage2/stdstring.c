#include "stdstring.h"

unsigned strlen(str str) {
    if (str == NULL) return 0;
    int i = 0;
    while(*str){i++; str++;}
    return i;
}

str strsearch(str str, char c) {
    if(str == NULL) return NULL;
    while (*str){
        if(*str == c) return str;
        str++;
    }
    return NULL;
}

str strcopy(str des, str src) {
    char* tmp = des;
    if (des == NULL) return NULL;
    if (src == NULL) {*des='\0'; return des;}

    while (src) {
        *des = *src;
        des++; src++;
    }
    *des = '\0';
    return tmp;
}
