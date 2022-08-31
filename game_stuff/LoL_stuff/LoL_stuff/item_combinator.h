#pragma once

int itemsStatArray[6];
string itemsNameArray[6];
int comboItemStatArray[6];
string comboItemNameArray[6];
string highestStatItemName_Array[100];
string highestItemStatCombo_Array[5];
string ignoreItemArray[10];
int comboVal = 0;
string itemPrefix;
int itemStatusNumber;
int arrayWhatToShow = 0;

int generalValueforOption3;

#include "function_itemCombo.h"

void combo_option1();
void combo_option2();
void combo_option3();
void combo_option4();
void combo_option5();


void itemCombinator() {
    string input;
    int options;

    while (true) {
        cout << "Options:\n1.See item stat from item\n";
        cout << "2.Get a sum of stat from 1 mythic and 4 legendary items\n";
        cout << "3.Get item with the highest item stat\n";
        cout << "4.Get best combination of 1 Mythic and 4 Legendary items for a certain item stat\n";
        cout << "5.Get every item with the item stat\n";
        cout << "6.Exit program\n";
        cout << "Enter option: ";
        cin >> input;
        if (input == "exit")
            return;
        options = stod(input);
        cin.clear();
        cin.ignore();

        if (cin.fail()) {
            cout << options << " :is an invalid answer.\n";
            cin.clear();
            cin.ignore();
        }
        switch (options) {
        case 1:
            combo_option1();
            break;
        case 2:
            combo_option2();
            break;
        case 3:
            combo_option3();
            break;
        case 4:
            combo_option4();
            break;
        case 5:
            combo_option5();
            break;
        case 6:
            cout << "exiting.";
            for (int i = 0; i <= 1; i++) { cout << " ."; }
            return;
        default:
            cout << options << " is an invalid answer.\n";
        }
        system("pause");
        cout << "\n-------------\n";
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
        }
    }
}


void combo_option1() {
    //get item stat from item name and abilityType
    string itemName;
    string abilityType;
    cout << "Enter item name: ";
    cin >> itemName;
    cout << "\nEnter what type of item stat you want: ";
    cin >> abilityType;
    itemStatusNumber = 0;
    itemStat(itemRow_finder(itemName), abilityType);
    itemPrefix_specifier(abilityType);
    if (itemStatusNumber == 69) { return; }
    //else {
    cout << "\n" << itemName << ": " << abilityType << " =\n";
    cout << itemStatusNumber << " " << itemPrefix << endl;
    //}

}
void combo_option2() {
    //Get a total (certain stat) value for 5 items from item names and abilityType.
    string MythicItem;
    string LegenItem1;
    string LegenItem2;
    string LegenItem3;
    string LegenItem4;
    string abilityType;
    cout << "Enter 1 Mythic and 4 Legendary items: ";
    cin >> MythicItem >> LegenItem1 >> LegenItem2 >> LegenItem3 >> LegenItem4;
    cout << "\nEnter item stat: ";
    cin >> abilityType;
    cout << "\nProcessing.";
    for (int i = 0; i < 2; i++) { cout << " ."; }
    int returnVal = itemCombo(MythicItem, LegenItem1, LegenItem2, LegenItem3, LegenItem4, abilityType);
    int totalVal = totalItemStatCalculator();
    itemPrefix_specifier(abilityType);
    if (returnVal == 69) { cout << "ah shit\n"; return; }
    else if (returnVal == 0) {
        cout << "\nTotal " << abilityType << " for:\n";
        cout << MythicItem << ", " << LegenItem1 << ", " << LegenItem2 << ", " << LegenItem3 << ", " << LegenItem4 << "\n= ";
        cout << totalVal << " " << itemPrefix << endl;
    }
    else { cout << "\nerror.\n"; }

}

void combo_option3() {
    //Find item with highest stat of a certain item stat.
    string abilityType;
    string itemType;
    generalValueforOption3 = 0;
    string ignoreItem = "none";

    cout << "\nEnter either Mythic or Legendary: ";
    cin >> itemType;
    cout << "\nEnter item stat to get highest item for: ";
    cin >> abilityType;
    cout << "\nWant to ignore any item? Y/N:\n";
    cin >> ignoreItem;
    if (ignoreItem == "Y") {
        cin.clear();
        cout << "Enter name of item to ignore: ";
        cin >> ignoreItem;
    }
    else { ignoreItem == "none"; }

    highestStat_Finder(abilityType, itemType, ignoreItem);
    //cout << arrayWhatToShow << endl;
    if (generalValueforOption3 == 0) {
        cout << "No " << itemType << " item has that item stat." << endl;
    }
    else {
        cout << "The item(s) with the highest " << abilityType;
        if (arrayWhatToShow == 0) { cout << " is:\n"; }
        else { cout << " are:\n"; }
        for (int i = 0; i <= arrayWhatToShow; i++) {
            cout << "-" << highestStatItemName_Array[i] << "\n";
        }
        itemPrefix_specifier(abilityType);
        cout << "with " << generalValueforOption3 << " " << itemPrefix << endl;
    }

}

void combo_option4() {
    //Find best combination of 1 mythic and 4 legendary to get an item stat as high as possible.
    string AbilityType;
    string ignoreItem = "none";
    ignoreItemArray[0] = "none";
    ignoreItemArray[1] = "none";
    ignoreItemArray[2] = "none";
    ignoreItemArray[3] = "none";
    ignoreItemArray[4] = "none";

    cout << "\nEnter item stat you want the best combo for: ";
    cin >> AbilityType;
    cout << "\nIs there any item you want to ignore? Y/N: ";
    cin >> ignoreItem;
    if (ignoreItem == "Y") {
        cin.clear();
        cout << "\nEnter item name to ignore: ";
        cin >> ignoreItem;
        itemComboCrafter(AbilityType, ignoreItem);

    }
    else {
        itemComboCrafter(AbilityType, ignoreItem);
    }
    cout << "\nProcessing.";
    for (int i = 0; i < 2; i++) { cout << " ."; }
    itemPrefix_specifier(AbilityType);
    cout << "\n \n";
    cout << "\nThe item combination with the highest " << AbilityType << " is:\n";
    for (int n = 0; n <= 4; n++) {
        cout << highestItemStatCombo_Array[n] << ", ";
    }
    cout << "\nWith a total value of: " << comboVal << " " << itemPrefix << endl;
}

void combo_option5() {
    //Display every item with the requested itemStat/abilityType, with the corresponding stat
    string AbilityType;
    string itemType;
    bool loopVal = true;

    while (loopVal) {
        cout << "Enter requested item stat: ";
        cin >> AbilityType;
        cout << "\nWant the Stat list for the itemtype Mythic or Legendary?:\n";
        cin >> itemType;
        if (itemType == "Mythic") { displayEveryItemWithTheStat(itemType, AbilityType); break; }
        else if (itemType == "Legendary") { displayEveryItemWithTheStat(itemType, AbilityType); break; }
        else { cout << "You mispronounced the itemType" << endl; }
    }

}
