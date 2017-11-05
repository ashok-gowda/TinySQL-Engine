#ifndef PARSETREE_H
#define PARSETREE_H

#include<string.h>
#include<vector>

class parseTree
{
public:
	parseTree(char* val, int type, parseTree* par = NULL);


	void setChild(parseTree* child);
	std::vector<parseTree*> getChildren();
	void setParent(parseTree* par);
	parseTree* getParent();
	void setValue(char* val);
	char* getValue();


	void setNodeType(int type);
	int getNodeType();


private:
	char* value;
	int nodeType;
	parseTree* parent;
	std::vector<parseTree*> children;
};


#endif // !PARSETREE_H