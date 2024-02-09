
#include <stdio.h>
#include <cstring>

int main(int argc, char* argv[]) {
    printf("You have entered %d number of arguments:\n", argc);

    for(int i=0; i<argc; i++) {
        if(strcmp(argv[i], "1")==0) printf("sucess | ");
        printf("%s\n", argv[i]);
    }
    return 0;
}
