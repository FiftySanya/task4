#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    size_t initial_size = 10;
    char* buffer = (char*)malloc(initial_size);
    
    if (buffer == NULL) {
        printf("Помилка виділення початкової пам'яті\n");
        return 1;
    }
    
    strcpy(buffer, "Дані");
    printf("Початкові дані: %s\n", buffer);

    size_t huge_size = (size_t) - 1;
    char* new_buffer = (char*)realloc(buffer, huge_size);
    
    if (new_buffer == NULL) {
        printf("realloc() не зміг виділити пам'ять\n");
        printf("Оригінальні дані збережені: %s\n", buffer);
        
        free(buffer);
    }
    else {
        buffer = new_buffer;
        printf("Успішне перевиділення пам'яті\n");
        free(buffer);
    }
    
    return 0;
}