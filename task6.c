#include <stdio.h>
#include <stdlib.h>

int main() {
    // realloc(NULL, size) - поводиться як malloc(size)
    void* ptr1 = realloc(NULL, 10);
    printf("realloc(NULL, 10) повернув: %p\n", ptr1);
    
    // realloc(ptr, 0) - поводиться як free(ptr)
    void* ptr2 = realloc(ptr1, 0);
    printf("realloc(ptr, 0) повернув: %p\n", ptr2);
    
    if (ptr2) {
        free(ptr2);
    }
    
    return 0;
}