# libsort

A simple insertion, heap, and quick sort implementation in C with MIT License.

## Caveat
 * Only sort an array of structures.
 * Only sort by (int) in structures.

## Implementation
 * Each sorting function is implemented as a single function using only memcpy for swapping.
 * Each sorting function does not require external functions for swapping or comparing data.
 * Each sorting function uses the exact same interface.

## Quick Sort
 * Uses insertion sort for N < 32 to optimize overhead.
 * Uses heap sort when quick sort performance degrades.
 * Uses an explicit stack that is 2 * lg(N).

## Example
```
struct node {
    int key;
};

int main(int argc, char * argv[]) {
    struct node list[30];
    struct node temp;       /* temporary storage for swapping */

    heap_sort(list, &temp, 30, struct node, key);
    quick_sort(list, &temp, 30, struct node, key);

    return 0;
}


```
