#pragma once

#include<string>
#include <fstream>
#include<iostream>
#include<list>
#include"Component.h"

using namespace std;

class BooleanFormulaeGenetator
{

private:
	//2 sets - 1- set of boolean operators for int--  -2- set of arithmetic operator
	//set of components read from the parsed file
	list<Component*> *componentList= new list<Component*>();
	list<string> *int_BooleanOperatorsList;				//predefined--" <,>,=="
	list<string> *int_arithmeticOperatorsList;			//predefined--"+,*,-"

	ifstream typeTheoryParserFile;						//name of the opened file
	string fileLineGet="";								//used to read a line from the file 
	string generalPath = "/Users/TOSHIBA/Desktop/";		//general path

public:
	BooleanFormulaeGenetator(string fileName, list<string> *int_BooleanOperatorsListI, list<string> *int_arithmeticOperatorsListI);
	void generateFormulae(int operatorCounter, bool boolOpTaken, bool takeComponent,string *printer,int index);							//used to ouptput the formulae
	~BooleanFormulaeGenetator();
};

