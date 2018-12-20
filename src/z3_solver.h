#include "z3++.h"
#include <string>
#include <vector>

using namespace z3;

size_t FindBooleanOperator(std::string &expression);
expr FormBooleanExpression(std::string &booleanExpression, context &c);
size_t GetNextArithmeticOperator(std::string &arithmeticExpression);
bool CheckIfInteger(std::string &stringToCheck, int &outValue);
expr FormArithmeticExpression(std::string &arithmeticExpression, context &c);
expr FormValuationFormula(std::string &valuationFormula, context &c, char delimiter);
std::vector<std::string> SolveValuationFormulae(std::vector<std::string> &valuationFormulae);
bool SolveValuationFormula(std::string &valuationFormula);
std::string GetVariableAssignments(model &m);