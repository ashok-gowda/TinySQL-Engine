#include "insertStatement.h"

bool isValue(std::string line, int& index, parseTree* current)
{
	parseTree* p1 = createNode(NULL, VALUE, "value");
	int counter = index;
	bool flag = false;
	if (counter != line.size() && (isLiteral(line, counter, p1) || isInteger(line, counter, p1)))
		flag = true;
	else if (counter != line.size() && matchString(line, counter, "null"))
	{
		parseTree* p2 = createNode(p1, TERMINALS, "NULL");
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

bool isValueList(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	int counter = index;
	parseTree* p1 = createNode(NULL, VALUELIST, "value-list");
	if (counter != line.size() && isValue(line, counter, p1))
	{
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && line[counter] == ',')
		{
			counter++;
			parseTree* p2 = createNode(p1, TERMINALS, ",");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && isValueList(line, counter, p1))
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

bool isAttributeList(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	int counter = index;
	parseTree* p1 = createNode(NULL, ATTRIBUTELIST, "attribute-list");
	if (counter != line.size() && isName(line, counter, ATTRIBUTENAME, p1))
	{
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && line[counter] == ',')
		{
			counter++;
			parseTree* p2 = createNode(p1, TERMINALS, ",");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && isAttributeList(line, counter, p1))
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

bool checkInsertTuples(std::string line, int& index, parseTree* current)
{
	int counter = index;
	parseTree* p1 = createNode(NULL, INSERTTUPLES, "insert-tuples");
	bool flag = false;
	if (counter != line.size() && matchString(line, counter, "values"))
	{
		parseTree* p2 = createNode(p1, TERMINALS, "VALUES");
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && line[counter] == '(')
		{
			counter++;
			parseTree* p3 = createNode(p1, TERMINALS, "(");
			if (counter != line.size() && isValueList(line, counter, p1))
			{
				if (counter != line.size() && line[counter] == ')')
				{
					counter++;
					parseTree* p4 = createNode(p1, TERMINALS, ")");
					flag = true;
				}
			}
		}
	}
	else
	{
		selectStatement* selectStmtObj = new selectStatement();
		if (counter != line.size())
		{
			selectStmtObj->parse(line, counter);
			if (selectStmtObj->isValidSyntax())
			{
				parseTree* p5 = selectStmtObj->getRoot();
				p1->setChild(p5);
				p5->setParent(p1);
				flag = true;
			}
		}
	}

	if (flag)
	{
		p1->setParent(current);
		current->setChild(p1);
		index = counter;
	}
	else
		deleteNode(p1);

	return flag;
}

void insertStatement::parse(std::string line, int& index)
{
	int counter = index;
	parseTree* p1 = createNode(NULL, INSERTSTATEMENT, "insert-statement");
	if (counter != line.size() && matchString(line, counter, "insert"))
	{
		parseTree* p2 = createNode(p1, TERMINALS, "INSERT");
		bool intoCheck = false, tableNameCheck = false, insertTuplesCheck = false, finalCheck = false;
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && matchString(line, counter, "into"))
		{
			parseTree* p3 = createNode(p1, TERMINALS, "into");
			intoCheck = true;
		}
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && isName(line, counter, TABLENAME, p1))
		{
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && line[counter] == '(')
			{
				counter++;
				parseTree* p4 = createNode(p1, TERMINALS, "(");
				while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
					counter++;
				if (counter != line.size() && isAttributeList(line, counter, p1))
				{
					while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
						counter++;
					if (counter != line.size() && line[counter] == ')')
					{
						counter++;
						parseTree* p5 = createNode(p1, TERMINALS, ")");
						tableNameCheck = true;
					}

				}
			}
		}
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && checkInsertTuples(line, counter, p1))
			insertTuplesCheck = true;
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter == line.size() || line[counter] == '\n')
			finalCheck = true;
		if (intoCheck && tableNameCheck && insertTuplesCheck && finalCheck)
		{
			syntaxValid = true;
			root = p1;
			index = counter;
		}
		else
			deleteNode(p1);
	}
}