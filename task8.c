#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define NUM_ALLOCS 100000
#define MAX_SIZE 1024

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main() {
    void* ptrs[NUM_ALLOCS];
    double start, end;
    
    start = get_time();
    
    for (int i = 0; i < NUM_ALLOCS; i++) {
        size_t size = rand() % MAX_SIZE + 1;
        ptrs[i] = malloc(size);
        if (!ptrs[i]) {
            printf("Помилка виділення пам'яті\n");
            exit(1);
        }
    }
    
    for (int i = 0; i < NUM_ALLOCS; i++) {
        free(ptrs[i]);
    }
    
    end = get_time();
    printf("Час виконання: %.6f секунд\n", end - start);
    
    return 0;
}