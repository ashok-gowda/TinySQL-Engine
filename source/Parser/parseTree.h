#include<string.h>
#include<vector>

class parseTree
{
public:
	parseTree(char* val, int type, parseTree* par)
	{
		value = new char[strlen(val) + 1];
		strcpy_s(value, strlen(val)+1, val);
		parent = par;
		numberOfChildren = 0;
	}


	void setChild(parseTree* child)
	{
		children[numberOfChildren++] = child;
	}
	std::vector<parseTree*> getChildren()
	{
		return children;
	}


	int getNumberOfChildren()
	{
		return numberOfChildren;
	}


	void setParent(parseTree* par)
	{
		parent = par;
	}
	parseTree* getParent()
	{
		return parent;
	}
	

	void setValue(char* val)
	{
		char* temp = new char[strlen(value) + 1];
		strcpy_s(temp, strlen(val) + 1, val);
		delete[] value;
		value = temp;
	}
	char* getValue()
	{
		return value;
	}


	void setNodeType(int type)
	{
		nodeType = type;
	}
	int getNodeType()
	{
		return nodeType;
	}


private:
	char* value;
	int nodeType;
	int numberOfChildren;
	parseTree* parent;
	std::vector<parseTree*> children;
};