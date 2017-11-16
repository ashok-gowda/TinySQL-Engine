#include <string>
#include "statementTypesConstants.h"

parseTree* createNode(parseTree* parent, int type, char* value)
{
	parseTree* child = new parseTree(value, type, parent);
	return child;
}

void deleteNode(parseTree* node)
{
	std::vector<parseTree*> children = node->getChildren();
	for (std::vector<parseTree*>::iterator child = children.begin(); child != children.end(); child++)
	{
		parseTree* temp = *child;
		delete temp;
	}
	delete node;
}

bool matchString(std::string line, int& index, char* str)
{
	int i = index;
	char val[100];
	std::locale loc;
	while (i < index + strlen(str) && i < line.size())
	{
		val[i - index] = std::tolower(line[i], loc);
		i++;
	}
	val[i - index] = '\0';
	if (!strcmp(val, str))
	{
		if ((index + strlen(str)) != line.size() && isalpha(line[index + strlen(str)]))
			return false;
		index += strlen(str);
		return true;
	}
	return false;
}

bool isLiteral(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	char val[100];
	int count = 0, counter = index;
	bool startFlag = false, valFlag = false, endFlag = false;
	if (counter != line.size() && line[counter] == '\"')
	{
		val[count++] = '\"';
		startFlag = true;
		counter++;
	}
	while (counter != line.size() && line[counter] != '\"')
	{
		val[count++] = line[counter];
		counter++;
		if (!valFlag)
			valFlag = true;
	}
	if (counter != line.size() && line[counter] == '\"')
	{
		val[count++] = '\"';
		endFlag = true;
		counter++;
	}
	if (startFlag && valFlag && endFlag)
	{
		val[count] = '\0';
		parseTree* p1 = createNode(current, LITERAL, "literal");
		parseTree* p2 = createNode(p1, TERMINALS, val);
		index = counter;
		flag = true;
	}
	return flag;
}

bool isComparisonOperator(std::string line, int& index, parseTree* current)
{
	if (line[index] == '>' || line[index] == '<' || line[index] == '=')
	{
		parseTree* p1 = createNode(current, COMPOP, "comp-op");
		char str[2] = { line[index] + '\0' };
		parseTree* p2 = createNode(p1, TERMINALS, str);
		index++;
		return true;
	}
	return false;
}

bool isInteger(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	char val[10];
	int count = 0, counter = index;
	while (counter != line.size() && !isspace(line[counter]) && isdigit(line[counter]))
	{
		val[count++] = line[counter];
		counter++;
		if (!flag)
			flag = true;
	}
	if (flag)
	{
		val[count] = '\0';
		parseTree* p1 = createNode(current, INTEGER, "integer");
		parseTree* p2 = createNode(p1, TERMINALS, val);
		index = counter;
	}
	return flag;
}

bool isName(std::string line, int& index, int type, parseTree* current)
{
	bool flag = false;
	int counter = index;
	if (isalpha(line[index]))
	{
		flag = true;
		counter++;
	}
	while (counter != line.size() && isalnum(line[counter]))
	{
		counter++;
	}
	if (flag)
	{
		char name[50];
		for (int i = index; i < counter; i++)
			name[i - index] = line[i];
		name[counter - index] = '\0';
		char typeValue[20];
		switch (type)
		{
		case TABLENAME:
			strcpy_s(typeValue, 20, "table-name");
			break;
		case ATTRIBUTENAME:
			strcpy_s(typeValue, 20, "attribute-name");
			break;
		}
		parseTree* p1 = createNode(current, type, typeValue);
		parseTree* p2 = createNode(p1, TERMINALS, name);
		index = counter;
	}
	return flag;
}

bool isColumnName(std::string line, int& index, parseTree* cur)
{
	bool flag = false;
	int counter = index;
	bool tableNameCheck = true;
	parseTree* p1 = createNode(NULL, COLUMNNAME, "column-name");
	if (counter != line.size() && isName(line, counter, TABLENAME, p1))
	{
		if (counter != line.size() && line[counter] == '.')
		{
			parseTree* p2 = createNode(p1, TERMINALS, ".");
			counter++;
		}
		else
		{
			p1->getChildren()[0]->setValue("attribute-name");
			p1->getChildren()[0]->setNodeType(ATTRIBUTENAME);
			tableNameCheck = false;
		}
	}
	if (tableNameCheck)
	{
		if (counter != line.size() && isName(line, counter, ATTRIBUTENAME, p1))
		{
			flag = true;
		}
	}
	else
		flag = true;
	if (flag)
	{
		p1->setParent(cur);
		cur->setChild(p1);
		index = counter;
	}
	else
		deleteNode(p1);
	return flag;
}

bool isDataType(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	int counter1 = index, counter2 = index;
	char val[10];
	while (counter1 - index < 3 && counter1 != line.size())
	{
		val[counter1 - index] = line[counter1];
		counter1++;
	}
	val[counter1 - index] = '\0';
	if (counter1 != line.size() && !strcmp(val, "INT") && (isspace(line[counter1]) || line[counter1]==',' || line[counter1] == ')'))
		flag = true;
	if (!flag)
	{
		while (counter2 - index < 5 && counter2 != line.size())
		{
			val[counter2 - index] = line[counter2];
			counter2++;
		}
		val[counter2 - index] = '\0';
		if (counter2 != line.size() && !strcmp(val, "STR20") && (isspace(line[counter2]) || line[counter2] == ',' || line[counter2] == ')'))
			flag = true;
	}
	if (flag)
	{
		parseTree* p1 = createNode(current, DATATYPE, "data-type");
		parseTree* p2 = createNode(p1, TERMINALS, val);
		if (counter2 == index)
			index = counter1;
		else
			index = counter2;
	}
	return flag;
}

bool isTerm(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	int counter = index;
	parseTree* p1 = createNode(NULL, TERM, "term");
	if (counter != line.size() && isColumnName(line, counter, p1))
		flag = true;
	else if (counter != line.size() && isLiteral(line, counter, p1))
		flag = true;
	else if (counter != line.size() && isInteger(line, counter, p1))
		flag = true;
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

bool isExpression(std::string line, int& index, parseTree* current)
{
	int counter = index;
	bool flag = false;
	parseTree* p1 = createNode(NULL, EXPRESSION, "expression");
	if (counter != line.size() && line[counter] != '(')
	{
		if (counter != line.size() && isTerm(line, counter, p1))
			flag = true;
	}
	else
	{
		counter++;
		parseTree *p2, *p3, *p4;
		p2 = createNode(p1, TERMINALS, "(");
		if (counter != line.size() && isTerm(line, counter, p1))
		{
			bool operatorFlag = true;
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && line[counter] == '+')
				p3 = createNode(p1, TERMINALS, "+");
			else if (counter != line.size() && line[counter] == '-')
				p3 = createNode(p1, TERMINALS, "-");
			else if (counter != line.size() && line[counter] == '*')
				p3 = createNode(p1, TERMINALS, "*");
			else
				operatorFlag = false;
			if (operatorFlag)
			{
				counter++;
				while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
					counter++;
				if (counter != line.size() && isTerm(line, counter, p1))
				{
					if (counter != line.size() && line[counter] == ')')
					{
						counter++;
						p4 = createNode(p1, TERMINALS, ")");
						flag = true;
					}
				}
			}
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

bool isBooleanFactor(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	int counter = index;
	parseTree* p1 = createNode(NULL, BOOLEANFACTOR, "boolean-factor");
	if (counter != line.size() && isExpression(line, counter, p1))
	{
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && isComparisonOperator(line, counter, p1))
		{
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && isExpression(line, counter, p1))
				flag = true;
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

bool isBooleanTerm(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	parseTree* p1 = createNode(NULL, BOOLEANTERM, "boolean-term");
	int counter = index;
	if (counter != line.size() && isBooleanFactor(line, counter, p1))
	{
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && matchString(line, counter, "and"))
		{
			parseTree* p2 = createNode(p1, TERMINALS, "AND");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && isBooleanTerm(line, counter, p1))
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

bool checkSearchCondition(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	parseTree* p1 = createNode(NULL, SEARCHCONDITION, "search-condition");
	int counter = index;
	if (counter != line.size() && isBooleanTerm(line, counter, p1))
	{
		while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (counter != line.size() && matchString(line, counter, "or"))
		{
			parseTree* p2 = createNode(p1, TERMINALS, "OR");
			while (counter != line.size() && line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (counter != line.size() && checkSearchCondition(line, counter, p1))
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