
#include <iostream>
#include <string>
#include <cstring>

#include <unistd.h>
#include <libgen.h>
#include <linux/limits.h>

using namespace std;


class dirPath {
    public:
        string path;
        string desc;
};

dirPath ex1;

void initPaths() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    const char *pathP;
    if (count != -1) {
        pathP = dirname(result);
    }

    char path[100];
    string temp = strcat(strcpy(path, pathP), "/");
    ex1.path = temp.substr(0, temp.find("cpp"))+"python/opencv/angleArea/data/csv_artif/";
    // ex1.path = 
    ex1.desc = "\"example 1\"";
}


int main() {
    initPaths();

    int n=69;
    cout << to_string(n) << endl << endl;
    int temp[10];
    temp[4] = 69;
    for(int i=0; i<10; i++) {
        cout << "- " << temp[i] << endl;
    }


    cout << ex1.path << ": " << ex1.desc << endl;


    return 0;
}