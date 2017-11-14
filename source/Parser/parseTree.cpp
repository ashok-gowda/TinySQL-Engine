#include "parseTree.h"

parseTree::parseTree(char* val, int type, parseTree* par)
{
	value = new char[strlen(val) + 1];
	strcpy_s(value, strlen(val) + 1, val);
	parent = par;
	nodeType = type;
	if (par)
		par->setChild(this);
}

void parseTree::setChild(parseTree* child)
{
	children.push_back(child);
}

std::vector<parseTree*> parseTree::getChildren()
{
	return children;
}

void parseTree::setParent(parseTree* par)
{
	parent = par;
}

parseTree* parseTree::getParent()
{
	return parent;
}

void parseTree::setValue(char* val)
{
	char* temp = new char[strlen(value) + 1];
	strcpy_s(temp, strlen(val) + 1, val);
	delete[] value;
	value = temp;
}

char* parseTree::getValue()
{
	return value;
}


void parseTree::setNodeType(int type)
{
	nodeType = type;
}
int parseTree::getNodeType()
{
	return nodeType;
}