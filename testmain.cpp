#include<iostream>
#include"BooleanFormulaeGenetator.h"
using namespace std;

int main() {
	int maxOperator = 0;
	string fileName, input;
	cout << "Enter the parsed file name please: ";
	cin >> fileName; //project636Test

	list<string> *int_BooleanOperatorsListI = new list<string>();
	list<string> *int_arithmeticOperatorsListI = new list<string>();

	cout << "Enter the int_Boolean operators List: \n";

	cin >> input;
	while (input != "done") {
		int_BooleanOperatorsListI->push_back(input);
		cin >> input;
	}
	cout << "Enter the int_arithmetic operators List: \n";

	cin >> input;
	while (input != "done") {
		int_arithmeticOperatorsListI->push_back(input);
		cin >> input;
	}

	cout << "\nEnter the maximum number of operators allowes please: ";
	cin >> maxOperator;
	cout << "------------------------------------------------------------------------------------\n";

	BooleanFormulaeGenetator *testClass = new BooleanFormulaeGenetator(fileName, int_BooleanOperatorsListI, int_arithmeticOperatorsListI);

	string *printer = new string[20];
	for (int i = 0; i < 20; i++)printer[i] = " ";

	for (int i = 1; i <= maxOperator; i++) {

		testClass->generateFormulae(i, false, true, printer, 0);// operator counter // boolean not taken // must take component
	}

	system("PAUSE");
	return 0;
}