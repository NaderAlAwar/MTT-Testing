#include "BooleanFormulaeGenetator.h"



BooleanFormulaeGenetator::BooleanFormulaeGenetator(string fileName, list<string> *int_BooleanOperatorsListI, list<string> *int_arithmeticOperatorsListI, list<string> *atomicBooleanFormulaeListI) {

	this->int_arithmeticOperatorsList = int_arithmeticOperatorsListI;
	this->int_BooleanOperatorsList = int_BooleanOperatorsListI;
	this->atomicBooleanFormulaeList = atomicBooleanFormulaeListI;

	typeTheoryParserFile.open(fileName);


	if (typeTheoryParserFile.fail())cout << "Oops, cannot open file" << endl;//checking if we couldn't open the file
	else {
		cout << "\n\t\tFile opened successfully!!\n\n";
		string identifier;
		string type;
		string dataType;
		while (getline(typeTheoryParserFile, fileLineGet)) {
			if (fileLineGet.length() == 0)continue; // for empty lines

			identifier = fileLineGet;
			getline(typeTheoryParserFile, type);
			getline(typeTheoryParserFile, dataType);
			componentList->push_back(new Component(identifier, type, dataType));

			}
			
	   }
}

void BooleanFormulaeGenetator::generateFormulae(int operatorCounter,bool boolOpTaken,bool takeComponent,string *printer,int index) {
	
	if (operatorCounter == 0) {//must take a component of and return--maybe add ann if of the type
		
		for (list<Component*>::iterator it = componentList->begin(); it != componentList->end(); it++) {
			printer[index] = (*it)->getIdentifier();
			
			string atomicBooleanFormula="";
			atomicBooleanFormula=printer[0];

			for (int i = 1; i <= index; i++)atomicBooleanFormula.append(printer[i]);
			
			atomicBooleanFormulaeList->push_back(atomicBooleanFormula);
			cout << atomicBooleanFormulaeList->back()<<endl;
			
			//for (int i = 0; i <= index; i++)cout << printer[i];//printing
			//cout<<endl;

		}
		return;
	}

	if (takeComponent) {//take a component
		for (list<Component*>::iterator it = componentList->begin(); it != componentList->end(); it++) {
			printer[index] = (*it)->getIdentifier();
			//call recursive with must takecomponent 
			generateFormulae(operatorCounter, boolOpTaken, false, printer, index + 1);
		}
		return;
	}

	if (operatorCounter == 1 && !boolOpTaken ) {//must take a boolean operator
		
		for (list<string>::iterator it = int_BooleanOperatorsList->begin(); it != int_BooleanOperatorsList->end(); it++) {
			printer[index] = *it;
			//call recursive with must takecomponent 
			generateFormulae(0, true, true,printer,index+1);
		}
		return;
	}
	
	//we still have 2 cases --either take a arithmeetic or a boolean operator(if u didnt)
	if (!boolOpTaken) {
		for (list<string>::iterator it = int_BooleanOperatorsList->begin(); it != int_BooleanOperatorsList->end(); it++) {
			printer[index] = *it;
			//call recursive with must takecomponent 
			generateFormulae(operatorCounter - 1, true, true, printer, index + 1);
		}
		for (list<string>::iterator it = int_arithmeticOperatorsList->begin(); it != int_arithmeticOperatorsList->end(); it++) {
			printer[index] = *it;
			//call recursive with must takecomponent  and keeping the same boolOpTaken
			generateFormulae(operatorCounter - 1, false, true, printer, index + 1);
		}
		return;
	}
	//last case take a arithmetic operator
	for (list<string>::iterator it = int_arithmeticOperatorsList->begin(); it != int_arithmeticOperatorsList->end(); it++) {
		printer[index] = *it;
		//call recursive with must takecomponent  and keeping the same boolOpTaken which is true
		generateFormulae(operatorCounter - 1, true, true, printer, index + 1);
	}
	return;
}

BooleanFormulaeGenetator::~BooleanFormulaeGenetator()
{

}
