
#pragma once

#define _JT_ARRAY

#ifdef __cplusplus

template <typename T>
struct array {
    size_t count;
    T *e;
};

#define MallocArray(type, count) {count, (type *)malloc((count)*sizeof(type))}
#define MallocEmptyArray(type, count) {0, (type *)malloc((count)*sizeof(type))}

#define For(arr) for (auto *it = arr.e, *_jt_end = arr.e + arr.count; it != _jt_end; ++it)
#define ForReverse(arr) for (auto *it = arr.e + arr.count - 1, *_jt_end = arr.e - 1; it != _jt_end; --it)

#else // C

#define GenerateArrayType(type) typedef struct { u32 count; type *e; } type##_array

#define MallocArray(type, count) (type##_array){count, (type *)malloc((count)*sizeof(type))}
#define MallocEmptyArray(type, count) (type##_array){0, (type *)malloc((count)*sizeof(type))}

#define For(type, arr) for (type *it = arr.e, *_jt_end = arr.e + arr.count; it != _jt_end; ++it)
#define ForReverse(type, arr) for (type *it = arr.e + arr.count - 1, *_jt_end = arr.e - 1; it != _jt_end; --it)

#endif // #ifdef __cplusplus

#define ArrayFromCArray(carr) {(sizeof(carr)/sizeof(carr[0])), carr}

// for stack-like behavior with buffers > count
#define ArrayPush(arr, v) (arr.e[arr.count++] = (v))
#define ArrayPop(arr, v)  (arr.e[--arr.count] = (v))
