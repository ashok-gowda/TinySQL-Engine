#include "selectStatement.h"

class insertStatement :public statement
{
public:
	insertStatement()
	{
		syntaxValid = false;
		root = NULL;
	}
	void parse(std::string line, int& index);
	parseTree* getRoot()
	{
		return root;
	}
	bool isValidSyntax()
	{
		return syntaxValid;
	}
private:
	bool syntaxValid;
	parseTree* root;
};