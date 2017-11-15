#include <string.h>
#include <iostream>
#include "Constants.h"
class OperandOperator
{
	std::string name;
	kOperandOperatorType type;
	std::string tableName;

public:
	OperandOperator(std::string& n, kOperandOperatorType typeVal, std::string t)
	{
		name = n;
		type = typeVal;
		tableName = t;
	}
	std::string getName()
	{
		return name;
	}
	std::string getTableName()
	{
		return tableName;
	}
	kOperandOperatorType getType()
	{
		return type;
	}
};