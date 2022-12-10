#pragma once


#include "function_cdr.h"

void option1();
void option2();
void option3();

void cdr_calculator() {
    int options;
    string input;

    while (true) {
        cout << "Options:\n1.Get ult CD stats\n2.Get ult CD with AbilityHaste\n3.Manually get cooldown from cd and Ability Haste\n4.Exit\n";
        cout << "Enter choice: ";
        cin >> input;
        if (input == "exit") { break; }
        options = stod(input);
        cin.clear();
        cin.ignore();
        if (cin.fail()) {
            cout << "error: wrong input.";
            cin.clear();
            cin.ignore();
        }

        switch (options) {
        case 1:
            option1();
            break;
        case 2:
            option2();
            break;
        case 3:
            option3();
            break;
        case 4:
            return;
        default:
            cout << "invalid option\n";
            if (cin.fail()) {
                cin.clear();
                cin.ignore();
            }
        }
        cout << "\n-------------\n";
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
        }
    }
    return;
}


void option1() {
    string Champion;
    cout << "Enter champion name: ";
    cin >> Champion;
    if (cin.fail()) {
        cin.clear();
        cin.ignore();
    }
    cout << txtFileFinder(ChampionRow_finder(Champion));
}

void option2() {
    string Champion;
    int level;
    int AbilityHaste;
    cout << "Enter champion name: ";
    cin >> Champion;
    cout << "Enter champion's level: ";
    cin >> level;
    cout << "Enter Ability Haste: ";
    cin >> AbilityHaste;
    if (cin.fail()) {
        cout << "error: wrong input.";
        cin.clear();
        cin.ignore();
    }
    double answer;
    double cooldown = AbilityStat(ChampionRow_finder(Champion), level);
    if (cooldown != 0) {
        answer = round(((1 - CD_reduction(AbilityHaste)) * cooldown), 1);
        cout << Champion << "'s CD with " << AbilityHaste << "Ability Haste is:\n";
        cout << answer << " seconds";
    }
}

void option3() {
    double cooldown;
    double AbilityHaste;

    cout << "Enter cooldown and Ability Haste: ";
    cin >> cooldown >> AbilityHaste;
    cout << cooldown << "s cooldown with " << AbilityHaste << "Ability Haste=\n";
    cout << round(((1 - CD_reduction(AbilityHaste)) * cooldown), 1);
}
