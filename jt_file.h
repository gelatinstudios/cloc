#ifndef JT_FILE_H
#define JT_FILE_H

typedef struct {
    size_t size;
    u8 *contents;
} file;

#ifdef _WIN32

#include <windows.h>
#include <fileapi.h>

static file win32_read_file(const char *filename) {
    file result = {0};
    
    HANDLE f = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    
    if (f == INVALID_HANDLE_VALUE) {
        return result;
    }
    
    result.size = GetFileSize(f, 0);
    
    result.contents = (u8 *)VirtualAlloc(0, result.size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    DWORD bytes_read;
    ReadFile(f, result.contents, result.size, &bytes_read, 0);
    
    assert(bytes_read == result.size);
    
    CloseHandle(f);
    
    return result;
}

static void win32_free_file(file f) {
    VirtualFree(f.contents, 0, MEM_RELEASE);
}

#endif

// C std lib wrapper
static size_t get_file_size(FILE *f) {
    if (!f) return 0;
    
    fseek(f, 0, SEEK_END);
    size_t result = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    return result;
}

static file read_file(const char *filename) {
    file result = {0};
    
    FILE *f = fopen(filename, "rb");
    
    if (!f) return result;
    
    fseek(f, 0, SEEK_END);
    result.size = ftell(f);
    result.contents = (u8 *)malloc(result.size);
    fseek(f, 0, SEEK_SET);
    fread(result.contents, result.size, 1, f);
    fclose(f);
    
    return result;
}

static void free_file(file f) {
    free(f.contents);
}

static u32 count_file_lines(file f) {
    u32 result = 0;
    
    for (char *c = (char *)f.contents, *end = c + f.size; c != end; ++c) {
        if (*c == '\n') ++result;
    }
    
    return result;
}

#endif //JT_FILE_H
