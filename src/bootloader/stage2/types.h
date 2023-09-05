#pragma once

#define bool __Bool

typedef unsigned char          u8;
typedef unsigned short int     u16;
typedef unsigned int           u32;
typedef unsigned long long int u64;

typedef char          i8;
typedef short int     i16;
typedef int           i32;
typedef long long int i64;

typedef u64 size_t;

typedef float  f32;
typedef double f64;

typedef int __Bool;

typedef char* str;

#define true 1
#define false 0

#define NULL ((void *)0)

bool isLower(char chr);
bool isUpper(char chr);

char toUpper(char chr);
char toLower(char chr);
