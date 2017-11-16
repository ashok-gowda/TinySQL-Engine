#include<iostream>
#include "Operand.h"
#include<vector>

class operatorStack
{
	std::vector<OperandOperator*> stackObj;
public:
	operatorStack()
	{
	}
	void push(OperandOperator* oValue)
	{
		stackObj.push_back(oValue);
	}
	OperandOperator* pop()
	{
		OperandOperator* value = stackObj[stackObj.size() - 1];
		stackObj.pop_back();
		return value;
	}
	bool isEmpty()
	{
		return stackObj.empty();
	}
};

class operandStack
{
	std::vector<std::vector<OperandOperator*>> stackObj;
public:
	operandStack()
	{
	}
	void push(std::vector<OperandOperator*> oValue)
	{
		stackObj.push_back(oValue);
	}
	std::vector<OperandOperator*> pop()
	{
		std::vector<OperandOperator*> value = stackObj[stackObj.size() - 1];
		stackObj.pop_back();
		return value;
	}
	bool isEmpty()
	{
		return stackObj.empty();
	}
};
