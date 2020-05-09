
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDEBUG
#include <assert.h>

#include <windows.h>

#include "jt_int.h"
#include "jt_file.h"
#include "jt_loops.h"
#include "jt_arrays.h"

const char *code_exts[] = {
    "sh",
    "bat",
    "c",
    "cpp",
    "h",
    "hpp",
    "jc",
    "jh",
    "jl",
    "java",
    "rs",
    "cs",
    "lisp",
};

b32 is_space(char c) {
    return ((c == ' ')  |
            (c == '\n') |
            (c == '\r') |
            (c == '\t'));
}

void strip_tail_whitespace(char *str) {
    char *c = str + strlen(str);
    assert(*c == 0);
    do --c; while (is_space(*c));
    c[1] = 0;
}

typedef const char* string;

GenerateArrayType(string);

string_array get_blacklist(void) {
    FILE *in = fopen("cloc_blacklist.txt", "r");
    if (!in) return (string_array) {0};
    
    size_t size = get_file_size(in) + 1;
    
    string_array result = MallocEmptyArray(string, size);
    
    char *buffer = malloc(size);
    char *buffer_end = buffer + size;
    
    char *buffer_cursor = buffer;
    
    while (fgets(buffer_cursor, buffer_end - buffer_cursor, in)) {
        strip_tail_whitespace(buffer_cursor);
        
        ArrayPush(result, buffer_cursor);
        buffer_cursor += strlen(buffer_cursor) + 1;
    }
    
    fclose(in);
    
    return result;
}

b32 in_blacklist(const char *filename, string_array blacklist) {
    For (string, blacklist)
        if (!strcmp(filename, *it)) return true;
    
    return false;
}

const char *get_extension(const char *filename) {
    b32 has_dot = false;
    const char *c = filename;
    while (*c) {
        if (*c == '.') {
            has_dot = true;
            break;
        }
        ++c;
    }
    
    if (!has_dot) return NULL;
    
    c = filename + strlen(filename);
    while (*c != '.') --c;
    return c + 1;
}

b32 has_valid_extension(const char *filename) {
    if (!strcmp(filename, "."))  return false;
    if (!strcmp(filename, "..")) return false;
    
    const char *ext = get_extension(filename);
    if (!ext) return false;
    
    ForCArray(string, code_exts)
        if (!strcmp(ext, *it)) return true;
    
    return false;
}

typedef struct {
    sint loc;
    sint sloc;
} lines_of_code;

lines_of_code count_lines_of_code(const char *filename) {
    lines_of_code result = {0};
    
    file in = read_file(filename);
    if (!in.size) {
        return result;
    }
    
    b32 significant = false;
    
    char *c = in.contents;
    char *end = c + in.size;
    
    while (c < end) {
        char ch = *c++;
        significant |= !is_space(ch);
        if (ch == '\n') {
            ++result.loc;
            if (significant) ++result.sloc;
            significant = false;
        }
    }
    
    return result;
}

void set_count_str(char *str, sint count) {
    if (count == 0) strcpy(str, "-");
    else {
        snprintf(str, 17, "%lld", count);
    }
}

int main(void) {
    string_array blacklist = get_blacklist();
    
    DWORD buffer_len = GetCurrentDirectory(0, 0);
    buffer_len += 256;
    char *directory = malloc(buffer_len);
    GetCurrentDirectory(buffer_len, directory);
    strcat(directory, "\\*");
    
    size_t total_loc = 0;
    size_t total_sloc = 0;
    
    printf("%-16s %10s %10s\n", "filename", "loc", "sloc");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    
    WIN32_FIND_DATAA ffd;
    HANDLE search_handle = FindFirstFileA(directory, &ffd);
    b32 running = true;
    while (running) {
        char *filename = ffd.cFileName;
        if (has_valid_extension(filename) && !in_blacklist(filename, blacklist)) {
            lines_of_code loc = count_lines_of_code(filename);
            total_loc  += loc.loc;
            total_sloc += loc.sloc;
            
            if (strlen(filename) > 16) {
                char abbrev[17] = {0};
                const char *ext = get_extension(filename);
                size_t n = 16 - strlen(ext) - 2;
                strncpy(abbrev, filename, n);
                strcat(abbrev, "..");
                strcat(abbrev, ext);
                filename = abbrev;
            }
            
            char loc_str[17];  set_count_str(loc_str,  loc.loc);
            char sloc_str[17]; set_count_str(sloc_str, loc.sloc);
            
            printf("%-16s %10s %10s\n", filename, loc_str, sloc_str);
        }
        
        running = FindNextFileA(search_handle, &ffd);
    }
    
    printf("\n%-16s %10zu %10zu\n", "totals",  total_loc, total_sloc);
}
