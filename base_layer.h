#ifndef BASE_LAYER_H
#define BASE_LAYER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TYPES */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef int32_t bool;
typedef char byte;

typedef ptrdiff_t size;
typedef size_t usize;
typedef ssize_t ssize;

/* BOOLEAN VALUES */
#define null NULL
#define true 1
#define false 0

/* COMMON OPERATORS */
#define todo(x) fprintf(stdout, "%s", x)
#define array_size(arr) (sizeof(arr) / sizeof(arr[0]))
#define assert(c) \
    while (!(c))  \
    __builtin_unreachable()

/* STATIC KEYWORD PRECISIONS */
#define global static
#define local static
#define function static

/* STRINGS */
#define STRSIZE 10

typedef struct {
    char *str;
    unsigned int size;
} string;

string *str_create(char *init);
void str_free(string *s);

void str_grow(string *s);
void str_growto(string *s, unsigned int newsize);
void str_shrink(string *s);

unsigned int str_sizeof(string *s);
unsigned int str_len(string *s);

void str_append(string *s, char *app);
void str_cappend(string *s, char c);

char str_getc(string *s, unsigned int index);
char *str_get(string *s);

void str_set(string *s, char *newstr);
void str_setc(string *s, char c, unsigned int index);

bool str_cmp(string *s1, string *s2);

/* DYNAMIC ARRAYS */
// TODO(jocabin): insert, append, delete, get, update, clear

/* ARENA MEMORY ALLOCATOR */

#endif  // BASE_LAYER_H
