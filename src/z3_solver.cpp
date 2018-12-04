#include "z3++.h"
#include <vector>
#include <iostream>
#include <string>
#include <bits/stdc++.h> 
#include <stdlib.h>
#include <iterator>

using namespace z3;

size_t FindBooleanOperator(std::string &expression);
expr FormBooleanExpression(std::string &booleanExpression, context &c);
size_t GetNextArithmeticOperator(std::string &arithmeticExpression);
bool CheckIfInteger(std::string &stringToCheck, int &outValue);
expr FormArithmeticExpression(std::string &arithmeticExpression, context &c);
expr FormValuationFormula(std::string &valuationFormula, context &c, char delimiter);
std::vector<std::string> SolveValuationFormulae(std::vector<std::string> &valuationFormulae);
std::string GetVariableAssignments(model &m);

void GetVariables()
{

}

int main()
{
    std::vector<std::string> v;
    v.push_back("xss+y*5+2+z<=7+643+fga,fg9+78+92>x");
    v.push_back("1>2");
    v.push_back("8+xss==3");
    v = SolveValuationFormulae(v);
    for (int i = 0; i < v.size(); i++)
    {
        std::cout << v.at(i) << std::endl;
    }
    // context c;

    // std::cout << GetNextArithmeticOperator(test);
    // expr temp = FormValuationFormula(test, c, ',');
    // test = "x+y+z<2";
    // expr* temp2 = FormBooleanExpression(test);
	// std::cout << "find_model_example1\n";
    // expr x = FormArithmeticExpression(test, c);
    // std::cout << temp;
    // expr x = c.int_const("x");
    // expr y = c.int_const("y");
    // expr z = c.int_const("x");
    // // expr q = z < 1;
    // // // expr w = q + 1;
    // // std::cout << q;
    // solver s(c);
    // s.add(temp);

    // s.add(z > 7);
    // // s.add(x >= 1);
    // s.add(y < x + 3);
    // std::cout << s.check() << "\n";

    // model m = s.get_model();
    // std::cout << m << "\n";
    // traversing the model
    // for (unsigned i = 0; i < m.size(); i++) {
    //     func_decl v = m[i];
    //     // this problem contains only constants
    //     assert(v.arity() == 0); 
    //     std::cout << v.name() << " = " << m.get_const_interp(v) << "\n";
    // }
 //    // we can evaluate expressions in the model.
 //    std::cout << "x + y + 1 = " << m.eval(x + y + 1) << "\n";
}

std::string GetVariableAssignments(model &m)
{
    std::string result = "";
    for (unsigned i = 0; i < m.size(); i++) {
        func_decl v = m[i];
        result = result + "," + v.name().str() + "=" + m.get_const_interp(v).to_string();
    }
    return result;
}

std::vector<std::string> SolveValuationFormulae(std::vector<std::string> &valuationFormulae)
{
    std::vector<std::string> result;
    for(int i = 0; i < valuationFormulae.size() ; i++)
    {
        context c;
        expr valuationFormula = FormValuationFormula(valuationFormulae.at(i), c, ',');
        solver s(c);
        s.add(valuationFormula);
        if(s.check() == unsat)
        {
            result.push_back("unsat");
        }
        else
        {
            model m = s.get_model();
            result.push_back(GetVariableAssignments(m));
        }
    }
    return result;
}



expr FormValuationFormula(std::string &valuationFormula, context &c, char delimiter)
{
    size_t delimiterLocation = valuationFormula.find(delimiter);

    if (delimiterLocation == std::string::npos)
    {
        return FormBooleanExpression(valuationFormula, c);
    }

    std::string atomicFormula = valuationFormula.substr(0, delimiterLocation);
    expr atomicExpression = FormBooleanExpression(atomicFormula, c);
    std::string remainingValuationFormula = valuationFormula.substr(delimiterLocation + 1);
    return atomicExpression && FormValuationFormula(remainingValuationFormula, c, delimiter);
}

size_t FindBooleanOperator(std::string &expression)
{
    std::string booleanOperators[] = {"<", ">", "<=", ">=", "==", "!="};
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

expr FormBooleanExpression(std::string &booleanExpression, context &c)
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

    expr leftExpression = FormArithmeticExpression(leftHalf ,c);
    expr rightExpression = FormArithmeticExpression(rightHalf, c);

    if(booleanExpression[operatorLocation] == '>' && booleanExpression[operatorLocation + 1] == '=')
    {
        return leftExpression >= rightExpression;
    }
    else if(booleanExpression[operatorLocation] == '<' && booleanExpression[operatorLocation + 1] == '=')
    {
        return leftExpression <= rightExpression;
    }
    else if(booleanExpression[operatorLocation] == '!' && booleanExpression[operatorLocation + 1] == '=')
    {
        return leftExpression != rightExpression;
    }
    else if(booleanExpression[operatorLocation] == '=' && booleanExpression[operatorLocation + 1] == '=')
    {
        return leftExpression == rightExpression;
    }
    else if(booleanExpression[operatorLocation] == '>')
    {
        return leftExpression > rightExpression;
    }
    else if(booleanExpression[operatorLocation] == '<')
    {
        return leftExpression < rightExpression;
    }

    return leftExpression;
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
        int nextConstant;
        if (CheckIfInteger(arithmeticExpression, nextConstant))
        {
            return c.int_val(nextConstant);
        }
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
