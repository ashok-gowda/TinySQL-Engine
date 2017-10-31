#include <string>
#include "parseTree.h"

#define LETTER 1
#define DIGIT 2
#define INTEGER 3
#define COMPOP 4
#define TABLENAME 5
#define ATTRIBUTENAME 6
#define COLUMNNAME 7
#define LITERAL 8
#define CREATETABLESTATEMENT 9
#define DATATYPE 10
#define ATTRIBUTETYPELIST 11
#define DROPTABLESTATEMENT 12
#define SELECTSTATEMENT 13
#define SELECTLIST 14
#define SELECTSUBLIST 15
#define TABLELIST 16
#define DELETESTATEMENT 17
#define INSERTSTATEMENT 18
#define INSERTTUPLES 19
#define ATTRIBUTELIST 20
#define VALUE 21
#define VALUELIST 22
#define SEARCHCONDITION 23
#define BOOLEANTERM 24
#define BOOLEANFACTOR 25
#define EXPRESSION 26
#define TERM 27
#define TERMINALS 28

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

bool isLiteral(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	char val[100];
	int count = 0, counter=index;
	while (counter != line.size() && !isspace(line[counter]) && line[counter]!=')')
	{
		if (line[counter] == '\"')
		{
			flag = false;
			break;
		}
		else
		{
			if (!flag)
				flag = true;
			val[count++] = line[counter];
			counter++;
		}
	}
	if (flag)
	{
		val[count] = '\0';
		parseTree* p1 = createNode(current, LITERAL, "literal");
		parseTree* p2 = createNode(p1, TERMINALS, val);
		index = counter;
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
	int count = 0, counter=index;
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
	while (counter != line.size() && !isspace(line[counter]) && line[counter]!='.' && line[counter]!=')')
	{
		if (isalnum(line[counter]))
			counter++;
		else
		{
			flag = false;
			break;
		}
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
	parseTree* p1 = createNode(NULL, COLUMNNAME, "column-name");
	if (isName(line, counter, TABLENAME, p1))
	{
		if (line[counter] == '.')
		{
			parseTree* p2 = createNode(p1, TERMINALS, ".");
			counter++;
		}
		else
			return false;
	}
	if (isName(line, counter, ATTRIBUTENAME, p1))
	{
		flag = true;
	}
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
	}
	val[counter1 - index] = '\0';
	if (!strcmp(val, "INT") && isspace(line[counter1]))
		flag = true;
	if (!flag)
	{
		while (counter2 - index < 5 && counter2 != line.size())
		{
			val[counter2 - index] = line[counter2];
		}
		val[counter2 - index] = '\0';
		if (!strcmp(val, "STR20") && isspace(line[counter2]))
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
	if (isColumnName(line, counter, p1))
		flag = true;
	else if (isLiteral(line, counter, p1))
		flag = true;
	else if (isInteger(line, counter, p1))
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
	if (line[counter++] != '(')
	{
		if (isTerm(line, counter, p1))
			flag = true;
	}
	else
	{
		parseTree *p2, *p3, *p4;
		p2 = createNode(p1, TERMINALS, "(");
		if (isTerm(line, counter, p1))
		{
			bool operatorFlag = true;
			while (line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (line[counter] == '+')
				p3 = createNode(p1, TERMINALS, "+");
			else if (line[counter] == '-')
				p3 = createNode(p1, TERMINALS, "-");
			else if (line[counter] == '*')
				p3 = createNode(p1, TERMINALS, "*");
			else
				operatorFlag = false;
			if (operatorFlag)
			{
				counter++;
				while (line[counter] != '\n' && isspace(line[counter]))
					counter++;
				if (isTerm(line, counter, p1))
				{
					if (line[counter++] == ')')
					{
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
	if (isExpression(line, counter, p1))
	{
		while (line[counter] != '\n' && isspace(line[counter]))
			counter++;
		if (isComparisonOperator(line, counter, p1))
		{
			while (line[counter] != '\n' && isspace(line[counter]))
				counter++;
			if (isExpression(line, counter, p1))
				flag = true;
		}
	}
	if (flag)
	{
		p1->setParent(current);
		current->setChild(p1);
	}
}
bool checkSearchCondition(std::string line, int& index, parseTree* current)
{

}