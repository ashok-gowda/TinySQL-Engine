#include<string>
class JoinCondition {
	std::string operand1;
	std::string operatorOfOperation;
	std::string operand2;
	bool isOperand2Variable;

public:
	JoinCondition(std::string operand1,std::string operatorOfOperation,std::string operand2, bool isOperand2Variable) {
		this->operand1 = operand1;
		this->operatorOfOperation = operatorOfOperation;
		this->isOperand2Variable = isOperand2Variable;
		this->operand2 = operand2;
	}

	std::string getOperand1() {
		return this->operand1;
	}

	std::string getOperand2() {
		return this->operand2;
	}

	std::string getOperatorOfOperation() {
		return this->operatorOfOperation;
	}

	bool getOperand2Variable() {
		return this->isOperand2Variable;
	}

};