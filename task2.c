#include <stdio.h>
#include <stdlib.h>

int main() {
    int xa = 1000000;  // 1 мільйон
    int xb = 1000000;  // 1 мільйон
    int num = xa * xb;
    void* ptr = malloc(num); // Передача num у malloc

    if (ptr == NULL) {
        printf("malloc не зміг виділити %d байтів\n", num);
    }
    else {
        printf("malloc виділив %d байтів\n", num);
        free(ptr);
    }
    return 0;
}