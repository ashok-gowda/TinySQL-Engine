#include "createStatement.h"

bool isAttributeTypeList(std::string line, int& index, parseTree* current)
{
	bool flag = true;
	int counter = index;
	parseTree* p1 = createNode(NULL, ATTRIBUTETYPELIST, "attribute-type-list");
	if (counter != line.size() && isName(line, counter, ATTRIBUTENAME, p1))
	{
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && isDataType(line, counter, p1))
		{
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && line[counter] == ',')
			{
				counter++;
				parseTree* p2 = createNode(p1, TERMINALS, ",");
				while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
					counter++;
				if (counter != line.size() && isAttributeTypeList(line, counter, p1))
					flag = true;
			}
			else
				flag = true;
		}
	}
	if (flag)
	{
		index = counter;
		p1->setParent(current);
		current->setChild(p1);
	}
	else
		deleteNode(p1);
	return flag;
}

void createStatement::parse(std::string line, int& index)
{
	int counter = index;
	parseTree* p1 = createNode(NULL, CREATETABLESTATEMENT, "create-table-statement");
	if (counter != line.size() && matchString(line, counter, "create"))
	{
		parseTree* p2 = createNode(p1, TERMINALS, "CREATE");
		bool tableCheck = false, attributeListCheck = false, finalCheck = false;
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && matchString(line, counter, "table"))
		{
			parseTree* p3 = createNode(p1, TERMINALS, "TABLE");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && isName(line, counter, TABLENAME, p1))
				tableCheck = true;
		}
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && line[counter++] == '(')
		{
			parseTree* p4 = createNode(p1, TERMINALS, "(");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && isAttributeTypeList(line, counter, p1))
			{
				while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
					counter++;
				if (counter != line.size() && line[counter++] == ')')
				{
					parseTree* p5 = createNode(p1, TERMINALS, ")");
					attributeListCheck = true;
				}
			}
		}
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter == line.size() || line[counter] == '\n')
			finalCheck = true;
		if (finalCheck && attributeListCheck && tableCheck)
		{
			root = p1;
			index = counter;
			syntaxValid = true;
		}
		else
			deleteNode(p1);
	}
}