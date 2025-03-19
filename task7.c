#include <stdio.h>
#include <stdlib.h>

struct sbar {
    int a;
    char b[100];
};

int main() {
    // Оригінальний код
    struct sbar *ptr, *newptr;
    ptr = calloc(1000, sizeof(struct sbar));
    printf("calloc: ptr = %p\n", ptr);
    
    newptr = realloc(ptr, 500 * sizeof(struct sbar));
    printf("realloc: newptr = %p\n", newptr);
    
    free(newptr);
    
    // Код з використанням reallocarray
    struct sbar *ptr2, *newptr2;
    ptr2 = calloc(1000, sizeof(struct sbar));
    printf("calloc: ptr2 = %p\n", ptr2);
    
    newptr2 = reallocarray(ptr2, 500, sizeof(struct sbar));
    printf("reallocarray: newptr2 = %p\n", newptr2);
    
    free(newptr2);
    
    return 0;
}