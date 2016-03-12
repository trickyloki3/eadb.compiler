#include "libsort.h"

#define swap(tmp, buf, r, c, size)  \
    memcpy((char *) tmp, (char *) buf + r, size);     \
    memcpy((char *) buf + r, (char *) buf + c, size); \
    memcpy((char *) buf + c, (char *) tmp, size);

#define get_key(x, y, z)    (*((int *) ((char *) (x) + (y) + (z))))

/* return the median from 3 numbers */
#define mof3(buf, key, min, mid, max)                                               \
    get_key(buf, min, key) < get_key(buf, mid, key) ?                               \
    get_key(buf, mid, key) < get_key(buf, max, key) ? mid  /* min < mid < max */ :  \
    get_key(buf, min, key) < get_key(buf, max, key) ? max  /* min < max < mid */ :  \
                                                      min  /* max < min < mid */ :  \
    get_key(buf, max, key) < get_key(buf, mid, key) ? mid  /* max < mid < min */ :  \
    get_key(buf, max, key) < get_key(buf, min, key) ? max  /* mid < max < min */ :  \
                                                      min  /* mid < min < max */

/* stack */
struct part {
    int min;
    int max;
};

#define stack_push(stack, stack_size, x, y) \
    (stack)[stack_size].min = (x);          \
    (stack)[stack_size].max = (y);          \
    (stack_size)++;

#define stack_pull(stack, stack_size, x, y) \
    (x) = (stack)[(stack_size) - 1].min;    \
    (y) = (stack)[(stack_size) - 1].max;    \
    (stack_size)--;

void insertion_sort_x(void * ptr, void * tmp, int count, int size, size_t key) {
    int j;
    int i = size * 1;
    int n = size * count;

    for(; i < n; i += size) {
        memcpy(tmp, (char *) ptr + i, size);
        for(j = i - size; j >= 0 && get_key(ptr, j, key) > get_key(tmp, 0, key); j -= size)
            memcpy((char *) ptr + j + size, (char *) ptr + j, size);
        memcpy((char *) ptr + j + size, tmp, size);
    }
}

void heap_sort_x(void * ptr, void * tmp, int count, int size, size_t key) {
    int i;  /* iterator */
    int r;  /* root */
    int c;  /* child */
    int n;  /* size */

    n = size * count;

    for(i = size * ((count / 2) - 1); i >= 0; i -= size) {
        for(r = i; (c = r * 2 + size) < n; r = c) {
            if(c + size < n && get_key(ptr, c, key) < get_key(ptr, c + size, key))
                c += size;
            if(get_key(ptr, r, key) > get_key(ptr, c, key))
                break;
            swap(tmp, ptr, r, c, size);
        }
    }

    for(i = size * (count - 1); i > 0; i -= size) {
        swap(tmp, ptr, 0, i, size);
        for(r = 0; (c = r * 2 + size) < i; r = c) {
            if(c + size < i && get_key(ptr, c, key) < get_key(ptr, c + size, key))
                c += size;
            if(get_key(ptr, r, key) > get_key(ptr, c, key))
                break;
            swap(tmp, ptr, r, c, size);
        }
    }
}

void quick_sort_x(void * ptr, void * tmp, int count, int size, size_t key) {
    int length = count;
    int height = 0;
    struct part * stack;    /* stack */
    int stack_size = 0;

    int n, min, max;        /* size */
    int dif, mid;           /* ninther */
    int i, j, p, q, k;      /* sort */

    /* calculate the height */
    while(length) {
        length /= 2;
        height += 2;
    }

    /* allocate the stack */
    stack = calloc(height, sizeof(struct part));
    if(NULL == stack)
        return;

    stack_push(stack, stack_size, 0, count - 1);
    while(stack_size) {
        stack_pull(stack, stack_size, min, max);

        n = max - min + 1;

        if(n < 32) {
            /* minimize overhead of quicksort */
            insertion_sort_x((char *) ptr + (min * size), tmp, n, size, key);
        } else {
            /* calculate the ninther as pivot */
            dif = n / 8;
            mid = min + n / 2;
            mid = mof3(ptr, key,
                mof3(ptr, key, min * size, (min + dif) * size, (min + dif + dif) * size),
                mof3(ptr, key, (mid - dif) * size, mid * size, (mid + dif) * size),
                mof3(ptr, key, (max - dif - dif) * size, (max - dif) * size, max * size)
            );
            swap(tmp, ptr, mid, min * size, size);

            /* bentley-mcllroy 3-way partitioning */
            i = p = size * min;
            j = q = size * (max + 1);
            mid = get_key(ptr, i, key);
            while(1) {
                for(i += size; get_key(ptr, i, key) < mid; i += size)
                    if(i == max * size)
                        break;

                for(j -= size; mid < get_key(ptr, j, key); j -= size)
                    if(j == min * size)
                        break;

                if(i == j && get_key(ptr, i, key) == mid) {
                    p += size; /* swap is macro q.q */
                    swap(tmp, ptr, p, i, size);
                }

                if(i >= j)
                    break;

                swap(tmp, ptr, i, j, size);
                if(get_key(ptr, i, key) == mid) {
                    p += size;
                    swap(tmp, ptr, p, i, size);
                }
                if(get_key(ptr, j, key) == mid) {
                    q -= size;
                    swap(tmp, ptr, q, j, size);
                }
            }

            i = j + size;
            for(k = min * size; k <= p; k += size) {
                swap(tmp, ptr, k, j, size);
                j -= size;
            }
            for(k = max * size; k >= q; k -= size) {
                swap(tmp, ptr, k, i, size);
                i += size;
            }

            if(stack_size < height) {
                stack_push(stack, stack_size, i / size, max);
            } else {
                heap_sort_x((char *) ptr + i, tmp, max - (i / size) + 1, size, key);
            }

            if(stack_size < height) {
                stack_push(stack, stack_size, min, j / size);
            } else {
                heap_sort_x((char *) ptr + min * size, tmp, j / size - min + 1, size, key);
            }
        }
    }
    free(stack);
}
