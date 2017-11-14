// #include "engine.h"
// #include "typedefs.h"

// void
// swap_bytes(char *a, char *b, u32 nbytes)
// {
//     char tmp;
//     do
//     {
//         tmp = *a;
//         *a++ = *b;
//         *b++ = tmp;
//     } while (--nbytes);
// }

// void
// insert_sort(void *a,
//             u32 len,
//             u32 size,
//             int (*cmp_lt)(void *a, void *b, void *context),
//             void *context)
// {
//     char *i, *j;
//     char *end = (char *)a + size * len;
//     for (i = (char *)a + size; i < end; i += size)
//     {
//         for (j = i; j > (char *)a && cmp_lt(j, j - size, context); j -= size)
//         {
//             swap_bytes(j, j - size, size);
//         }
//     }
// }

// void
// comb_sort(void *a,
//           u32 len,
//           u32 size,
//           int (*cmp_lt)(void *a, void *b, void *context),
//           void *context)
// {
//     static const double shrink_factor = 1.2473309501039786540366528676643;

//     char *i, *j;
//     int swap;
//     u32 gap = len;

//     do
//     {
//         if (gap > 2)
//         {
//             gap = (u32)gap / shrink_factor;
//             if (gap == 9 || gap == 10)
//             {
//                 gap = 11;
//             }
//         }

//         swap = 0;
//         for (i = a; i < (char *)a + (len - gap) * size; i += size)
//         {
//             j = i + (gap * size);
//             if (cmp_lt(j, i, context))
//             {
//                 swap_bytes(i, j, size);
//                 swap = 1;
//             }
//         }
//     } while (swap || gap > 2);

//     if (gap != 1)
//     {
//         insert_sort(a, len, size, cmp_lt, context);
//     }
// }

// struct intro_sort_stack
// {
//     void *left, *right;
//     int depth;
// };

// static inline void *
// intro_sort__select_pivot(void *val1,
//                          void *val2,
//                          void *val3,
//                          int (*cmp_lt)(void *a, void *b, void *context),
//                          void *context)
// {
//     return (cmp_lt(val1, val2, context)
//                 ? (cmp_lt(val2, val3, context)
//                        ? val2
//                        : (cmp_lt(val1, val3, context) ? val3 : val1))
//                 : (cmp_lt(val1, val3, context)
//                        ? val1
//                        : (cmp_lt(val2, val3, context) ? val3 : val2)));
// }

// static inline void
// intro_sort__partition(char *left,
//                       u32 len,
//                       u32 size,
//                       int (*cmp_lt)(void *a, void *b, void *context),
//                       void *context,
//                       u32 max_depth)
// {
//     char *right, *pivot;

//     if (--max_depth == 0)
//     {
//         // Worst case scenario - switch to comb sort
//         comb_sort(left, len, size, cmp_lt, context);
//         return;
//     }

//     right = left + (len - 1) * size;

//     pivot = intro_sort__select_pivot(left,
//                                      right,
//                                      left + ((right - left) >> 1) * size,
//                                      cmp_lt,
//                                      context);

//     // use stack instead of recursion?
//     for (;;)
//     {
//         do
//         {
//             left += size;
//         }
//     }
// }

// void
// intro_sort(void *a,
//            u32 len,
//            u32 size,
//            int (*cmp_lt)(void *a, void *b, void *context),
//            void *context)
// {
//     int depth;
//     struct intro_sort_stack *top;

//     // Iterators
//     char *i, *j, *k, *l, *m;

//     // Trivial optimizations
//     if (len <= 1)
//     {
//         return;
//     }
//     // TODO: Sorting network optimization
//     else if (len == 2)
//     {
//         if (cmp_lt((char *)a + size, a, context))
//         {
//             swap_bytes((char *)a + size, a, size);
//         }
//         return;
//     }

//     depth = 2;
//     while ((1ul << depth) < len)
//     {
//         ++depth;
//     }

//     struct intro_sort_stack stack[depth + 2];
//     top = stack;

//     i = a;
//     j = (char *)a + (len - size);

//     depth <<= 1;
//     for (;;)
//     {
//         if (i < j)
//         {
//             if (--depth == 0)
//             {
//                 comb_sort(i, j - i + size, size, cmp_lt, context);
//                 j = i;
//                 continue;
//             }

//             k = i;
//             l = j;
//             m =
//         }
//     }
// }