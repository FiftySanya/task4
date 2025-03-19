#include <stdio.h>
#include <stdlib.h>

int main() {
    void* ptr = malloc(0);
    
    if (ptr != NULL) {
        printf("malloc(0) повернув вказівник: %p\n", ptr);
        printf("Цей вказівник безпечно передавати в free()\n");
        free(ptr);
    } 
    else {
        printf("malloc(0) повернув NULL\n");
    }
    
    return 0;
}