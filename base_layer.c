#include "base_layer.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

string *str_create(char *init) {
    string *s = (string *)malloc(sizeof(string));

    if (!s) return NULL;

    s->size = MAX(strlen(init) + 1, STRSIZE);
    s->str = (char *)malloc(s->size);

    strcpy(s->str, init);

    return s;
}

void str_free(string *s) {
    free(s->str);
    free(s);
}

void str_growto(string *s, unsigned int newsize) {
    if (newsize <= s->size) return;

    unsigned int oldsize = s->size;

    while (s->size < newsize)
        s->size *= 2;

    s->str = (char *)realloc((void *)s->str, s->size);

    memset((void *)(s->str + oldsize), '\0', s->size - oldsize);
}

void str_grow(string *s) {
    str_growto(s, s->size * 2);
}

void str_shrink(string *s) {
    s->size /= 2;
    s->size = MAX(s->size, STRSIZE);
    s->str = (char *)realloc((void *)s->str, s->size);
    s->str[s->size - 1] = '\0';
}

unsigned int str_sizeof(string *s) {
    return s->size;
}

unsigned int str_len(string *s) {
    return strlen(s->str);
}

char str_getc(string *s, unsigned int index) {
    if (index >= s->size) return '\0';

    return s->str[index];
}

char *str_get(string *s) {
    return s->str;
}

void str_append(string *s, char *app) {
    int len = strlen(s->str);

    str_growto(s, s->size + len);

    for (usize i = 0; i < strlen(app); i++) {
        s->str[len + i] = app[i];
    }
}

void str_cappend(string *s, char c) {
    unsigned int len = strlen(s->str);

    if (len >= s->size - 1)
        str_grow(s);

    s->str[len] = c;
}

void str_set(string *s, char *newstr) {
    str_growto(s, s->size + strlen(newstr));
    strcpy(s->str, newstr);
}

void str_setc(string *s, char c, unsigned int index) {
    s->str[index] = c;
}

bool str_cmp(string *s1, string *s2) {
    if (str_len(s1) != str_len(s2)) {
        return false;
    }

    bool is_eq = true;

    for (usize i = 0; i < str_len(s1); ++i) {
        if (s1->str[i] != s2->str[i]) {
            is_eq = false;
            break;
        }
    }

    return is_eq;
}
