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

parseTree* createChild(parseTree* parent, int type, char* value)
{
	parseTree* child = new parseTree(value, type, parent);
	return child;
}

bool isLiteral(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	char val[10];
	int count = 0;
	while (index != line.size() && !isspace(line[index]) && isdigit(line[index]))
	{
		val[count++] = line[index];
		index++;
		if (!flag)
			flag = true;
	}
	if (flag)
	{
		parseTree* p1 = createChild(current, INTEGER, "integer");
		parseTree* p2 = createChild(p1, TERMINALS, val);
	}
	return flag;
}
bool isComparisonOperator(std::string line, int& index, parseTree* current)
{
	if (line[index] == '>' || line[index] == '<' || line[index] == '=')
	{
		parseTree* p1 = createChild(current, COMPOP, "comp-op");
		char str[2] = { line[index] + '\0' };
		parseTree* p2 = createChild(p1, TERMINALS, str);
		index++;
		return true;
	}
	return false;
}

bool isInteger(std::string line, int& index, parseTree* current)
{
	bool flag = false;
	char val[10];
	int count = 0;
	while (index != line.size() && !isspace(line[index]) && isdigit(line[index]))
	{
		val[count++] = line[index];
		index++;
		if (!flag)
			flag = true;
	}
	if (flag)
	{
		parseTree* p1 = createChild(current, INTEGER, "integer");
		parseTree* p2 = createChild(p1, TERMINALS, val);
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
	while (counter != line.size() && !isspace(line[counter]) && line[counter]!='.')
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
		parseTree* p1 = createChild(current, type, typeValue);
		parseTree* p2 = createChild(p1, TERMINALS, name);
		index = counter;
	}
	return flag;
}

bool isColumnName(std::string line, int& index, parseTree* cur)
{
	bool flag = false;
	int counter = index;
	if (isName(line, counter, TABLENAME, cur))
	{
		if (line[counter] == '.')
		{
			parseTree* p1 = createChild(cur, TERMINALS, ".");
			counter++;
		}
		else
			return false;
	}
	if (isName(line, counter, ATTRIBUTENAME, cur))
	{
		flag = true;
	}
	if (flag)
		index = counter;
	return flag;
}

