#include "z3++.h"
#include <vector>
#include <iostream>
#include <string>
#include <bits/stdc++.h> 
#include <stdlib.h>
#include <iterator>
#include "z3_solver.h"

using namespace z3;

std::string GetVariableAssignments(model &m)
{
    std::string result = "";
    for (unsigned i = 0; i < m.size(); i++) {
        func_decl v = m[i];
        result = result + v.name().str() + "=" + m.get_const_interp(v).to_string() + "," ;
    }
    result = result.substr(0, result.size() - 1);
    return result;
}

bool SolveValuationFormula(std::string &valuationFormula)
{
    context c;
    expr valuationFormulaExpression = FormValuationFormula(valuationFormula, c, ',');
    solver s(c);
    s.add(valuationFormulaExpression);
    if(s.check() == unsat)
    {
        return false;
    }
    else
    {
        return true;
    }
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
    std::string booleanOperators[] = {"<", ">", "<=", ">=", "==", "!=" };
    for (int i = 0 ; i < 6; i++)
    {
        size_t result = expression.find(booleanOperators[i]);
        if (result != std::string::npos)
        {
            return result;
        }
    }
    return std::string::npos;
}

expr FormBooleanExpression(std::string &booleanExpr, context &c)
{
    bool isNegated = false;
    std::string booleanExpression = booleanExpr; // will remove the negation operator if it exists TODO: add const to parameter
    if (booleanExpr[0] == '!')
    {
        isNegated = true;
        booleanExpression = booleanExpr.substr(2, booleanExpr.size() -3);
    }
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

    expr leftExpression = FormArithmeticExpression(leftHalf, c);
    expr rightExpression = FormArithmeticExpression(rightHalf, c);

    expr result = leftExpression; // expr objects can't be empty
    if(booleanExpression[operatorLocation] == '>' && booleanExpression[operatorLocation + 1] == '=')
    {
        result = leftExpression >= rightExpression;
    }
    else if(booleanExpression[operatorLocation] == '<' && booleanExpression[operatorLocation + 1] == '=')
    {
        result = leftExpression <= rightExpression;
    }
    else if(booleanExpression[operatorLocation] == '!' && booleanExpression[operatorLocation + 1] == '=')
    {
        result = leftExpression != rightExpression;
    }
    else if(booleanExpression[operatorLocation] == '=' && booleanExpression[operatorLocation + 1] == '=')
    {
        result = leftExpression == rightExpression;
    }
    else if(booleanExpression[operatorLocation] == '>')
    {
        result = leftExpression > rightExpression;
    }
    else if(booleanExpression[operatorLocation] == '<')
    {
        result = leftExpression < rightExpression;
    }

    if(isNegated)
    {
        return !result;
    }

    return result;
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
