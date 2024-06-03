
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

int main(int argc, char** argv) {
    for (int ii=0; ii<10; ++ii ) {
        int input;
        char receiv[255], toSend[255];
        std::string output;
        // std::cin >> input;
        // scanf("%d", &input);

        scanf("%s", &receiv); 

        usleep(1'000'000);
        /*
            unless width is specified in format-string, newline char
            must be added to print string in python script
        */

        // std::cout << std::strlen(receiv) << std::endl;
        
        // std::string temp(ii, 'P');
        // if(input%2==0)
        //     printf("%s\n", (std::to_string(input*ii)+"_CPP"+temp).c_str());
        // else
        //     printf("%s\n", (std::to_string(input*ii)).c_str());


        printf("%c%d", receiv[0], argc);
        for(int i=1; i<argc; i++) {
            if(i>1) printf(", ");
            // for(int n=0; n<strlen(argv[i]); n++) {
                
            // }
            printf("\"%s\"", argv[i]);
        }
        printf("%c", receiv[strlen(receiv)-1]);
        // for(int i=0; i<strlen(receiv); i++) {
            
        // }


        printf(" %s\n", receiv);

        std::cout.flush();
    }
}