#include "selectStatement.h"

bool checkTableList(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	int counter = index;
	parseTree* p1 = createNode(NULL, TABLELIST, "table-list");
	if (counter != line.size() && isName(line, counter, TABLENAME, p1))
	{
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && line[counter] == ',')
		{
			counter++;
			parseTree* p2 = createNode(p1, TERMINALS, ",");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && checkTableList(line, counter, p1))
				flag = true;
		}
		else
			flag = true;
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

bool checkSelectSubList(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	int counter = index;
	parseTree* p1 = createNode(NULL, SELECTSUBLIST, "select-sublist");
	if (counter != line.size() && isColumnName(line, counter, p1))
	{
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && line[counter] == ',')
		{
			counter++;
			parseTree* p2 = createNode(p1, TERMINALS, ",");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && checkSelectSubList(line, counter, p1))
				flag = true;
		}
		else
			flag = true;
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

bool checkSelectList(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	int counter = index;
	parseTree* p1 = createNode(NULL, SELECTLIST, "select-list");
	if (counter != line.size() && line[counter] == '*')
	{
		flag = true;
		parseTree* p2 = createNode(p1, TERMINALS, "*");
		counter++;
	}
	else
	{
		if (counter != line.size() && checkSelectSubList(line, counter, p1))
			flag = true;
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

void selectStatement::parse(std::string line, int& index)
{
	int counter = index;
	parseTree* p1 = createNode(NULL, SELECTSTATEMENT, "select-statement");
	if (counter != line.size() && matchString(line, counter, "select"))
	{
		parseTree* p2 = createNode(p1, TERMINALS, "SELECT");
		bool selectListCheck = false, fromCheck = false, whereCheck = true, orderByCheck = true, finalSizeCheck = false;
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && matchString(line, counter, "distinct"))
			parseTree* p3 = createNode(p1, TERMINALS, "DISTINCT");
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && checkSelectList(line, counter, p1))
			selectListCheck = true;
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && matchString(line, counter, "from"))
		{
			parseTree* p4 = createNode(p1, TERMINALS, "FROM");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && checkTableList(line, counter, p1))
				fromCheck = true;
		}
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && matchString(line, counter, "where"))
		{
			parseTree* p5 = createNode(p1, TERMINALS, "WHERE");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (!(counter != line.size() && checkSearchCondition(line, counter, p1)))
				whereCheck = false;
		}
		if (counter != line.size() && matchString(line, counter, "order by"))
		{
			parseTree* p5 = createNode(p1, TERMINALS, "ORDER BY");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (!(counter != line.size() && isColumnName(line, counter, p1)))
				orderByCheck = false;
		}
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter == line.size() || line[counter] == '\n')
			finalSizeCheck = true;
		if (selectListCheck && fromCheck && whereCheck && orderByCheck && finalSizeCheck)
		{
			syntaxValid = true;
			root = p1;
			index = counter;
		}
		else
			deleteNode(p1);
	}
}