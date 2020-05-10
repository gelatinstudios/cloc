#ifndef JT_STRING_H
#define JT_STRING_H

#ifndef JT_Allocate

#define Win32Alloc(size) (VirtualAlloc(0, (size), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))
#define Win32Free(ptr)   (VirtualFree(ptr, 0, MEM_RELEASE))

#ifdef _WIN32
#define JT_Allocate(x) Win32Alloc(x)
#define JT_Deallocate(x) Win32Free(x)
#else
#define JT_Allocate(x) malloc(x)
#define JT_Deallocate(x) free(x)
#endif

#endif // JT_Allocate

#include "jt_arrays.h"

// null-terminated AND count INCULDES the null-terminator
// therefore: string_length(string) == strlen(string.e)

typedef struct {
    size_t count;
    char *e;
} string;

string make_string(char *c) {
    string result;
    result.count = strlen(c) + 1;
    result.e = (char *)JT_Allocate(result.count);
    memcpy(result.e, c, result.count);
    return result;
}

void free_string(string s) {
    JT_Deallocate(s.e);
}

size_t string_length(string s) {
    return s.count-1;
}

string concat(string a, string b) {
    string result;
    result.count = a.count + b.count - 1;
    result.e = (char *)JT_Allocate(result.count);
    memcpy(result.e, a.e, a.count - 1);
    memcpy(result.e + a.count - 1, b.e, b.count);
    return result;
}

GenerateArrayType(string);

string_array split(string s, char c) {
    string_array result = {0};
    result.e = (string *)JT_Allocate(s.count);
    string new_string = {0};
    new_string.e = s.e;
    For (char, s) {
        ++new_string.count;
        if (*it == c) {
            *it = 0;
            ArrayPush(result, new_string);
            new_string.e = it + 1;
            new_string.count = 0;
        }
    }
    ArrayPush(result, new_string);
    return result;
}
char string_last(string s) {
    return s.e[s.count-2];
}

char string_pop_back(string *s) {
    char *end = &s->e[s->count-2];
    char result = *end;
    *end = 0;
    s->count--;
    return result;
}

char string_advance(string *s) {
    char result = s->e[0];
    s->e++;
    s->count--;
    return result;
}

void string_to_lower(string s) {
    For (char, s) *it = tolower(*it);
}

void string_to_upper(string s) {
    For (char, s) *it = toupper(*it);
}

int is_space(char c) {
    return ((c == ' ')  ||
            (c == '\n') ||
            (c == '\r') ||
            (c == '\t'));
}

void strip_surrounding_whitespace(string *s) {
    while (is_space(s->e[0]))         string_advance(s);
    while (is_space(string_last(*s))) string_pop_back(s);
}
#endif //JT_STRING_H
