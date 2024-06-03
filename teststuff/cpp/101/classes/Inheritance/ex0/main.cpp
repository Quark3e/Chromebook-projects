
#include <iostream>

using namespace std;

class parent {
    private:
    string firstname;
    string job;
    string surname;
    public:

    void setname(string first, string last) {
        firstname = first;
        surname = last;
    }
    string getname() {
        return firstname +" " + surname;
    }
};

class child : parent {
    //unresolved
};



int main(int argc, char** argv) {

    parent Phil;
    Phil.setname("Phil", "Coulson");
    cout << Phil.getname();

    return 0;
}