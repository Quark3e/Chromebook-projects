#include <iostream>
#include <string>
#include <vector>
using namespace std;

vector<int> findVectorIndex(vector<vector<string>> vecvec, string toFind, bool printChecks=false);

int main() {
	vector<vector<string>> testVec;
    string name="-h", name2="--help";
    testVec.push_back(std::vector<std::string>(2, ""));
    testVec[testVec.size()-1][0] = name;
    testVec[testVec.size()-1][1] = name2;
    
    vector<int> result=findVectorIndex(testVec,"--help",true);

	cout <<result[0]<<":"<<result[1]<<endl;
 	return 0;
}

vector<int> findVectorIndex(vector<vector<string>> vecvec, string toFind, bool printChecks) {
    if(printChecks) {
        cout <<"\"" <<toFind << "\": "<< vecvec.size()<< ": {\n";
        for(vector<string> vecStr: vecvec) {
            cout << "\t{ ";
            for(string strIIng: vecStr) cout << strIIng << " ";
            cout<<"}" << endl;
        }
        cout << "}"<<endl;
    }

    if(vecvec.size()==0 || vecvec[0].size()==0) return vector<int>(2, -1);

    vector<int> indices(2, -1);
    for(size_t i=0; i<vecvec.size(); i++) {
        for(size_t n=0; n<vecvec[i].size(); n++) {
            if(vecvec[i][n]==toFind) {
                indices[0] = i;
                indices[1] = n;
                return indices;
            }
        }
    }

    return indices;
}
