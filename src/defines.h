#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef float f32;
typedef double f64;

#if defined(WIN32) || defined(_WIN32) || defined (_WIN32_)
#define PLATFORM_WIN 1
#ifndef _WIN64
#error "Only 64-bit Windows allowed."
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM_LINUX 1
#endif