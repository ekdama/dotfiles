#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
        int args[2];     
        int result;
    while (1) {
        result = scanf("%d %d", &args[0], &args[1]);
        printf("%d", args[0] + args[1]);
                if (result != 1) {
            break;
        }
        if (result == EOF) {
            break;
        }


    }

}