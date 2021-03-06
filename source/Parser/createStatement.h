#include "statement.h"

class createStatement :public statement
{
public:
	createStatement()
	{
		syntaxValid = false;
		root = NULL;
	}
	void parse(std::string line, int& index, std::ofstream& fWriteExecute);
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