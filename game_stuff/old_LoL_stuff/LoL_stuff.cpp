
#include <iostream>
#include <string>
#include <math.h>
#include <time.h>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

#include "cdr_calculator.h"
#include "champ_builds.h"
#include "item_combinator.h"


int main(int argc, char* argv[])
{
	while (true) {
		string input;
		int opt;
		cout << "1. Champion ability cooldown calculator\n";
		cout << "2. List of champion builds\n";
		cout << "3. Item stat combinator\n";
		cout << "enter choice: ";
		cin >> input;
		if (input == "exit")
			return 0;
		opt = stoi(input);
		cin.clear();
		cin.ignore();

		switch (opt) {
		case 1:
			cdr_calculator();
			break;
		case 2:
			champBuilds();
			break;
		case 3:
			itemCombinator();
			break;
		default:
			cout << "\"" << opt << "\" is not a valid choice.\nTry again\n";
		}
	}

	return 0;
}

