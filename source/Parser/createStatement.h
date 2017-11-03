#include "statement.h"

class createStatement :public statement
{
public:
	createStatement()
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