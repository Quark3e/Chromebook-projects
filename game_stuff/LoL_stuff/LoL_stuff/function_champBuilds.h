#pragma once


void printBuild(string championName, string BuildsName) {
    string NameBase = "champion:";
    string compareName = NameBase + championName;
    cout << "----------" << compareName << ":" << BuildsName << " build:" << "\n" << endl;
    string s;
    ifstream myfile("Builds.txt");
    if (myfile.is_open()) {
        while (true) {
            getline(myfile, s);
            if (s == compareName + "\r")
                break;
            if (myfile.eof()) { cout << "\nMispronounced champion name\n"; return; }
        }
        while (true) {
            getline(myfile, s);
            if (s == BuildsName + "\r")
                break;
            if (myfile.eof()) { cout << "\nMispronounced build name\n"; return; }
        }
        while (true) {
            getline(myfile, s);
            if (s == ":\r")
                break;
            if (s == "Items:")
                cout << endl;
            cout << s << "\n";
        }
    }
    else { cout << "Unable to open file\n"; }
    myfile.close();

}


void displayEveryBuild(string champion) {
    string NameBase = "champion:";
    string compareName = NameBase + champion + "\r";
    cout << "----------" << compareName << " builds" << endl;
    string s;
    ifstream myfile("Builds.txt");
    if (myfile.is_open()) {
        while (true) {
            getline(myfile, s);
            if (s == compareName)
                break;
            if (myfile.eof()) { cout << "\nMispronounced champion name\n"; return; }
        }
        while (true) {
            getline(myfile, s);
            if (s == ";")
                break;
            else if (s == ":")
                cout << endl << endl;
            else if (s == "Items:")
                cout << endl;
            else
                cout << s << "\n";
        }
    }
    else { cout << "Unable to open file\n"; }
    myfile.close();
}


void displayBuildNames(string championName) {
    cout << championName << " builds:" << endl;
    string s;
    string findChamp;
    ifstream myfile("Builds.txt");
    int n = 0;
    if (myfile.is_open()) {
        while (true) {
            // cout << "iteration:" << n << endl;
            getline(myfile, findChamp);
            // cout << findChamp << endl;
            if (findChamp == ("champion:" + championName + "\r")) {
                // cout << "FOUND IT\n";
                //cout << "found champ\n";
                break;
            }
            // if (n == 415) {
            //     cout << "champion:" + championName << endl;
            // }
            else if (myfile.eof()) {
                cout << "Mispronounced champion name, try again" << endl;
                
                return;
            }
            n++;
        }
        getline(myfile, s);
        cout << "-" << s << "\n";
        while (true) {
            getline(myfile, s);
            //cout << s << endl;
            //cout << n++ << endl;
            if (s == ":\r") {
                getline(myfile, s);
                if (s == ";\r") {
                    //cout << "read on \";\"\n";
                    break;
                }
                cout << "-" << s << "\n";
            }
            else if (myfile.eof()) {
                cout << "warning. .eof() reached the end\n"; break;
            }
        }
    }
    else { cout << "Unable to open file\n"; }
    myfile.close();
}


void selectBuild(string championName, bool showEveryBuild) {
    bool loopVal = true;
    int options;

    if (!showEveryBuild) {
        while (loopVal) {
    

            displayBuildNames(championName);

            cout << "Enter build name: ";
            cin >> chosenBuild;
            if (chosenBuild == "exit" || chosenBuild == "back")
                return;
            printBuild(championName, chosenBuild);

            cout << endl;
    
            cout << "\n \nOptions:\n1.Get another build\n2.Pick Another Champion\n3.Exit\n";
            cin >> options;
            switch (options) {
            case 1:
                break;
            case 2:
                return;
            case 3:
                exitVal = true;
                cout << endl;
                return;
            default:
                cout << "\nwrong answer\n";
                exitVal = true;
                return;
            }
        }
    }
    else if (showEveryBuild) {
        for (int i = 0; i < *(&availableChamps + 1) - availableChamps; i++) {
            if (championName == availableChamps[i]) {
                displayEveryBuild(championName);
                return;
            }
        }
        cout << "Incorrect input." << endl;
    }
}

void listEveryBuild() {
    string s;
    ifstream myfile("Builds.txt");
    if (myfile.is_open()) {
        while (true) {
            getline(myfile, s);
            if (s.substr(0, 9) == "champion:") {
                s.erase(0, 9);
                cout << s << endl;
                getline(myfile, s);
                cout << "- " << s << endl;
            }
            else if (s == ":") {
                getline(myfile, s);
                if (s == ";")
                    cout << endl;
                else {
                    cout << "- " << s << endl;
                }
            }
            if (myfile.eof())
                break;
        }
    }
    else { cout << "Unable to open file\n"; }
    myfile.close();
}

void everyBuild() {
    string s;
    ifstream myfile("Builds.txt");
    if (myfile.is_open()) {
        while (true) {
            getline(myfile, s);
            if (s == ";")
                cout << "------------------------------" << endl;
            else if (s == ":")
                cout << endl;
            else
                cout << s << "\n";
            if (myfile.eof())
                break;
        }
    }
    else { cout << "Unable to open file\n"; }
    myfile.close();
}