#include "deleteStatement.h"

void deleteStatement::parse(std::string line, int& index)
{
	int counter = index;
	parseTree* p1 = createNode(NULL, DELETESTATEMENT, "delete-statement");
	if (counter != line.size() && matchString(line, counter, "delete"))
	{
		parseTree* p2 = createNode(p1, TERMINALS, "DELETE");
		bool fromCheck = false, whereCheck = true, finalSizeCheck = false;
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && matchString(line, counter, "from"))
		{
			parseTree* p3 = createNode(p1, TERMINALS, "FROM");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && isName(line, counter, TABLENAME, p1))
				fromCheck = true;
		}
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && matchString(line, counter, "where"))
		{
			parseTree* p4 = createNode(p1, TERMINALS, "WHERE");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (!(counter != line.size() && checkSearchCondition(line, counter, p1)))
				whereCheck = false;
		}
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter == line.size() || line[counter] == '\n')
			finalSizeCheck = true;
		if (fromCheck && whereCheck && finalSizeCheck)
		{
			syntaxValid = true;
			root = p1;
			index = counter;
		}
		else
			deleteNode(p1);
	}
}