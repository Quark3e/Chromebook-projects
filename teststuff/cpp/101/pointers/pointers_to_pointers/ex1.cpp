
#include <iostream>


int main(int argc, char** argv) {
    int i0 = 0;
    int i1 = 1;
    int i2 = 2;

    int *arr[] = {&i0, &i1, &i2};

    std::cout << std::endl;
    for(int i=0; i<3; i++) {
        std::cout << " i: "<< i << ": " << arr[i] << " -> " << *arr[i] << std::endl;
    }

    return 0;
}
