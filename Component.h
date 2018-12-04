#pragma once

#include<string>
#include<iostream>

using namespace std;


class Component
{

private:
	string identifier;
	string type;
	string dataType;

public:
	Component(string indentifier,string type, string dataType);
	void print();
	string getDatatype();
	string getIdentifier();
	~Component();
};

