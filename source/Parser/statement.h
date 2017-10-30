#include "statementTypesConstants.h"

class statement
{
private:
	bool correctSyntactically;
public:
	void parse();
	void parseWhere();
	void parseSearchCondition();
	void parseColumnName();
	bool checkSyntax() { return false; }
};