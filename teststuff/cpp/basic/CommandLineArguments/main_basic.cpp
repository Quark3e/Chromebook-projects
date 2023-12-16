
#include <stdio.h>

int main(int argc, char* argv[]) {
    printf("You have entered %d number of arguments:\n", argc);

    for(int i=0; i<argc; i++) {
        printf("%s\n", argv[i]);
    }
    return 0;
}
