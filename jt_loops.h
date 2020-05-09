
#pragma once

// NOTE: requires jt_int.h to be #included first

#define ArrayCount(a) (sizeof(a)/sizeof(a[0]))

#pragma once

#define ForVar(_jt_var, _jt_cnt) for (size_t _jt_var = 0, _jt_count = (_jt_cnt); _jt_var != _jt_count; ++_jt_var)
#define ForI(count) ForVar(i, count)
#define ForJ(count) ForVar(j, count)
#define ForX(count) ForVar(x, count)
#define ForY(count) ForVar(y, count)
#define ForICount(array) ForI(ArrayCount(array))

#ifdef __cplusplus

#define ForCArray(arr) for (auto *it = arr, *_jt_end = arr + ArrayCount(arr); it != _jt_end; ++it)
#define ForCArrayReverse(arr) for (auto *it = arr + ArrayCount(arr) - 1, *_jt_end = arr - 1; it != _jt_end; --it)

#else 

#define ForCArray(type, arr) for (type *it = arr, *_jt_end = arr + ArrayCount(arr); it != _jt_end; ++it)
#define ForCArrayReverse(type, arr) for (type *it = arr + ArrayCount(arr) - 1, *_jt_end = arr - 1; it != _jt_end; --it)

#endif

// stretchy_buffer.h
#define ForSB(type, sb) for (type *it = sb, *_jt_end = sb + sb_count(sb); it != _jt_end; ++it)
#define ForISB(sb) ForI(sb_count(sb))
