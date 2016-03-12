#include <time.h>
#include <stdio.h>
#include "libsort.h"

#define TEST_INSERTION  0x1
#define TEST_HEAP       0x2
#define TEST_QUICK      0x4
#define TEST_ALGORITHM  (TEST_INSERTION | TEST_HEAP | TEST_QUICK)

typedef struct node {
    short id;   /* 2 */
    int data;   /* 4 */
    long ext;   /* 4 or 8 */
} * node_t;

void node_rand(node_t buffer, int length) {
    int i;

    fprintf(stderr, "Sorting %d nodes.\n", length);

    for (i = 0; i < length; i++) {
        buffer[i].data = (short) rand();
        buffer[i].id = buffer[i].data;
        buffer[i].ext = buffer[i].data;
    }
}

void node_chck(node_t buffer, int length) {
    int i;

    length--;

    for (i = 0; i < length; i++)
        if (buffer[i].data > buffer[i + 1].data ||
            buffer[i].id > buffer[i + 1].id ||
            buffer[i].ext > buffer[i + 1].ext)
            break;

    fprintf(stderr, "Sort verification %s.\n", (i != length) ? "failed" : "passed");
}

int main(int argc, char * argv[]) {
    size_t length;
    node_t buffer;
    struct node tmp;

    /* use unix time for random seed */
    srand((unsigned int) time(NULL));

    length = rand() + 1;
    buffer = calloc(length, sizeof(struct node));
    if(NULL != buffer) {
        if(TEST_ALGORITHM & TEST_INSERTION) {
            fprintf(stderr, "Insertion Sort\n");
            node_rand(buffer, length);
            insertion_sort(buffer, &tmp, length, struct node, data);
            node_chck(buffer, length);
        }

        if(TEST_ALGORITHM & TEST_HEAP) {
            fprintf(stderr, "Heap Sort\n");
            node_rand(buffer, length);
            heap_sort(buffer, &tmp, length, struct node, data);
            node_chck(buffer, length);
        }

        if(TEST_ALGORITHM & TEST_QUICK) {
            fprintf(stderr, "Quick Sort\n");
            node_rand(buffer, length);
            quick_sort(buffer, &tmp, length, struct node, data);
            node_chck(buffer, length);
        }
    }

    free(buffer);
    return 0;
}
