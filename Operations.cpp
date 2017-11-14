#include<iostream>
#include<fstream>
#include<iterator>
#include<cstdlib>
#include<string>
#include<ctime>
#include "StorageManager\Block.h"
#include "StorageManager\Config.h"
#include "StorageManager\Disk.h"
#include "StorageManager\Field.h"
#include "StorageManager\MainMemory.h"
#include "StorageManager\Relation.h"
#include "StorageManager\Schema.h"
#include "StorageManager\SchemaManager.h"
#include "StorageManager\Tuple.h"
#include "JoinCondition.cpp"

std::ofstream fout;

Relation * createTable(SchemaManager& schema_manager,string relation_name, vector<string> field_names, vector<enum FIELD_TYPE> field_types) {
	Schema schema(field_names, field_types);
	Relation* relation_ptr = schema_manager.createRelation(relation_name, schema);
	if (relation_ptr == NULL) {
		fout << "Table with name " << relation_name << " Has failed to be created";
	}
	return relation_ptr;
}

void appendTupleToRelation(Relation* relation_ptr, MainMemory& mem, int memory_block_index, Tuple& tuple) {
	Block* block_ptr;
	if (relation_ptr->getNumOfBlocks() == 0) {
		block_ptr = mem.getBlock(memory_block_index);
		block_ptr->clear(); 
		block_ptr->appendTuple(tuple);
		relation_ptr->setBlock(relation_ptr->getNumOfBlocks(), memory_block_index);
	}
	else {
		relation_ptr->getBlock(relation_ptr->getNumOfBlocks() - 1, memory_block_index);
		block_ptr = mem.getBlock(memory_block_index);

		if (block_ptr->isFull()) {
			block_ptr->clear();
			block_ptr->appendTuple(tuple); 
			relation_ptr->setBlock(relation_ptr->getNumOfBlocks(), memory_block_index); 
		}
		else {
			block_ptr->appendTuple(tuple); 
	
			relation_ptr->setBlock(relation_ptr->getNumOfBlocks() - 1, memory_block_index); 
		}
	}
}


inline bool isInteger(const std::string & s)
{
	if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

	char * p;
	strtol(s.c_str(), &p, 10);

	return (*p == 0);
}



bool insertTable(string tableName,SchemaManager& schema_manager, std::map<string,string> map_of_attributes, MainMemory& mem){
	Relation * relation = schema_manager.getRelation(tableName);
	try {
		Tuple t = relation->createTuple();
		Schema scheme_of_tuple = t.getSchema();
		bool result = false;
		map<string, string>::const_iterator map_iterator;
		for (map_iterator = map_of_attributes.cbegin(); map_iterator != map_of_attributes.cend(); map_iterator++) {
			string field_name = map_iterator->first;
			string field_value = map_iterator->second;
			FIELD_TYPE	 field_type = scheme_of_tuple.getFieldType(field_name);
			if (field_type == STR20)
			{
				result = t.setField(field_name, field_value);
				if (!result) {
					throw "Setting field " + field_name + " " + field_value + "Has failed";
				}
			}
			else {
				//Check if it is a integer
				result = isInteger(field_value);
				if (!result) {
					throw "Setting field " + field_name + " because " + field_value + " is not a integer";
				}
				result = t.setField(field_name, std::stoi(field_value));
				if (!result) {
					throw "Setting field " + field_name + " " + field_value + "Has failed";
				}

			}
			 appendTupleToRelation(relation, mem, 0, t);
		}

	}
	catch (char *s) {
		cerr << s;
		return false;
	}
	return true;
}

bool dropTable(string table_name, ofstream &file_output, SchemaManager &schema_manager){
	try {
		Relation * table_relation = schema_manager.getRelation(table_name);
		bool result = table_relation->deleteBlocks(0);
		if (!result) {
			throw "Deleting from the disk for table " + table_name + " has failed";
		}
		result = schema_manager.deleteRelation(table_name);
		if (!result) {
			throw "Deleting table relation " + table_name + " has failed";
		}
	}
	catch (char *s) {
		cerr << s;
		return false;
	}
	return true;
}


bool selectTable(string table_name, SchemaManager &schema_manager, vector<JoinCondition> &listOfJoinConditions, MainMemory& mem) {
	try {
		Relation *table_relation = schema_manager.getRelation(table_name);
		if (table_relation == NULL) {
			throw "Given relation with " + table_name + " Does not exist";
		}
		Schema schema = table_relation->getSchema();
		verifySchema(schema, listOfJoinConditions, table_name);
		Block* block_pointer;
		block_pointer = mem.getBlock(0);
		int numOfBlocks = table_relation->getNumOfBlocks();
		for (int i = 0; i < numOfBlocks; i++) {
			table_relation->getBlock(i, 0);
			vector<Tuple> listOfTuples = block_pointer->getTuples;
			vector<Tuple>::iterator itr;
			for (itr = listOfTuples.begin(); itr != listOfTuples.end(); itr++) {
				Tuple tuple = *itr;
				bool resultOfCheckingOnConditions = checkIfTupleSatisfiesConditions(tuple,schema,listOfJoinConditions);
				if (resultOfCheckingOnConditions) {
					tuple.printTuple();
				}
				else {

				}
			}

		}

	}
	catch (std::string s) {
		cout << s;
		return false;
	}
	return false;
}

void verifySchema(Schema schema, vector<JoinCondition> & listOfJoinConditions,string table_name) {
	vector<JoinCondition>::iterator itr;
	for (itr = listOfJoinConditions.begin(); itr != listOfJoinConditions.end(); itr++) {
		if (!schema.fieldNameExists(itr->getOperand1)) {
			throw "Field " + itr->getOperand1() + " Does not exist in " + table_name;
		}
		else if (itr->getOperand2Variable() && !schema.fieldNameExists(itr->getOperand2())) {
			throw "Field " + itr->getOperand2() + "Does not exist in " + table_name;
		}
	}
}

bool checkIfTupleSatisfiesConditions(Tuple& tuple, Schema& schema, vector<JoinCondition> & listOfJoinConditions) {
	vector<JoinCondition>::iterator itr;
	for (itr = listOfJoinConditions.begin(); itr != listOfJoinConditions.end(); itr++) {
		FIELD_TYPE fieldTypeOfOperand1 = schema.getFieldType(itr->getOperand1);
		Field Operand2;
		Field Operand1 = tuple.getField(itr->getOperand1());
		int integerValueOfOperand2 = 0;
		string stringValueOfOperand2 = NULL;
		if (!itr->getOperand2Variable) {
			if (fieldTypeOfOperand1 == INT) {
				integerValueOfOperand2 = stoi(itr->getOperand2);
			}
			else {
				stringValueOfOperand2 = itr->getOperand2;
			}
		}
		else {
			Operand2= tuple.getField(itr->getOperand2());
		}
		if (fieldTypeOfOperand1 == INT) {
			if (strcmp(itr->getOperatorOfOperation().c_str, "=") == 0) {

				if (itr->getOperand2Variable && Operand1.integer != Operand2.integer) {
					return false;
				}
				else if(!itr->getOperand2Variable && Operand1.integer!=integerValueOfOperand2)
						return false;				
		   }
			else if(strcmp(itr->getOperatorOfOperation().c_str, "<") == 0) {

				if (itr->getOperand2Variable && Operand1.integer >= Operand2.integer) {
					return false;
				}
				else if (!itr->getOperand2Variable && Operand1.integer >= integerValueOfOperand2)
					return false;
			}
			else if (strcmp(itr->getOperatorOfOperation().c_str, ">") == 0) {

				if (itr->getOperand2Variable && Operand1.integer <= Operand2.integer) {
					return false;
				}
				else if (!itr->getOperand2Variable && Operand1.integer <= integerValueOfOperand2)
					return false;
			}
				
		}
		else if (fieldTypeOfOperand1 == STR20) {
			if (strcmp(itr->getOperatorOfOperation().c_str, "=") == 0) {
				if (itr->getOperand2Variable && strcmp(Operand1.str->c_str, Operand2.str->c_str) != 0) {
					return false;
				}
				else if (!itr->getOperand2Variable && strcmp(Operand1.str->c_str, stringValueOfOperand2.c_str) != 0) {
					return false;
				}
			}
		}
	}
	return true;
}

