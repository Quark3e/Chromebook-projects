
#include <iostream>
#include <string>
#include <cstring>

int main() {
    for (int ii=0; ii<10; ++ii ) {
        int input;
        char receiv[255];
        std::string output;
        // std::cin >> input;
        // scanf("%d", &input);

        scanf("%s", &receiv); 
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

        printf("%s\n", receiv);
        std::cout.flush();
    }
}