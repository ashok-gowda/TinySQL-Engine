#include "dropStatement.h"

void dropStatement::parse(std::string line, int& index)
{
	int counter = index;
	parseTree* p1 = createNode(NULL, DROPTABLESTATEMENT, "drop-table-statement");
	if (counter != line.size() && matchString(line, counter, "drop"))
	{
		parseTree* p2 = createNode(p1, TERMINALS, "DROP");
		bool statementCheck = false, finalCheck = false;
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && matchString(line, counter, "table"))
		{
			parseTree* p3 = createNode(p1, TERMINALS, "TABLE");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && isName(line, counter, TABLENAME, p1))
				statementCheck = true;
		}
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter == line.size() || line[counter] == '\n')
			finalCheck = true;
		if (statementCheck && finalCheck)
		{
			root = p1;
			syntaxValid = true;
			index = counter;
		}
		else
			deleteNode(p1);
	}
}