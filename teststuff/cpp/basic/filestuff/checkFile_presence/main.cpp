
#include <iostream>
#include <sys/stat.h>
#include <string>
using namespace std;

int main(int argc, char **argv) {
    string temp;

    if(argc>1) {
        temp = argv[1];
    }
    else {
        cout << "ERROR: missing file argument. Exiting" << endl;
        return 1;
    }

    struct stat sb;

    if(stat(temp.c_str(), &sb) == 0 && !(sb.st_mode & S_IFDIR)) {
        cout << "The Path is valid!";
    }
    else {
        cout << "The Path is invalid!";
    }
    cout << endl;

    return 0;
}
