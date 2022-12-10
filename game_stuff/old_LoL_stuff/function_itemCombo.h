#pragma once

void itemPrefix_specifier(string abilityspecifier) {
    if (abilityspecifier == "AttackDamage") { itemPrefix = "AD"; }
    else if (abilityspecifier == "AbilityPower") { itemPrefix = "AP"; }
    else if (abilityspecifier == "CriticalStrike") { itemPrefix = "%. crit"; }
    else if (abilityspecifier == "AttackSpeed") { itemPrefix = "%. atk"; }
    else if (abilityspecifier == "Lethality") { itemPrefix = "Lethalilty"; }
    else if (abilityspecifier == "ArmorPenetration") { itemPrefix = "%. Armor pen"; }
    else if (abilityspecifier == "Omnivamp") { itemPrefix = "Omnivamp"; }
    else if (abilityspecifier == "LifeSteal") { itemPrefix = "LifeSteal"; }
    else if (abilityspecifier == "AbilityHaste") { itemPrefix = "AH"; }
    else if (abilityspecifier == "Health") { itemPrefix = "Health"; }
    else if (abilityspecifier == "Mana") { itemPrefix = "Mana"; }
    else if (abilityspecifier == "Armor") { itemPrefix = "Armor"; }
    else if (abilityspecifier == "MagicResistance") { itemPrefix = "MR"; }
    else if (abilityspecifier == "HealthRegen") { itemPrefix = "%. Health regen."; }
    else if (abilityspecifier == "ManaRegen") { itemPrefix = "%. Mana regen."; }
    else if (abilityspecifier == "ApPenetration") { itemPrefix = "%. AP pen"; }
    else if (abilityspecifier == "MovementSpeed") { itemPrefix = "ms"; }
}

int itemRow_finder(string itemName) {
    int rowLine = 1;
    bool loopVal = true;
    string s;
    string delimiter = "/";
    size_t pos = 0;
    ifstream myfile("itemList.txt");
    if (myfile.is_open()) {
        while (loopVal) {
            getline(myfile, s);
            pos = s.find(delimiter);
            if (itemName == s.substr(0, pos)) { break; }
            s.erase(0, pos + delimiter.length());
            rowLine = rowLine + 1;
            if (myfile.eof()) { rowLine = 0; break; }
        }
    }
    else { cout << "Unable to open file\n"; }
    return rowLine;
}


string getItemName_fromrowLine(int rowLine) {
    string itemName;
    string s;
    string delimiter = "/";
    size_t pos = 0;
    ifstream myfile("itemList.txt");
    if (myfile.is_open()) {
        for (int i = 0; i <= rowLine; i++) {
            getline(myfile, s);
            pos = s.find(delimiter);
            itemName = s.substr(0, pos);
            s.erase(0, pos + delimiter.length());
        }
    }
    else { cout << "Unable to open file\n"; }
    myfile.close();

    return itemName;
}

string stringFinder(int rowLine) {
    string line;
    string failed_returnVal;
    if (rowLine == 0) { failed_returnVal = "69"; return failed_returnVal; }
    ifstream myfile("itemList.txt");
    if (myfile.is_open()) {
        for (int i = 1; i <= rowLine; i++) {
            getline(myfile, line);
        }
        myfile.close();
    }
    return line;
}

int abilityType_StringToInt(string abilityType) {
    int abilityTypeVar = 0;
    int false_check = 1;

    itemPrefix_specifier(abilityType);

    if (abilityType == "AttackDamage") {
        abilityTypeVar = 2;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "AbilityPower") {
        abilityTypeVar = 3;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "CriticalStrike") {
        abilityTypeVar = 4;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "AttackSpeed") {
        abilityTypeVar = 5;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "Lethality") {
        abilityTypeVar = 6;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "ArmorPenetration") {
        abilityTypeVar = 7;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "Omnivamp") {
        abilityTypeVar = 8;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "LifeSteal") {
        abilityTypeVar = 9;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "AbilityHaste") {
        abilityTypeVar = 10;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "Health") {
        abilityTypeVar = 11;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "Mana") {
        abilityTypeVar = 12;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "Armor") {
        abilityTypeVar = 13;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "MagicResistance") {
        abilityTypeVar = 14;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "HealthRegen") {
        abilityTypeVar = 15;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "ManaRegen") {
        abilityTypeVar = 16;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "ApPenetration") {
        abilityTypeVar = 17;
        false_check = 0;
        return abilityTypeVar;
    }
    else if (abilityType == "MovementSpeed") {
        abilityTypeVar = 18;
        false_check = 0;
        return abilityTypeVar;
    }
    if (false_check = 1) { abilityTypeVar = 69; return abilityTypeVar; }

    return abilityTypeVar;
}

int StatCalculator(string statString) {
    int statVal;
    string baseStat;
    string addSign = "+";
    size_t pos = 0;
    size_t stringSign = statString.find('+');
    if (stringSign != string::npos) {
        pos = statString.find(addSign);
        baseStat = statString.substr(0, pos);
        statString.erase(0, pos + addSign.length());
        cout << "checkpoint if: ";
        cout << baseStat;
        cout << " " << statString;
        statVal = stoi(baseStat) + stoi(statString);
        cout << " :end\n";
    }
    else {
        cout << "checkpoint else: ";
        cout << statString;
        statVal = stoi(statString);
        cout << " :end\n";
        return statVal;
    }
    
    return statVal;
}

int itemStat(int rowLine, string abilityType) {
    int itemStat;
    int return_failed;
    string s = stringFinder(rowLine);
    int ItemStatType = abilityType_StringToInt(abilityType);
    if (s == "69") { cout << "bastard misspelled the item name: " << rowLine << endl; return_failed = 69; return return_failed; }
    if (ItemStatType == 69) { cout << "ya misspelled the item stat fucko. Start again"; return_failed = 69; return return_failed; }
    int loopCount = 0;
    string delimiter = "/";
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        pos = s.find(delimiter);
        token = s.substr(0, pos);
        if (pos > string::npos) { break; }
        s.erase(0, pos + delimiter.length());
        loopCount = loopCount + 1;
        if (loopCount >= ItemStatType) { break; }
        //cout << token << endl;
    }
    // cout << "\n" << token << endl;
    if (ItemStatType > 1) {
        itemStat = StatCalculator(token);
    }
    else { itemStat = 0; }
    itemStatusNumber = itemStat;
    //cout << itemStatusNumber;
    return itemStat;
}

void displayEveryItemWithTheStat(string itemType, string abilityType) {
    int displayVal;
    string displayItemName;

    itemPrefix_specifier(abilityType);
    cout << "List of items with " << abilityType << ": " << itemPrefix << ":" << endl;
    if (itemType == "Mythic") {
        for (int mythicCount = 2; mythicCount <= 24; mythicCount++) {
            displayVal = itemStat(mythicCount, abilityType);
            displayItemName = getItemName_fromrowLine(mythicCount - 1);
            if (displayVal != 0) {
                cout << "\n- " << displayItemName << ": " << displayVal;
            }
            else {}
        }
    }
    else if (itemType == "Legendary") {
        for (int legenCount = 27; legenCount <= 90; legenCount++) {
            displayVal = itemStat(legenCount, abilityType);
            displayItemName = getItemName_fromrowLine(legenCount - 1);
            if (displayVal != 0) {
                cout << "\n- " << displayItemName << ": " << displayVal;
            }
            else {}
        }
    }
    else {
        cout << "\nWell shit you messed up the name but the if/if-else's didn't detect it";
        for (int i = 0; i < 3; i++) {
            cout << ". ";
        }
        cout << "shi";
        for (int shiCount = 0; shiCount < 50; shiCount++) {
            cout << "i";
        }
        cout << endl;
        return;
    }
    cout << endl;
}

void highestStat_Finder(string abilityType, string itemType, string ignoreItemOnce) {
    int var1;
    int var2;
    int i = 2;
    int arrayCount = 0;
    arrayWhatToShow = 0;
    string currentStatus = "notEqual";
    int ignoreItemName = itemRow_finder(ignoreItemOnce);
    int specialItemIgnore = 0;
    int ArrSize = *(&ignoreItemArray + 1) - ignoreItemArray;
    //cout << "--" << ignoreItemOnce << ignoreItemName << endl;

    if (itemType == "Mythic") {
        var1 = itemStat(i, abilityType);
        if (i == ignoreItemName) { var1 = 0; }
        for (int n = 0; n <= (ArrSize - 1); n++) {
            specialItemIgnore = itemRow_finder(ignoreItemArray[n]);
            if (i == specialItemIgnore) { var1 = 0; }
        }
        highestStatItemName_Array[0] = getItemName_fromrowLine(i - 1);
        for (i = 3; i <= 24; i++) {
            var2 = itemStat(i, abilityType);
            if (i == ignoreItemName) { var2 = 0; }
            for (int n = 0; n <= (ArrSize - 1); n++) {
                specialItemIgnore = itemRow_finder(ignoreItemArray[n]);
                if (i == specialItemIgnore) { var2 = 0; }
            }
            if (var1 > var2) {
                //represents previous line
                generalValueforOption3 = var1;
            }
            else if (var1 < var2) {
                //represents current/new line
                highestStatItemName_Array[0] = getItemName_fromrowLine(i - 1);
                arrayWhatToShow = 0;
                arrayCount = 0;
                currentStatus = "notEqual";
                generalValueforOption3 = var2;
                var1 = var2;
            }
            else if (var1 == var2) {
                if (currentStatus == "Equal") {
                    arrayWhatToShow++;
                    arrayCount++;
                    highestStatItemName_Array[arrayCount] = getItemName_fromrowLine(i - 1);
                }
                else if (currentStatus == "notEqual") {
                    currentStatus = "Equal";
                    arrayWhatToShow++;
                    arrayCount++;
                    highestStatItemName_Array[arrayCount] = getItemName_fromrowLine(i - 1);
                }
            }
        }
    }
    else if (itemType == "Legendary") {
        var1 = itemStat(i, abilityType);
        if (i == ignoreItemName) { var1 = 0; }
        for (int n = 0; n <= (ArrSize - 1); n++) {
            specialItemIgnore = itemRow_finder(ignoreItemArray[n]);
            if (i == specialItemIgnore) { var1 = 0; }
        }
        //cout << "--- "<< getItemName_fromrowLine(ignoreItemName) << ignoreItemName << endl;
        highestStatItemName_Array[0] = getItemName_fromrowLine(i - 1);
        for (i = 27; i <= 90; i++) {
            var2 = itemStat(i, abilityType);
            if (i == ignoreItemName) { var2 = 0; }
            for (int n = 0; n <= (ArrSize - 1); n++) {
                specialItemIgnore = itemRow_finder(ignoreItemArray[n]);
                if (i == specialItemIgnore) { var2 = 0; }
            }
            //string function_result = Comparer_bigger(var1, var2);
            if (var1 > var2) {
                //represents previous line
                generalValueforOption3 = var1;
            }
            else if (var1 < var2) {
                //represents current/new line
                highestStatItemName_Array[0] = getItemName_fromrowLine(i - 1);
                arrayWhatToShow = 0;
                arrayCount = 0;
                currentStatus = "notEqual";
                generalValueforOption3 = var2;
                var1 = var2;
            }
            else if (var1 == var2) {
                if (currentStatus == "Equal") {
                    arrayWhatToShow++;
                    arrayCount++;
                    highestStatItemName_Array[arrayCount] = getItemName_fromrowLine(i - 1);
                }
                else if (currentStatus == "notEqual") {
                    currentStatus = "Equal";
                    arrayWhatToShow++;
                    arrayCount++;
                    highestStatItemName_Array[arrayCount] = getItemName_fromrowLine(i - 1);
                }
            }
        }
    }
}


int itemComboCrafter(string abilityType, string ignoreItem) {
    //Returns a rowLine for a random item depending on the itemType (either mythic or legendary)
    int rowLine;
    int n = 0;
    string iterationLine;
    comboVal = 0;

    highestStat_Finder(abilityType, "Mythic", ignoreItem);
    highestItemStatCombo_Array[0] = highestStatItemName_Array[0];
    // cout << highestItemStatCombo_Array[0] << endl;

    for (int i = 1; i <= 4; i++) {
        highestStat_Finder(abilityType, "Legendary", ignoreItem);
        highestItemStatCombo_Array[i] = highestStatItemName_Array[0];
        // cout << highestItemStatCombo_Array[i] << endl;
        // ignoreItem = highestItemStatCombo_Array[i];
        ignoreItemArray[i - 1] = highestItemStatCombo_Array[i];
        // i = i + n;
        // cout << n << endl;
        // out << "&" << ignoreItem << endl;
    }
    for (n = 0; n <= 4; n++) {
        iterationLine = highestItemStatCombo_Array[n];
        comboVal = comboVal + itemStat(itemRow_finder(iterationLine), abilityType);
    }
    return 0;
}


int totalItemStatCalculator() {
    int totalVal = 0;
    for (int i = 0; i <= 4; i++) {
        totalVal = totalVal + itemsStatArray[i];
    }
    return totalVal;
}

void bastardMisspelled() {
    cout << "trade offer: You misspell one item. I get to make you redo it.\n";
    cout << "redo it\n";
}


int itemCombo(string item1_mythic, string item2_legen, string item3_legen, string item4_legen, string item5_legen, string abilityType) {
    //build item stat sum of 5 items (1 mythic, 4 legendary)
    int answer;
    int failed_returnVal = 69;
    itemStatusNumber = 0;
    itemStat(itemRow_finder(item1_mythic), abilityType);
    if (itemStatusNumber == 69) { bastardMisspelled(); return failed_returnVal; }
    itemsStatArray[0] = itemStatusNumber;
    itemsNameArray[0] = item1_mythic;
    itemStat(itemRow_finder(item2_legen), abilityType);
    if (itemStatusNumber == 69) { bastardMisspelled(); return failed_returnVal; }
    itemsStatArray[1] = itemStatusNumber;
    itemsNameArray[1] = item2_legen;
    itemStat(itemRow_finder(item3_legen), abilityType);
    if (itemStatusNumber == 69) { bastardMisspelled(); return failed_returnVal; }
    itemsStatArray[2] = itemStatusNumber;
    itemsNameArray[2] = item3_legen;
    itemStat(itemRow_finder(item4_legen), abilityType);
    if (itemStatusNumber == 69) { bastardMisspelled(); return failed_returnVal; }
    itemsStatArray[3] = itemStatusNumber;
    itemsNameArray[3] = item4_legen;
    itemStat(itemRow_finder(item5_legen), abilityType);
    if (itemStatusNumber == 69) { bastardMisspelled(); return failed_returnVal; }
    itemsStatArray[4] = itemStatusNumber;
    itemsNameArray[4] = item5_legen;

    itemStatusNumber = 0;
    return 0;
}


//champion/ad/ap/crit/atk/leth/armorPen/omnivamp/lifesteal/abilityHaste/health/mana/armor/mr/healthRegen/manaRegen/apPen