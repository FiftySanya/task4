#include <stdio.h>
#include <stdlib.h>

int main() {
    int n = 10;
    void *ptr = NULL;
    
    /*
    printf("Проблемний код:\n");
    int i = 0;
    while (i < 3) {
        if (!ptr)
            ptr = malloc(n);
        
        printf("Ітерація %d: ptr = %p\n", i, ptr);
        
        free(ptr);
        // ptr залишається не NULL, але вказує на звільнену пам'ять
        i++;
    }
    */
    
    int i = 0;
    ptr = NULL;
    
    while (i < 3) {
        if (!ptr)
            ptr = malloc(n);
        
        printf("Ітерація %d: ptr = %p\n", i, ptr);
        
        free(ptr);
        ptr = NULL; // Встановлюємо ptr в NULL після звільнення
        i++;
    }
    
    return 0;
}