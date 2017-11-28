#ifndef STATEMENT_H
#define STATEMENT_H

#include "statementTypesConstants.h"

class statement
{
public:
	virtual void parse(std::string line, int& index, std::ofstream& fWriteExecute) { }
	virtual bool isValidSyntax() { return false; }
	virtual parseTree* getRoot() { return NULL; }

private:
	bool correctSyntactically;
};


#endif // !STATEMENT_H