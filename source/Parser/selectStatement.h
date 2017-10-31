#include "statement.h"

class selectStatement:public statement
{
public:
	void parse();
	void parseFrom();
	void parseDistinct();
	void parseSelectList();
	void parseTableList();
	//void parseWhere();
	//void parseSearchCondition();
	void parseOrderBy();
	//void parseColumnName();
	bool checkSyntax();



private:
	bool correctSyntactically;
};