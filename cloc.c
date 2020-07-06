
#define NDEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include <windows.h>
#include <io.h>

#include "jt_int.h"
#include "jt_loops.h"
#include "jt_arrays.h"
#include "jt_file.h"
#include "jt_string.h"

static const char *default_code_exts[] = {
    "sh",
    "bat",
    "c",
    "cpp",
    "h",
    "hpp",
    "java",
    "rs",
    "cs",
    "lisp",
    "py",
    "js",
    "php",
    "d"
};

typedef const char* c_str;

static string_array get_blacklist(void) {
    string_array result = {0};
    
    string entire_file = read_file_into_string("cloc_blacklist.txt");
    if (!entire_file.count) return result;
    
    result = split(entire_file, '\n');
    For (string, result) strip_surrounding_whitespace(it);
    
    return result;
}

static b32 in_blacklist(const char *filename, string_array blacklist) {
    For (string, blacklist)
        if (!strcmp(filename, it->e)) return true;
    
    return false;
}

static const char *get_extension(const char *filename) {
    const char *c = filename + strlen(filename);
    while (*c != '.' && c != filename) --c;
    return c + 1;
}

static b32 has_valid_extension(const char *filename, const char *code_exts[], size_t code_exts_count) {
    if (!strcmp(filename, "."))  return false;
    if (!strcmp(filename, "..")) return false;
    
    const char *ext = get_extension(filename);
    for (int i = 0; i < code_exts_count; ++i)
        if (!strcmp(ext, code_exts[i])) return true;
    
    return false;
}

typedef struct {
    sint loc;
    sint sloc;
} lines_of_code;

static lines_of_code count_lines_of_code(const char *filename) {
    lines_of_code result = {0};
    
    file in = read_file(filename);
    if (!in.size) return result;
    
    b32 significant = false;
    
    char *c = in.contents;
    char *end = c + in.size;
    
    while (c < end) {
        char ch = *c++;
        significant |= !is_space(ch);
        if (ch == '\n') {
            ++result.loc;
            result.sloc += significant;
            significant = false;
        }
    }
    
    if (c[-1] != '\n') {
        ++result.loc;
        result.sloc += significant;
    }
    
    free_file(in);
    
    return result;
}

static void set_count_str(char *str, sint count) {
    if (count == 0) strcpy(str, "-");
    else snprintf(str, 32, "%lld", count);
}

static char buffer[1<<24] = {0};

static int write_to_buffer(const char *format, ...) {
    static char tmp[256];
    
    va_list args;
    va_start(args, format);
    
    int result = vsnprintf(tmp, 256, format, args);
    strcat(buffer, tmp);
    
    va_end(args);
    
    return result;
}

int main(int argc, const char *argv[]) {
    string_array blacklist = get_blacklist();
    
    const char **code_exts;
    size_t code_exts_count;
    if (argc < 2) {
        code_exts = default_code_exts;
        code_exts_count = ArrayCount(default_code_exts);
    } else {
        code_exts = &argv[1];
        code_exts_count = argc - 1;
    }
    
    DWORD dir_buffer_len = GetCurrentDirectory(0, 0);
    dir_buffer_len += 256;
    char *directory = Win32Alloc(dir_buffer_len);
    GetCurrentDirectory(dir_buffer_len, directory);
    strcat(directory, "\\*");
    
    size_t total_loc  = 0;
    size_t total_sloc = 0;
    
    size_t buffer_size = 0;
    
    buffer_size += write_to_buffer("%-31s %10s %10s\r\n", "filename", "loc", "sloc");
    buffer_size += write_to_buffer("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
    
    WIN32_FIND_DATAA ffd;
    HANDLE search_handle = FindFirstFileA(directory, &ffd);
    b32 running = true;
    while (running) {
        char *filename = ffd.cFileName;
        if (has_valid_extension(filename, code_exts, code_exts_count) && 
            !in_blacklist(filename, blacklist)) {
            lines_of_code loc = count_lines_of_code(filename);
            total_loc  += loc.loc;
            total_sloc += loc.sloc;
            
            if (strlen(filename) > 23) {
                char abbrev[24] = {0};
                const char *ext = get_extension(filename);
                size_t n = 23 - strlen(ext) - 2;
                strncpy(abbrev, filename, n);
                strcat(abbrev, "..");
                strcat(abbrev, ext);
                filename = abbrev;
            }
            
            char loc_str[32];  set_count_str(loc_str,  loc.loc);
            char sloc_str[32]; set_count_str(sloc_str, loc.sloc);
            
            buffer_size += write_to_buffer("%-31s %10s %10s\r\n", filename, loc_str, sloc_str);
        }
        
        running = FindNextFileA(search_handle, &ffd);
    }
    
    buffer_size += write_to_buffer("\r\n%-31s %10zu %10zu\r\n", "totals",  total_loc, total_sloc);
    
    _write(1, buffer, buffer_size);
}
