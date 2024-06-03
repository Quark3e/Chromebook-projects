#pragma once

string availableChamps[] = {
    "Amumu", "Anivia", "AurelianSol",
    "Galio",
    "Jarvan", "Jhin",
    "Katarina", "Kog'Maw",
    "Leona",
    "Mordekaiser",
    "Rakan",
    "Sion",
    "TahmKench", "Talon", "Tristana", "Tryndamere",
    "Volibear",
    "Warwick",
    "Ziggs"
};

string champion = "null";
string chosenBuild;

bool exitVal = false;

#include "function_champBuilds.h"


void champBuilds() {

    bool loopVal = true;
    int arrSize = *(&availableChamps + 1) - availableChamps;
    while (loopVal) {
        string input;

        cout << "Available Champion builds:" << "\n";
        for (int i = 0; i < arrSize; i++) { cout << availableChamps[i] << "\n"; }
        cout << "\n1. Get champion build\n";
        cout << "2. Display every build for champion\n";
        cout << "3. Show a namelist of every champion build\n";
        cout << "4. Display every build in the list\n";
        cout << "input: ";
        cin >> input;
        if (input == "exit")
            return;
        else if (stoi(input) == 1) {
            cout << "\nEnter champion name: ";
            cin >> champion;
            if (champion == "exit")
                return;
            selectBuild(champion, false);
        }
        else if (stoi(input) == 2) {
            cout << "\nEnter champion name: ";
            cin >> champion;
            if (champion == "exit")
                return;
            selectBuild(champion, true);
        }
        else if (stoi(input) == 3) {
            listEveryBuild();
        }
        else if (stoi(input) == 4) {
            cout << "\nLoading";
            for (int i = 0; i <= 3; i++) {
                cout << ". ";
            }

            everyBuild();
        }
        else {
            cout << "\"" << champion << "\" is an invalid input. Try again." << endl;
        }
       
        if (exitVal) { return; }
    }

}