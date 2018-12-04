#include "Component.h"



Component::Component(string indentifier, string type, string dataType){
	
	this->identifier = indentifier;
	this->type = type;
	this->dataType = dataType;


}


void Component::print() {

	cout << this->identifier;
}

string Component::getDatatype() {

	return this->dataType;
}
string Component::getIdentifier() {

	return this->identifier;
}
Component::~Component()
{
}
