#include "Executer.h"

std::string getSingleRelationName(std::vector<parseTree*>& terminals)
{
	for (std::vector<parseTree*>::iterator iter = terminals.begin(); iter != terminals.end(); iter++)
	{
		if ((*iter)->getParent()->getNodeType() == TABLENAME)
			return (*iter)->getValue();
	}
}

bool checkScopeOfAttributes(vector<vector<JoinCondition*>>& selectConditions, vector<string>& tableNames)
{
	for (vector<vector<JoinCondition*>>::iterator iter = selectConditions.begin(); iter != selectConditions.end(); iter++)
	{
		vector<JoinCondition*> andConditions;
		for (vector<JoinCondition*>::iterator iteriter = (*iter).begin(); iteriter != (*iter).end(); iteriter++)
		{
			vector<OperandOperator*> operand1 = (*iteriter)->getOperand1();
			vector<OperandOperator*> operand2 = (*iteriter)->getOperand2();
			for (vector<OperandOperator*>::iterator opIter = operand1.begin(); opIter != operand1.end(); opIter++)
			{
				if ((*opIter)->getType() == kOperandOperatorType::VARIABLE)
				{
					string tableName = (*opIter)->getTableName();
					bool found = false;
					for (vector<string>::iterator tIter = tableNames.begin(); tIter != tableNames.end(); tIter++)
					{
						if (!strcmp(tableName.c_str(), (*tIter).c_str()))
						{
							found = true;
							break;
						}
					}
					if (!found)
						return false;
				}
			}

			for (vector<OperandOperator*>::iterator opIter = operand2.begin(); opIter != operand2.end(); opIter++)
			{
				if ((*opIter)->getType() == kOperandOperatorType::VARIABLE)
				{
					string tableName = (*opIter)->getTableName();
					bool found = false;
					for (vector<string>::iterator tIter = tableNames.begin(); tIter != tableNames.end(); tIter++)
					{
						if (!strcmp(tableName.c_str(), (*tIter).c_str()))
						{
							found = true;
							break;
						}
					}
					if (!found)
						return false;
				}
			}
		}
	}
	return true;
}

bool doesFitInMemory(Relation* relation, int maxMemorySize)
{
	if (relation->getNumOfBlocks() <= maxMemorySize)
		return true;
	return false;
}

int min(int a, int b)
{
	return (a < b) ? (a) : (b);
}

void getTerminals(parseTree* node, std::vector<parseTree*>& terminals)
{
	if (node->getNodeType() == TERMINALS)
	{
		terminals.push_back(node);
		return;
	}
	vector<parseTree*> children = node->getChildren();
	for (vector<parseTree*>::iterator iter = children.begin(); iter != children.end(); iter++)
	{
		getTerminals(*iter, terminals);
	}
}

std::vector<std::string> getFieldNames(std::vector<parseTree*>& terminals)
{
	std::vector<std::string> fieldNames;
	for (vector<parseTree*>::iterator iter = terminals.begin(); iter != terminals.end(); iter++)
	{
		if ((*iter)->getParent()->getNodeType() == ATTRIBUTENAME)
		{
			fieldNames.push_back((*iter)->getValue());
		}
	}
	return fieldNames;
}


std::vector<enum FIELD_TYPE> getFieldTypes(std::vector<parseTree*>& terminals)
{
	std::vector<enum FIELD_TYPE> fieldTypes;
	for (vector<parseTree*>::iterator iter = terminals.begin(); iter != terminals.end(); iter++)
	{
		if ((*iter)->getParent()->getNodeType() == DATATYPE)
		{
			if (!strcmp((*iter)->getValue(), "STR20"))
				fieldTypes.push_back(STR20);
			else if(!strcmp((*iter)->getValue(), "INT"))
				fieldTypes.push_back(INT);
		}
	}
	return fieldTypes;
}
std::map<std::string, std::string> getMapOfAttributes(std::vector<parseTree*>& terminals, bool& success)
{
	std::map<std::string, std::string> mapOfAttributes;
	std::vector<std::string> attributes, values;
	for (vector<parseTree*>::iterator iter = terminals.begin(); iter != terminals.end(); iter++)
	{
		if ((*iter)->getParent()->getNodeType() == ATTRIBUTENAME)
		{
			attributes.push_back((*iter)->getValue());
		}
		else if (((*iter)->getParent()->getParent() != NULL) && (*iter)->getParent()->getParent()->getNodeType() == VALUE)
		{
			values.push_back((*iter)->getValue());
		}
	}
	if (attributes.size() != values.size())
	{
		success = false;
		return mapOfAttributes;
	}
	for (int count = 0; count < attributes.size(); count++)
	{
		mapOfAttributes.insert(std::make_pair(attributes[count], values[count]));
	}
	success = true;
	return mapOfAttributes;
}

std::map<std::string, std::string> getMapOfAttributes(Relation* selectStatementOutput, MainMemory mainMemoryObj)
{
	std::map<std::string, std::string> mapOfAttributes;
	vector<string> fieldNames = selectStatementOutput->getSchema().getFieldNames();
	Block* blockPtr = mainMemoryObj.getBlock(0);
	blockPtr->clear();
	selectStatementOutput->getBlock(0, 0);
	Tuple output = blockPtr->getTuple(0);
	for (vector<string>::iterator iter = fieldNames.begin(); iter != fieldNames.end(); iter++)
	{
		Field value = output.getField(*iter);
		std::string valueString;
		if (output.getSchema().getFieldType(*iter) == INT)
			valueString = std::to_string(value.integer);
		else
			valueString = *(value.str);
		mapOfAttributes.insert(make_pair(*iter, valueString));
	}
	return mapOfAttributes;
}

bool isSimpleInsert(std::vector<parseTree*>& terminals, parseTree* selectStmt)
{
	for (vector<parseTree*>::iterator iter = terminals.begin(); iter != terminals.end(); iter++)
	{
		if ((*iter)->getParent()->getNodeType() == SELECTSTATEMENT)
		{
			selectStmt = (*iter)->getParent();
			return false;
		}
	}
	return true;
}

bool getRelationNames(std::vector<parseTree*>& terminals, std::vector<std::string>& relations)
{
	vector<parseTree*>::iterator iter;
	for (iter = terminals.begin(); iter != terminals.end(); iter++)
	{
		if (!strcmp((*iter)->getValue(), "FROM"))
		{
			iter++;
			break;
		}
	}
	int count = 0;
	while (iter != terminals.end() && strcmp((*iter)->getValue(), "WHERE"))
	{
		if (strcmp((*iter)->getValue(), ","))
			relations.push_back((*iter)->getValue());
		iter++;
	}
	if (relations.size() > 1)
		return false;
	return true;	
}

void merge(std::vector<OperandOperator*>& prefix, std::vector<OperandOperator*>& operand1, OperandOperator* operatorVal, std::vector<OperandOperator*>& operand2)
{
	std::vector<OperandOperator*>::iterator iter;
	prefix.push_back(operatorVal);
	for (iter = operand1.begin(); iter != operand1.end(); iter++)
		prefix.push_back((*iter));
	for (iter = operand2.begin(); iter != operand2.end(); iter++)
		prefix.push_back((*iter));
}

std::vector<OperandOperator*> getPrefixExpression(std::vector<parseTree*>& terminals, std::string& relationName, std::vector<parseTree*>::iterator& iter, bool& flag)
{
	std::vector<OperandOperator*> prefixExpression;
	char* initialVal = (*iter)->getValue();
	operandStack operandStackObj;
	operatorStack operatorStackObj;
	if (!strcmp(initialVal, "("))
	{
		OperandOperator* obj = new OperandOperator(std::string(initialVal), kOperandOperatorType::OPERATOR, "");
		operatorStackObj.push(obj);
	}
	else
	{
		OperandOperator* obj;
		if ((*iter)->getParent()->getNodeType() == TABLENAME)
		{
			iter += 2;
			char* attributeValue = (*iter)->getValue();
			obj = new OperandOperator(std::string(attributeValue), kOperandOperatorType::VARIABLE, std::string(initialVal));
		}
		else if((*iter)->getParent()->getNodeType() == ATTRIBUTENAME)
			obj = new OperandOperator(std::string(initialVal), kOperandOperatorType::VARIABLE, relationName);
		else if((*iter)->getParent()->getNodeType() == LITERAL || (*iter)->getParent()->getNodeType() == INTEGER)
			obj = new OperandOperator(std::string(initialVal), kOperandOperatorType::PATTERN, "");
		std::vector<OperandOperator*> v;
		v.push_back(obj);
		operandStackObj.push(v);
	}
	iter++;
	while (iter != terminals.end() && !operatorStackObj.isEmpty())
	{
		char* value = (*iter)->getValue();
		OperandOperator* obj;
		if ((!strcmp(value, "(")) || (!strcmp(value, "+")) || (!strcmp(value, "*")) || (!strcmp(value, "-")))
		{
			obj = new OperandOperator(std::string(value), kOperandOperatorType::OPERATOR, "");
			operatorStackObj.push(obj);
		}
		else if ((!strcmp(value, ")")))
		{
			OperandOperator* operatorVal = operatorStackObj.pop();
			while (strcmp(operatorVal->getName().c_str(), "("))
			{
				std::vector<OperandOperator*> operand1 = operandStackObj.pop();
				std::vector<OperandOperator*> operand2 = operandStackObj.pop();
				std::vector<OperandOperator*> prefix;
				merge(prefix, operand2, operatorVal, operand1);
				operandStackObj.push(prefix);
				operatorVal = operatorStackObj.pop();
			}
			delete operatorVal;
		}
		else
		{
			if ((*iter)->getParent()->getNodeType() == TABLENAME)
			{
				iter += 2;
				char* attributeValue = (*iter)->getValue();
				obj = new OperandOperator(std::string(attributeValue), kOperandOperatorType::VARIABLE, std::string(value));
			}
			else if ((*iter)->getParent()->getNodeType() == ATTRIBUTENAME)
				obj = new OperandOperator(std::string(value), kOperandOperatorType::VARIABLE, relationName);
			else if ((*iter)->getParent()->getNodeType() == LITERAL || (*iter)->getParent()->getNodeType() == INTEGER)
				obj = new OperandOperator(std::string(value), kOperandOperatorType::PATTERN, "");
			std::vector<OperandOperator*> v;
			v.push_back(obj);
			operandStackObj.push(v);
		}
		iter++;
	}
	std::vector<OperandOperator*> finalExpression = operandStackObj.pop();
	if (operatorStackObj.isEmpty())
		iter--;
	if (!(operandStackObj.isEmpty()))
		flag = false;
	else
		flag = true;
	return finalExpression;
}

std::vector<std::vector<JoinCondition*>> getJoinConditions(std::string& relationName, std::vector<parseTree*>& terminals, bool& flag)
{
	std::vector<std::vector<JoinCondition*>> joinConditions;
	vector<parseTree*>::iterator iter = terminals.begin();
	while (iter != terminals.end() && strcmp((*iter)->getValue(), "WHERE"))
	{
		iter++;
	}
	if (iter != terminals.end())
	{
		iter++;
		JoinCondition* currentJoinCondition;
		std::vector<JoinCondition*> currentAndConditionVector;
		std::vector<OperandOperator*> currentOperandExpression;
		while (iter != terminals.end() && strcmp((*iter)->getValue(), "ORDER BY"))
		{
			if ((!strcmp((*iter)->getValue(), "<")) || (!strcmp((*iter)->getValue(), ">")) || (!strcmp((*iter)->getValue(), "=")))
			{
				bool retVal = false;
				std::string operatorValueCurrent((*iter)->getValue());
				iter++;
				std::vector<OperandOperator*> operandExpression2 = getPrefixExpression(terminals, relationName, iter, retVal);
				if (!retVal)
				{
					flag = false;
					return joinConditions;
				}
				std::vector<OperandOperator*> temp = currentOperandExpression;
				currentJoinCondition = new JoinCondition(temp, operatorValueCurrent, operandExpression2);
				currentOperandExpression.clear();
			}
			else if ((!strcmp((*iter)->getValue(), "AND")))
			{
				currentAndConditionVector.push_back(currentJoinCondition);
				currentJoinCondition = NULL;
			}
			else if ((!strcmp((*iter)->getValue(), "OR")))
			{
				currentAndConditionVector.push_back(currentJoinCondition);
				currentJoinCondition = NULL;
				std::vector<JoinCondition*> temp = currentAndConditionVector;
				joinConditions.push_back(temp);
				currentAndConditionVector.clear();
			}
			else
			{
				bool retVal = false;
				currentOperandExpression = getPrefixExpression(terminals, relationName, iter, retVal);
				if (!retVal)
				{
					flag = false;
					return joinConditions;
				}
			}
			iter++;
		}
		currentAndConditionVector.push_back(currentJoinCondition);
		joinConditions.push_back(currentAndConditionVector);
	}
	flag = true;
	return joinConditions;
}

vector<OperandOperator*> getProjectionList(std::string relationName, std::vector<parseTree*>& terminals)
{
	vector<OperandOperator*> projectionAttributes;
	vector<parseTree*>::iterator iter = terminals.begin();
	iter++;
	if (!strcmp((*iter)->getValue(), "*"))
		return projectionAttributes;
	while (iter != terminals.end() && strcmp((*iter)->getValue(), "FROM"))
	{
		OperandOperator* obj;
		char* value = (*iter)->getValue();
		if ((*iter)->getParent()->getNodeType() == TABLENAME)
		{
			iter += 2;
			char* attributeValue = (*iter)->getValue();
			obj = new OperandOperator(std::string(attributeValue), kOperandOperatorType::VARIABLE, std::string(value));
			projectionAttributes.push_back(obj);
		}
		else if ((*iter)->getParent()->getNodeType() == ATTRIBUTENAME)
		{
			obj = new OperandOperator(std::string(value), kOperandOperatorType::VARIABLE, relationName);
			projectionAttributes.push_back(obj);
		}		
		iter++;
	}
	return projectionAttributes;
}

std::vector<OperandOperator*> getOrderAttributesList(std::string relationName, std::vector<parseTree*>& terminals)
{
	vector<OperandOperator*> orderAttributes;
	vector<parseTree*>::iterator iter = terminals.begin();
	while (iter != terminals.end() && strcmp((*iter)->getValue(), "ORDER BY"))
	{
		iter++;
	}
	while (iter != terminals.end())
	{
		OperandOperator* obj;
		char* value = (*iter)->getValue();
		if ((*iter)->getParent()->getNodeType() == TABLENAME)
		{
			iter += 2;
			char* attributeValue = (*iter)->getValue();
			obj = new OperandOperator(std::string(attributeValue), kOperandOperatorType::VARIABLE, std::string(value));
			orderAttributes.push_back(obj);
		}
		else if ((*iter)->getParent()->getNodeType() == ATTRIBUTENAME)
		{
			obj = new OperandOperator(std::string(value), kOperandOperatorType::VARIABLE, relationName);
			orderAttributes.push_back(obj);
		}
		iter++;
	}
	return orderAttributes;
}


std::vector<OperandOperator*> mergeVectors(std::vector<OperandOperator*>& projectionList, std::vector<OperandOperator*>& orderAttributes)
{
	std::vector<OperandOperator*> finalList = projectionList;
	for (std::vector<OperandOperator*>::iterator iter = orderAttributes.begin(); iter != orderAttributes.end(); iter++)
	{
		bool present = false;
		for (std::vector<OperandOperator*>::iterator iter2 = projectionList.begin(); iter2 != projectionList.end(); iter2++)
		{
			if (!strcmp((*iter)->getName().c_str(), (*iter2)->getName().c_str()) && !strcmp((*iter)->getTableName().c_str(), (*iter2)->getTableName().c_str()))
				present = true;
		}
		if (!present)
			finalList.push_back(*iter);
	}
	return finalList;
}

std::vector<std::string> getAttributeNames(std::vector<OperandOperator*>& attributesOperands)
{
	std::vector<std::string> attributeNames;
	for (std::vector<OperandOperator*>::iterator iter = attributesOperands.begin(); iter != attributesOperands.end(); iter++)
		attributeNames.push_back((*iter)->getName());
	return attributeNames;
}

std::vector<std::vector<JoinCondition*>> getRelevantConditions(vector<vector<JoinCondition*>>& selectConditions, string relationName)
{
	vector<vector<JoinCondition*>> relevantConditions;
	for (vector<vector<JoinCondition*>>::iterator iter = selectConditions.begin(); iter != selectConditions.end(); iter++)
	{
		vector<JoinCondition*> andConditions;
		for (vector<JoinCondition*>::iterator iteriter = (*iter).begin(); iteriter != (*iter).end(); iteriter++)
		{
			bool flag1 = true, flag2 = true;
			vector<OperandOperator*> operand1 = (*iteriter)->getOperand1();
			vector<OperandOperator*> operand2 = (*iteriter)->getOperand2();
			for (vector<OperandOperator*>::iterator opIter = operand1.begin(); opIter != operand1.end(); opIter++)
			{
				if ((*opIter)->getType() == kOperandOperatorType::VARIABLE && strcmp((*opIter)->getTableName().c_str(), relationName.c_str()))
				{
					flag1 = false;
					break;
				}
			}
			for (vector<OperandOperator*>::iterator opIter = operand2.begin(); opIter != operand2.end(); opIter++)
			{
				if ((*opIter)->getType() == kOperandOperatorType::VARIABLE && strcmp((*opIter)->getTableName().c_str(), relationName.c_str()))
				{
					flag2 = false;
					break;
				}
			}
			if (flag1 && flag2)
				andConditions.push_back(*iteriter);
		}
		if (!andConditions.empty())
			relevantConditions.push_back(andConditions);
	}
	return relevantConditions;
}


std::vector<std::vector<JoinCondition*>> getRelevantJoinConditions(vector<vector<JoinCondition*>>& selectConditions, vector<string>& relationNames1, string relationName2)
{
	vector<vector<JoinCondition*>> relevantJoinConditions;
	for (vector<vector<JoinCondition*>>::iterator iter = selectConditions.begin(); iter != selectConditions.end(); iter++)
	{
		vector<JoinCondition*> andConditions;
		for (vector<JoinCondition*>::iterator iteriter = (*iter).begin(); iteriter != (*iter).end(); iteriter++)
		{
			bool relationFound1=false,relationFound2 = false;;
			vector<OperandOperator*> operand1 = (*iteriter)->getOperand1();
			vector<OperandOperator*> operand2 = (*iteriter)->getOperand2();
			OperandOperator* val;
			for (vector<OperandOperator*>::iterator opIter = operand1.begin(); opIter != operand1.end(); opIter++)
			{
				if ((*opIter)->getType() == kOperandOperatorType::VARIABLE)
				{
					if(!strcmp((*opIter)->getTableName().c_str(), relationName2.c_str()))
						relationFound2 = true;
					else
					{
						if (!relationFound1)
						{
							for (vector<string>::iterator relIter = relationNames1.begin(); relIter != relationNames1.end(); relIter++)
							{
								if (!strcmp((*opIter)->getTableName().c_str(), (*relIter).c_str()))
								{
									relationFound1 = true;
									break;
								}
							}
						}
					}
				}
			}
			for (vector<OperandOperator*>::iterator opIter = operand2.begin(); opIter != operand2.end(); opIter++)
			{
				if (!strcmp((*opIter)->getTableName().c_str(), relationName2.c_str()))
					relationFound2 = true;
				else
				{
					if (!relationFound1)
					{
						for (vector<string>::iterator relIter = relationNames1.begin(); relIter != relationNames1.end(); relIter++)
						{
							if (!strcmp((*opIter)->getTableName().c_str(), (*relIter).c_str()))
							{
								relationFound1 = true;
								break;
							}
						}
					}
				}
			}
			if (relationFound1 && relationFound2)
				andConditions.push_back(*iteriter);
		}
		if (!andConditions.empty())
			relevantJoinConditions.push_back(andConditions);
	}
	return relevantJoinConditions;
}

bool distictStatementPresent(std::vector<parseTree*>& terminals)
{
	for (std::vector<parseTree*>::iterator iter = terminals.begin(); iter != terminals.end(); iter++)
	{
		if (!strcmp((*iter)->getValue(), "DISTINCT"))
			return true;
	}
	return false;
}

void renameAttributes(vector<OperandOperator*>& attributeList)
{
	for (vector<OperandOperator*>::iterator iter = attributeList.begin(); iter != attributeList.end(); iter++)
		(*iter)->setName((*iter)->getTableName() + "." + (*iter)->getName());
}

void renameConditions(vector<vector<JoinCondition*>>& conditions)
{
	for (vector<vector<JoinCondition*>>::iterator iter = conditions.begin(); iter != conditions.end(); iter++)
	{
		vector<JoinCondition*> andConditions;
		for (vector<JoinCondition*>::iterator iteriter = (*iter).begin(); iteriter != (*iter).end(); iteriter++)
		{
			bool relationFound1 = false, relationFound2 = false;;
			vector<OperandOperator*> operand1 = (*iteriter)->getOperand1();
			vector<OperandOperator*> operand2 = (*iteriter)->getOperand2();
			OperandOperator* val;
			for (vector<OperandOperator*>::iterator opIter = operand1.begin(); opIter != operand1.end(); opIter++)
			{
				if ((*opIter)->getType() == kOperandOperatorType::VARIABLE)
					(*opIter)->setName((*opIter)->getTableName() + "." + (*opIter)->getName());
			}
			for (vector<OperandOperator*>::iterator opIter = operand2.begin(); opIter != operand2.end(); opIter++)
			{
				if ((*opIter)->getType() == kOperandOperatorType::VARIABLE)
					(*opIter)->setName((*opIter)->getTableName() + "." + (*opIter)->getName());
			}
		}
	}
}

void executeQuery(SchemaManager& schemaManagerObj, MainMemory& mainMemoryObj, Disk& diskObj, statement* statementObj, std::ofstream& fWriteExecute, Relation* selectStmtOutput, bool returnOutPut)
{
	parseTree* pTree = statementObj->getRoot();
	executeQuery(schemaManagerObj, mainMemoryObj, diskObj, pTree, fWriteExecute, selectStmtOutput, returnOutPut);
}

void executeQuery(SchemaManager& schemaManagerObj, MainMemory& mainMemoryObj, Disk& diskObj, parseTree* pTree, std::ofstream& fWriteExecute, Relation* selectStmtOutput, bool returnOutPut)
{
	std::vector<parseTree*> terminals;
	getTerminals(pTree, terminals);
	if (pTree->getNodeType() == CREATETABLESTATEMENT)
	{
		std::string relation_name = getSingleRelationName(terminals);
		std::vector<std::string> field_names = getFieldNames(terminals);
		std::vector<enum FIELD_TYPE> field_types = getFieldTypes(terminals);
		Relation * relationObj = createTable(schemaManagerObj, relation_name, field_names, field_types);
		if (!relationObj)
			fWriteExecute << "Execute of create failed\n\n\n\n\n";
		else
			fWriteExecute << "Create was successful\n\n" << schemaManagerObj << "\n\n\n\n\n";
	}

	else if (pTree->getNodeType() == DROPTABLESTATEMENT)
	{
		std::string relation_name = getSingleRelationName(terminals);
		bool retVal = dropTable(relation_name, fWriteExecute, schemaManagerObj);
		if (!retVal)
			fWriteExecute << "Execute of drop failed\n\n\n\n\n";
		else
			fWriteExecute << "Drop succeeded\n\n\n\n\n";
	}

	else if (pTree->getNodeType() == INSERTSTATEMENT)
	{
		std::string relation_name = getSingleRelationName(terminals);
		bool success = false;
		std::map<std::string, std::string> mapOfAttributes;
		parseTree* selectStmtNode=NULL;
		if (isSimpleInsert(terminals, selectStmtNode))
		{
			mapOfAttributes = getMapOfAttributes(terminals, success);
			if (!success)
			{
				fWriteExecute << "Execute of insert failed\n\n\n\n\n";
				return;
			}
		}
		else
		{
			Relation* selectStatementOutput=NULL;
			executeQuery(schemaManagerObj, mainMemoryObj, diskObj, selectStmtNode, fWriteExecute, selectStatementOutput, true);
			if (!selectStatementOutput)
			{
				fWriteExecute << "Execute of insert failed\n\n\n\n\n";
				return;
			}
			if (selectStatementOutput->getNumOfTuples() > 1)
			{
				fWriteExecute << "Execute of insert failed\n\n\n\n\n";
				return;
			}
			mapOfAttributes = getMapOfAttributes(selectStatementOutput, mainMemoryObj);
		}
		bool retVal = insertTable(relation_name, schemaManagerObj, mapOfAttributes, mainMemoryObj);
		if (!retVal)
			fWriteExecute << "Execute of insert failed\n\n\n\n\n";
		else
			fWriteExecute << "Insert succeeded\n\n" << schemaManagerObj << "\n\n\n\n\n";
	}

	else if (pTree->getNodeType() == SELECTSTATEMENT)
	{
		std::vector<std::string> tableNames;
		if (getRelationNames(terminals, tableNames))
		{
			std::string relation_name = tableNames[0];
			bool checkConditions = false;
			std::vector<std::vector<JoinCondition*>> joinConditionList = getJoinConditions(relation_name, terminals, checkConditions);
			Relation* selectWhereRelation;
			if (!checkScopeOfAttributes(joinConditionList, tableNames))
			{
				fWriteExecute << "Execute of select failed\n\n\n\n\n";
				return;
			}
			if (checkConditions)
			{
				Relation* intermediateRelation;
				vector<OperandOperator*> projectionList = getProjectionList(relation_name, terminals);
				std::vector<OperandOperator*> orderAttributes = getOrderAttributesList(relation_name, terminals);
				std::vector<OperandOperator*> finalProjectionAttributes = mergeVectors(projectionList, orderAttributes);
				selectWhereRelation = selectTable(relation_name, schemaManagerObj, joinConditionList, mainMemoryObj, finalProjectionAttributes, false);
				if (!selectWhereRelation)
				{
					fWriteExecute << "Execute of select failed\n\n\n\n\n";
					return;
				}
				intermediateRelation = selectWhereRelation;
				std::vector<std::string> projectionListString;
				if (finalProjectionAttributes.empty())
				{
					projectionListString = selectWhereRelation->getSchema().getFieldNames();
					for (vector<string>::iterator iter = projectionListString.begin(); iter != projectionListString.end(); iter++)
					{
						OperandOperator* obj = new OperandOperator((*iter), kOperandOperatorType::VARIABLE, "");
						finalProjectionAttributes.push_back(obj);
					}
				}
				else
				{
					projectionListString = getAttributeNames(finalProjectionAttributes);
				}

				if (distictStatementPresent(terminals))
				{
					if (doesFitInMemory(selectWhereRelation, NUM_OF_BLOCKS_IN_MEMORY - 1))
					{
						intermediateRelation = onePassDistinct(selectWhereRelation, mainMemoryObj, schemaManagerObj, projectionList);
						if (!intermediateRelation)
						{
							fWriteExecute << "Execute of select distict failed\n\n\n\n\n";
							return;
						}

					}

					else
					{
						bool successSort = false;
						vector<Relation*> sortedSubLists = sortSubList(selectWhereRelation, schemaManagerObj, NUM_OF_BLOCKS_IN_MEMORY - 1, finalProjectionAttributes, mainMemoryObj, successSort);
						for (int i = 0; i < sortedSubLists.size(); i++)
						{
							std::cout << "\n\n\n\n\n";
							sortedSubLists[i]->printRelation();
						}
						if (!successSort)
						{
							fWriteExecute << "Execute of select failed\n\n\n\n\n";
							return;
						}
						Relation* distictRelation = NULL;
						int countOfSubLists = 0;
						vector<Relation*> subListsSet;
						while (countOfSubLists < sortedSubLists.size())
						{
							int i;
							for (i = countOfSubLists; i < min(sortedSubLists.size(), countOfSubLists + (NUM_OF_BLOCKS_IN_MEMORY - 1)); i++)
							{
								subListsSet.push_back(sortedSubLists[i]);
							}
							distictRelation = removeDuplicatesOperation(subListsSet, schemaManagerObj, mainMemoryObj, projectionListString);
							if (!distictRelation)
							{
								fWriteExecute << "Execute of select distict failed\n\n\n\n\n";
								return;
							}
							subListsSet.clear();
							subListsSet.push_back(distictRelation);
							countOfSubLists += i;
						}
						intermediateRelation = distictRelation;
					}
				}

				if (!(orderAttributes.empty()))
				{
					if (doesFitInMemory(intermediateRelation, NUM_OF_BLOCKS_IN_MEMORY - 1))
					{
						Relation* orderedRelation = onePassOrdering(intermediateRelation, mainMemoryObj, schemaManagerObj, orderAttributes);
						if (!orderedRelation)
						{
							fWriteExecute << "Execute of order by failed\n\n\n\n\n";
							return;
						}
						intermediateRelation = orderedRelation;
					}
					else
					{
						bool success = false;
						vector<Relation*> sortedSubLists = sortSubList(intermediateRelation, schemaManagerObj, NUM_OF_BLOCKS_IN_MEMORY - 1, orderAttributes, mainMemoryObj, success);
						if (!success)
						{
							fWriteExecute << "Execute of select failed\n\n\n\n\n";
							return;
						}

						std::vector<std::string> orderAttributeString = getAttributeNames(orderAttributes);
						Relation* orderedRelation = NULL;
						int countOfSubLists = 0;
						vector<Relation*> subListsSet;
						while (countOfSubLists < sortedSubLists.size())
						{
							int i;
							for (i = countOfSubLists; i < min(sortedSubLists.size(), countOfSubLists + (NUM_OF_BLOCKS_IN_MEMORY - 1)); i++)
							{
								subListsSet.push_back(sortedSubLists[i]);
							}
							orderedRelation = sortOperation(sortedSubLists, schemaManagerObj, mainMemoryObj, orderAttributeString);
							if (!orderedRelation)
							{
								fWriteExecute << "Execute of order by failed\n\n\n\n\n";
								return;
							}
							subListsSet.clear();
							subListsSet.push_back(orderedRelation);
							countOfSubLists += i;
						}
						intermediateRelation = orderedRelation;
					}
				}
				joinConditionList.clear();
				Relation* finalRelation = selectTable(intermediateRelation->getRelationName(), schemaManagerObj, joinConditionList, mainMemoryObj, projectionList, false);
				if (!finalRelation)
				{
					fWriteExecute << "Execute of select failed\n\n\n\n\n";
					return;
				}
				if (!returnOutPut)
				{
					fWriteExecute << "Execute of select succeeded\n\n\n\n\n";
					std::cout << "\n\n\n\n\n\n";
					finalRelation->printRelation();
					std::cout << "\n\n\n\n\n";
				}
				else
					selectStmtOutput = finalRelation;
			}
		}


		else
		{
			vector<Relation*> relationsToBeMerged;
			bool conditionListPopulated = false;
			std::vector<std::vector<JoinCondition*>> selectConditions = getJoinConditions(string(""), terminals, conditionListPopulated);
			if (!checkScopeOfAttributes(selectConditions, tableNames))
			{
				fWriteExecute << "\n\n\nExecution of select failed\n\n\n";
				return;
			}
			if (!conditionListPopulated)
			{
				fWriteExecute << "\n\n\nExecution of select failed\n\n\n";
				return;
			}
			std::vector<OperandOperator*> actualProjectionList = getProjectionList(string(""), terminals);
			std::vector<OperandOperator*> actualOrderedList = getOrderAttributesList(string(""), terminals);
			for (vector<string>::iterator relationIter = tableNames.begin(); relationIter != tableNames.end(); relationIter++)
			{
				std::vector<std::vector<JoinCondition*>> subsetConditions;
				if (selectConditions.size()<2)
					subsetConditions = getRelevantConditions(selectConditions, (*relationIter));
				std::vector<OperandOperator*> intermediateProjectionList;
				Relation* intermediateObj = selectTable((*relationIter), schemaManagerObj, subsetConditions, mainMemoryObj, intermediateProjectionList, true);
				relationsToBeMerged.push_back(intermediateObj);
			}
			renameAttributes(actualOrderedList);
			renameAttributes(actualProjectionList);
			renameConditions(selectConditions);
			vector<Relation*> currentTables;
			int tablesCoveredSoFar = 1;
			currentTables.push_back(relationsToBeMerged[0]);
			vector<string> tableNamesAlreadyJoined;
			tableNamesAlreadyJoined.push_back(tableNames[tablesCoveredSoFar - 1]);
			double timer, io;
			for (vector<Relation*>::iterator tableIter = relationsToBeMerged.begin() + 1; tableIter != relationsToBeMerged.end(); tableIter++)
			{
				currentTables.push_back(*tableIter);
				vector<vector<JoinCondition*>> curJoinConditions;
				if (selectConditions.size() < 2)
					curJoinConditions = getRelevantJoinConditions(selectConditions, tableNamesAlreadyJoined, tableNames[tablesCoveredSoFar]);
				else
					curJoinConditions = selectConditions;
				diskObj.resetDiskTimer();
				diskObj.resetDiskIOs();
				Relation* crossJoin = cartesianProductOnePass(currentTables, schemaManagerObj, mainMemoryObj, curJoinConditions);
				timer = diskObj.getDiskTimer();
				io = diskObj.getDiskIOs();
				currentTables.clear();
				currentTables.push_back(crossJoin);
				tablesCoveredSoFar++;
				tableNamesAlreadyJoined.push_back(tableNames[tablesCoveredSoFar - 1]);
			}
			Relation* joinedRelation = currentTables[0];
			vector<string> projectionListString;
			if (actualProjectionList.empty())
			{
				projectionListString = joinedRelation->getSchema().getFieldNames();
				for (vector<string>::iterator iter = projectionListString.begin(); iter != projectionListString.end(); iter++)
				{
					OperandOperator* obj = new OperandOperator((*iter), kOperandOperatorType::VARIABLE, "");
					actualProjectionList.push_back(obj);
				}
			}
			else
			{
				projectionListString = getAttributeNames(actualProjectionList);
			}
			if (distictStatementPresent(terminals))
			{
				if (doesFitInMemory(joinedRelation, NUM_OF_BLOCKS_IN_MEMORY - 1))
				{
					Relation* distinctRelation = onePassDistinct(joinedRelation, mainMemoryObj, schemaManagerObj, actualProjectionList);
					if (!distinctRelation)
					{
						fWriteExecute << "Execute of select distict failed\n\n\n\n\n";
						return;
					}
					joinedRelation = distinctRelation;
				}
				else
				{
					bool successSort = false;
					vector<Relation*> sortedSubLists = sortSubList(joinedRelation, schemaManagerObj, NUM_OF_BLOCKS_IN_MEMORY - 1, actualProjectionList, mainMemoryObj, successSort);
					if (!successSort)
					{
						fWriteExecute << "Execute of select failed\n\n\n\n\n";
						return;
					}

					Relation* distictRelation = NULL;
					int countOfSubLists = 0;
					vector<Relation*> subListsSet;
					while (countOfSubLists < sortedSubLists.size())
					{
						int i;
						for (i = countOfSubLists; i < min(sortedSubLists.size(), countOfSubLists + (NUM_OF_BLOCKS_IN_MEMORY - 1)); i++)
						{
							subListsSet.push_back(sortedSubLists[i]);
						}
						distictRelation = removeDuplicatesOperation(subListsSet, schemaManagerObj, mainMemoryObj, projectionListString);
						if (!distictRelation)
						{
							fWriteExecute << "Execute of select distict failed\n\n\n\n\n";
							return;
						}
						subListsSet.clear();
						subListsSet.push_back(distictRelation);
						countOfSubLists += i;
					}
					joinedRelation = distictRelation;
				}
			}

			if (!(actualOrderedList.empty()))
			{
				if (doesFitInMemory(joinedRelation, NUM_OF_BLOCKS_IN_MEMORY - 1))
				{
					Relation* orderedRelation = onePassOrdering(joinedRelation, mainMemoryObj, schemaManagerObj, actualOrderedList);
					if (!orderedRelation)
					{
						fWriteExecute << "Execute of order by failed\n\n\n\n\n";
						return;
					}
					joinedRelation = orderedRelation;
				}
				else
				{
					bool success = false;
					vector<Relation*> sortedSubLists = sortSubList(joinedRelation, schemaManagerObj, NUM_OF_BLOCKS_IN_MEMORY - 1, actualOrderedList, mainMemoryObj, success);
					if (!success)
					{
						fWriteExecute << "Execute of select failed\n\n\n\n\n";
						return;
					}

					std::vector<std::string> orderAttributeString = getAttributeNames(actualOrderedList);
					Relation* orderedRelation = NULL;
					int countOfSubLists = 0;
					vector<Relation*> subListsSet;
					while (countOfSubLists < sortedSubLists.size())
					{
						int i;
						for (i = countOfSubLists; i < min(sortedSubLists.size(), countOfSubLists + (NUM_OF_BLOCKS_IN_MEMORY - 1)); i++)
						{
							subListsSet.push_back(sortedSubLists[i]);
						}
						orderedRelation = sortOperation(sortedSubLists, schemaManagerObj, mainMemoryObj, orderAttributeString);
						if (!orderedRelation)
						{
							fWriteExecute << "Execute of order by failed\n\n\n\n\n";
							return;
						}
						subListsSet.clear();
						subListsSet.push_back(orderedRelation);
						countOfSubLists += i;
					}
					joinedRelation = orderedRelation;
				}
			}
			selectConditions.clear();
			Relation* finalRelation = selectTable(joinedRelation->getRelationName(), schemaManagerObj, selectConditions, mainMemoryObj, actualProjectionList, false);
			if (!finalRelation)
			{
				fWriteExecute << "Execute of select failed\n\n\n\n\n";
				return;
			}
			if (!returnOutPut)
			{
				fWriteExecute << "Execute of select succeeded\n\n\n\n\n";
				std::cout << "\n\n\n\n\n\n";
				finalRelation->printRelation();
				std::cout << "\n\n\n\n\n";
				std::cout << "Timer: " << timer << "\nIOs: " << io << "\n\n";
			}
			else
				selectStmtOutput = finalRelation;
		}
	}

	else if (pTree->getNodeType() == DELETESTATEMENT)
	{
		std::vector<std::string> tableNames;
		getRelationNames(terminals, tableNames);
		std::string relation_name = tableNames[0];
		bool checkConditions = false;
		std::vector<std::vector<JoinCondition*>> joinConditionList = getJoinConditions(relation_name, terminals, checkConditions);
		if (checkConditions)
		{
			bool retVal = deleteTable(relation_name, schemaManagerObj, joinConditionList, mainMemoryObj);
			if (!retVal)
				fWriteExecute << "Execute of delete failed\n\n\n\n\n";
			else
				fWriteExecute << "Delete succeeded\n\n\n\n\n";
		}
		else
			fWriteExecute << "Execute of delete failed\n\n\n\n\n";
	}
}