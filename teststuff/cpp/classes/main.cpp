
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
    ex1.path = strcat(strcpy(path, pathP), "/");
    ex1.desc = "\"example 1\"";
}


int main() {
    initPaths();

    cout << ex1.path << ": " << ex1.desc << endl;


    return 0;
}