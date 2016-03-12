#ifndef LIB_SORT_H
#define LIB_SORT_H
    #include <string.h>
    #include <stdlib.h>
    #include <stddef.h>

    void insertion_sort_x(void *, void *, int, int, size_t);
    void heap_sort_x(void *, void *, int, int, size_t);
    void quick_sort_x(void *, void *, int, int, size_t);

    #define insertion_sort(ptr, tmp, cnt, type, key)    insertion_sort_x((ptr), (tmp), (cnt), sizeof(type), offsetof(type, key))
    #define heap_sort(ptr, tmp, cnt, type, key)         heap_sort_x((ptr), (tmp), (cnt), sizeof(type), offsetof(type, key))
    #define quick_sort(ptr, tmp, cnt, type, key)        quick_sort_x((ptr), (tmp), (cnt), sizeof(type), offsetof(type, key))
#endif
