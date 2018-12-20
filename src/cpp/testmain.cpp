#include<iostream>
#include"BooleanFormulaeGenetator.h"
#include"Component.h"	
#include"z3_solver.h"
#include <map>
using namespace std;


void generateFunctionLogicFormulae(list<string> *atomicBooleanFormulaeList, list<string>::iterator it, string to_SMTFormula, vector<string> &valuationFormulae);
void AddArithmeticOperators(list<string> *outputList);
void AddBooleanOperators(list<string> *outputList);

int main(int argc, char **argv) {
	// argc has to be 4
	// argv[1] is the file generated by pycparser
	// argv[2] is the maximum number of operators allowed per arithmetic expression
	// argv[3] is the file containing the identifiers of the arguments

	if(argc != 4)
	{
		cout << "Invalid arguments" << endl;
		return 0;
	}

	string fileName(argv[1]);
	int maximumNumberOfOperators = atoi(argv[2]);
	string argumentFileName(argv[3]);

	list<string> *int_BooleanOperatorsListI = new list<string>();
	list<string> *int_arithmeticOperatorsListI = new list<string>();
	list<string> *atomicBooleanFormulaeListI = new list<string>();
	

	AddArithmeticOperators(int_arithmeticOperatorsListI);
	AddBooleanOperators(int_BooleanOperatorsListI);

	BooleanFormulaeGenetator *testClass = new BooleanFormulaeGenetator(fileName, int_BooleanOperatorsListI, int_arithmeticOperatorsListI, atomicBooleanFormulaeListI);

	string *printer = new string[20];
	for (int i = 0; i < 20; i++)printer[i] = " ";

	cout << "Generating atomic Formulae..." << endl;
	for (int i = 1; i <= maximumNumberOfOperators; i++) {

		testClass->generateFormulae(i, false, true, printer, 0);// operator counter // boolean not taken // must take component
	}

	cout << endl << atomicBooleanFormulaeListI->size() << " atomic formulas generated" << endl << endl;

	list<string>::iterator it = atomicBooleanFormulaeListI->begin();
	cout << "Generating valuation Formulae..." << endl;
	vector<string> valuationFormulae;
	generateFunctionLogicFormulae(atomicBooleanFormulaeListI,it,"",valuationFormulae);
	cout << endl << valuationFormulae.size() << " valuation formulas generated" << endl << endl;

	vector<string> satisfyingAssignments = SolveValuationFormulae(valuationFormulae);

	cout << "Solving valuation formulae..." << endl;

	for(int i = 0; i < satisfyingAssignments.size(); i++)
	{
		cout << satisfyingAssignments.at(i) << endl;
	}	

	// vector<string> argumentsVector = readArgumentIdentifiers(argumentFileName);


	return 0;
}

// map<string, int> getArgumentAssignments(vector<string> &argumentIdentifiers, string assignment)
// {
// 	map<string, int> result;
// 	for(int i = 0; i < argumentIdentifiers.size(); i++)
// 	{
// 		size_t argumentLocation = assignment.find(argumentIdentifiers.at(i));
		
// 	}
// }


vector<string> readArgumentIdentifiers(string fileName)
{
	vector<string> arguments;
	ifstream parser;
	parser.open(fileName);
	if (parser.fail())
	{
		cout << "Oops, cannot open file" << endl;//checking if we couldn't open the file
	}
	else
	{
		string nextLine;
		string identifier;
		string type;
		string dataType;
		while (getline(parser, nextLine))
		{
			if (nextLine.length() == 0)
			{
				continue; // for empty lines
			}

			arguments.push_back(nextLine);
			getline(parser, type);
			getline(parser, dataType);
		}			
	}
}

void AddArithmeticOperators(list<string> *outputList)
{
	string operators[] = {"+"};
	int numberOfOperators = 1;

	for(int i = 0; i < numberOfOperators; i++)
	{
		outputList->push_back(operators[i]);
	}
}

void AddBooleanOperators(list<string> *outputList)
{
	string operators[] = {">"};
	int numberOfOperators = 1;

	for(int i = 0; i < numberOfOperators; i++)
	{
		outputList->push_back(operators[i]);
	}
}

void generateFunctionLogicFormulae(list<string> *atomicBooleanFormulaeList , list<string>::iterator it, string to_SMTFormula, vector<string> &valuationFormulae) {

	//to generate all possible function logic representations we just run recursively on the List by either taking true or not true
	// now we build a tree and check at each atomic formula added is SMT can give us SAT if yes we coninue
	//.. if no we just cut this part of the tree
	
	if (it == atomicBooleanFormulaeList->end()) {
		
		// here to_SMTFormula will hold a possible SAT function 
		// we call smt on it and go try it on the function to check the output
		//now we reason 
		to_SMTFormula = to_SMTFormula.substr(0, to_SMTFormula.size() - 1);
		valuationFormulae.push_back(to_SMTFormula);
		cout<< valuationFormulae.back() << endl;
		return;
	}
	
	

	
	string takeTrue = to_SMTFormula;
	takeTrue.append(*it).append(",");//case of take it true..now check SMT to know if i should continue

	to_SMTFormula.append("!(").append(*it).append("),");//case of take it false..now check SMT to know if i should continue
	takeTrue = takeTrue.substr(0, takeTrue.size() - 1);
	bool addedFormulaIsSat = SolveValuationFormula(takeTrue);

	to_SMTFormula = to_SMTFormula.substr(0, to_SMTFormula.size() - 1);
	bool addedNegatedFormulaIsSat = SolveValuationFormula(to_SMTFormula);

	takeTrue.append(",");
	to_SMTFormula.append(",");

	it++;
	
	/*

	


	call SMT on takeTrue string
	if(SAT)generateFunctionLogicFormulae(atomicBooleanFormulaeList, it, takeTrue);
	
	call SMT on take not true ( on to-SMTFomula string)
	if(SAT)generateFunctionLogicFormulae(atomicBooleanFormulaeList, it, to_SMTFormula);
	
	*/


	if(addedFormulaIsSat)
	{
		generateFunctionLogicFormulae(atomicBooleanFormulaeList, it, takeTrue, valuationFormulae);
	}
	

	if(addedNegatedFormulaIsSat)
	{
		generateFunctionLogicFormulae(atomicBooleanFormulaeList, it, to_SMTFormula, valuationFormulae);
	}
	
	// when u add the right code the next call should be removed
	// generateFunctionLogicFormulae(atomicBooleanFormulaeList, it, to_SMTFormula);


	
}