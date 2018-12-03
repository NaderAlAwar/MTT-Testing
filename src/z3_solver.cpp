#include "z3++.h"
#include <vector>
#include <iostream>
#include <string>
#include <bits/stdc++.h> 
#include<stdlib.h> 

using namespace z3;

size_t FindBooleanOperator(std::string &expression)
{
	std::string booleanOperators[] = {"<", ">", "<=", ">=", "!="};
	for (int i = 0 ; i < 5; i++)
	{
		size_t result = expression.find(booleanOperators[i]);
		if (result != std::string::npos)
		{
			return result;
		}
	}
	return std::string::npos;
}

expr* FormBooleanExpression(std::string &booleanExpression)
{
    size_t operatorLocation = FindBooleanOperator(booleanExpression);
    std::string leftHalf = booleanExpression.substr(0, operatorLocation);
    std::string rightHalf;
    if (booleanExpression[operatorLocation + 1] == '=')
    {
        rightHalf = booleanExpression.substr(operatorLocation + 2);
    }
    else
    {
        rightHalf = booleanExpression.substr(operatorLocation + 1);
    }
    std::cout << "LEFT " << leftHalf << std::endl;
    std::cout << "RIGHT " << rightHalf << std::endl;

    return NULL;
}

size_t GetNextArithmeticOperator(std::string &arithmeticExpression)
{
    std::string arithmeticOperators[] = {"+", "-", "*"};
    std::vector<size_t> operatorLocations;
    for (int i = 0 ; i < 3; i++)
    {
        size_t result = arithmeticExpression.find(arithmeticOperators[i]);
        if (result != std::string::npos)
        {
            operatorLocations.push_back(result);
        }
    }
    if (operatorLocations.size() == 0)
    {
        return std::string::npos;
    }
    size_t closestOperator = operatorLocations.at(0);
    for (int i = 0; i < operatorLocations.size(); i++)
    {
        if (operatorLocations.at(i) < closestOperator)
        {
            closestOperator = operatorLocations.at(i);
        }
    }
    return closestOperator;
}

bool CheckIfInteger(std::string &stringToCheck, int &outValue)
{
    for(int i = 0; i < stringToCheck.length(); i++)
    {
        if (isdigit(stringToCheck[i]) == 0)
        {
            return false;
        }
    }
    outValue = std::stoi(stringToCheck);
    return true;
}

expr FormArithmeticExpression(std::string &arithmeticExpression, context &c)
{
    size_t nextOperatorLocation = GetNextArithmeticOperator(arithmeticExpression);

    if (nextOperatorLocation == std::string::npos) //meaning no operators left in the string
    {
        return c.int_const(arithmeticExpression.c_str());
    }

    std::string nextVariable = arithmeticExpression.substr(0, nextOperatorLocation);

    int nextConstant;
    expr nextExpression = c.int_const("a"); // no default constructor
    if (CheckIfInteger(nextVariable, nextConstant))
    {
        nextExpression = c.int_val(nextConstant);
    }
    else
    {
        nextExpression = c.int_const(nextVariable.c_str());
    }

    std::string remainingArithmeticExpression = arithmeticExpression.substr(nextOperatorLocation + 1);

    if (arithmeticExpression[nextOperatorLocation] == '+')
    {
        return nextExpression + FormArithmeticExpression(remainingArithmeticExpression, c);
    }
    else if (arithmeticExpression[nextOperatorLocation] == '-')
    {
        return nextExpression - FormArithmeticExpression(remainingArithmeticExpression, c);
    }
    else if (arithmeticExpression[nextOperatorLocation] == '*')
    {
        return nextExpression * FormArithmeticExpression(remainingArithmeticExpression, c);
    }
}


void GetVariables()
{

}

int main()
{
    std::string test = "xss+y+2+z";
    // std::cout << GetNextArithmeticOperator(test);
    // expr* temp = FormBooleanExpression(test);
    // test = "x+y+z<2";
    // expr* temp2 = FormBooleanExpression(test);
	// std::cout << "find_model_example1\n";
    context c;
    expr x = FormArithmeticExpression(test, c);
    std::cout << x;
    // expr x = c.int_const("x");
    // expr y = c.int_const("y");
    // expr z = c.int_const("x");
    // // expr q = z < 1;
    // // // expr w = q + 1;
    // // std::cout << q;
    // solver s(c);

    // s.add(z > 7);
    // // s.add(x >= 1);
    // s.add(y < x + 3);
    // std::cout << s.check() << "\n";

    // model m = s.get_model();
    // std::cout << m << "\n";
    // // traversing the model
    // for (unsigned i = 0; i < m.size(); i++) {
    //     func_decl v = m[i];
    //     // this problem contains only constants
    //     assert(v.arity() == 0); 
    //     std::cout << v.name() << " = " << m.get_const_interp(v) << "\n";
    // }
 //    // we can evaluate expressions in the model.
 //    std::cout << "x + y + 1 = " << m.eval(x + y + 1) << "\n";
}

