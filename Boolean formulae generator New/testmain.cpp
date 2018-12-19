#include<iostream>
#include"BooleanFormulaeGenetator.h"
using namespace std;


void generateFunctionLogicFormulae(list<string> *atomicBooleanFormulaeList, list<string>::iterator it, string to_SMTFormula);

int main() {
	int maxOperator = 0;
	string fileName, input;
	cout << "Enter the parsed file name please: ";
	cin >> fileName; //project636Test

	list<string> *int_BooleanOperatorsListI = new list<string>();
	list<string> *int_arithmeticOperatorsListI = new list<string>();
	list<string> *atomicBooleanFormulaeListI = new list<string>();

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

	BooleanFormulaeGenetator *testClass = new BooleanFormulaeGenetator(fileName, int_BooleanOperatorsListI, int_arithmeticOperatorsListI, atomicBooleanFormulaeListI);

	string *printer = new string[20];
	for (int i = 0; i < 20; i++)printer[i] = " ";

	for (int i = 1; i <= maxOperator; i++) {

		testClass->generateFormulae(i, false, true, printer, 0);// operator counter // boolean not taken // must take component
	}

	cout << endl << atomicBooleanFormulaeListI->size();
	list<string>::iterator it = atomicBooleanFormulaeListI->begin();
	generateFunctionLogicFormulae(atomicBooleanFormulaeListI,it,"");
	

	system("PAUSE");
	return 0;
}




void generateFunctionLogicFormulae(list<string> *atomicBooleanFormulaeList , list<string>::iterator it, string to_SMTFormula) {

	//to generate all possible function logic representations we just run recursively on the List by either taking true or not true
	// now we build a tree and check at each atomic formula added is SMT can give us SAT if yes we coninue
	//.. if no we just cut this part of the tree
	
	if (it == atomicBooleanFormulaeList->end()) {
		
		// here to_SMTFormula will hold a possible SAT function 
		// we call smt on it and go try it on the function to check the output
		//now we reason 

		return;
	}
	
	

	
	string takeTrue = to_SMTFormula;
	takeTrue.append(*it).append(",");//case of take it true..now check SMT to know if i should continue
	to_SMTFormula.append("!(").append(*it).append("),");//case of take it false..now check SMT to know if i should continue
	cout << takeTrue<<endl;
	cout << to_SMTFormula << endl << endl;
	it++;
	
	/*


	call SMT on takeTrue string
	if(SAT)generateFunctionLogicFormulae(atomicBooleanFormulaeList, it, takeTrue);
	
	call SMT on take not true ( on to-SMTFomula string)
	if(SAT)generateFunctionLogicFormulae(atomicBooleanFormulaeList, it, to_SMTFormula);
	
	*/
	
	// when u add the right code the next call should be removed
	generateFunctionLogicFormulae(atomicBooleanFormulaeList, it, to_SMTFormula);

	

	
}