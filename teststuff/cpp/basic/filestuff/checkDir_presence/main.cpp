
// C++ Program to test presence of file/Directory
#include <iostream>
#include <sys/stat.h>
using namespace std;
 
int main(int argc, char **argv) {
    // Path to the directory
    //const char* dir = "testDir";

    if(argc>1) {
        const char* dir = argv[1];
    }
    else {
        cout << "ERROR: empty argument. Exiting" << endl;
        return 1;
    }

    // Structure which would store the metadata
    struct stat sb;
 
    // Calls the function with path as argument
    // If the file/directory exists at the path returns 0
    // If block executes if path exists
    if (stat(argv[1], &sb) == 0)
        cout << "The path is valid!";
    else
        cout << "The Path is invalid!";
    cout << endl;
 
    return 0;
}
