#pragma once


string txtFileFinder(int rowLine) {
    string line;

    ifstream myfile("UltCooldowns.txt");

    if (myfile.is_open()) {
        for (int i = 1; i <= rowLine; i++) { getline(myfile, line); } //getline starts at value 1
        myfile.close();
    }

    return line;
}


double simpleRound(double x) {
    double simpleRound = x - floor(x) < 0.5 ? floor(x) : ceil(x);
    return simpleRound;
}


double round(double x, double n) {
    double incrAnswer = pow(10, n);
    double reducAnswer = pow(10, (0 - n));
    double detailRound = (simpleRound(x * incrAnswer)) * reducAnswer;
    return detailRound;
}


int levelRounder(int i) {
    int lvl;
    int returnVal;
    if (i < 6) { lvl = i; returnVal = 0; cout << "Level too low. Ult not up yet"; }
    else if (i < 12) { lvl = 6; returnVal = 1; }
    else if (i < 18) { lvl = 12; returnVal = 2; }
    else if (i = 18) { lvl = 18; returnVal = 3; }
    else { cout << i << "invalid level (max lvl: 18)"; returnVal = 0; }
    return returnVal;
}


int AbilityStat(int rowLine, int i) {
    string s = txtFileFinder(rowLine);
    int lvl = levelRounder(i) + 1;
    int loopCount = 0;
    int returnVal;
    string delimiter = "/";
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        pos = s.find(delimiter);
        token = s.substr(0, pos);
        if (pos > string::npos) { break; }
        s.erase(0, pos + delimiter.length());
        loopCount = loopCount + 1;
        if (loopCount >= lvl) { break; }
    }
    if (lvl > 1) { returnVal = stoi(token); }
    else { returnVal = 0; }
    return returnVal;
}


int ChampionRow_finder(string Champion) {
    int loopCount = 1;
    string s;
    string delimiter = "/";
    size_t pos = 0;
    ifstream myfile("UltCooldowns.txt");
    if (myfile.is_open()) {
        while (true) {
            getline(myfile, s);
            pos = s.find(delimiter);
            if (Champion == s.substr(0, pos)) { break; }
            s.erase(0, pos + delimiter.length());
            loopCount = loopCount + 1;
            if (myfile.eof()) { loopCount = 0; break; }
            if (loopCount > 200) {
                cout << "Unable to find champion with champion name: \"" << Champion << "\"\n";
                break;
            }
        }
    }
    else { cout << "Unable to open file\n"; }
    myfile.close();
    return loopCount;
} //finds the rowline


double CD_reduction(double i) {
    double Reduction;
    Reduction = i / (i + 100);
    return Reduction;
}

