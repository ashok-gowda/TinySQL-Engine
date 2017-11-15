#include<string>
#include "Operand.h"
#include <vector>
class JoinCondition {
	std::vector<OperandOperator *> operand1;
	std::string operatorOfOperation;
	std::vector<OperandOperator *> operand2;
	kAndOrOperator andOrOperator;
public:
	JoinCondition(std::vector<OperandOperator *> operand1,std::string operatorOfOperation, std::vector<OperandOperator *> operand2) {
		this->operand1 = operand1;
		this->operatorOfOperation = operatorOfOperation;
		this->operand2 = operand2;
	}

	std::vector<OperandOperator *> getOperand1() {
		return this->operand1;
	}

	std::vector<OperandOperator *> getOperand2() {
		return this->operand2;
	}

	std::string getOperatorOfOperation() {
		return this->operatorOfOperation;
	}
	kAndOrOperator getAndOrOperator()
	{
		return andOrOperator;
	}
};