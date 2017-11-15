#include<iostream>
#include<fstream>
#include<iterator>
#include<cstdlib>
#include<string>
#include<ctime>
#include <stack>
#include "StorageManager\Block.h"
#include "StorageManager\Config.h"
#include "StorageManager\Disk.h"
#include "StorageManager\Field.h"
#include "StorageManager\MainMemory.h"
#include "StorageManager\Relation.h"
#include "StorageManager\Schema.h"
#include "StorageManager\SchemaManager.h"
#include "StorageManager\Tuple.h"
#include "JoinCondition.h"


Relation * createTable(SchemaManager& schema_manager, string relation_name, vector<string> field_names, vector<enum FIELD_TYPE> field_types);

void appendTupleToRelation(Relation* relation_ptr, MainMemory& mem, int memory_block_index, Tuple& tuple);

bool insertTable(string tableName, SchemaManager& schema_manager, std::map<string, string> map_of_attributes, MainMemory& mem);

bool dropTable(string table_name, ofstream &file_output, SchemaManager &schema_manager);

void verifySchema(Schema schema, vector<vector<JoinCondition>> &listOflistOfJoinConditions, string table_name);

int getValueFromConversionOfPrefixToInfix(vector<OperandOperator *> vectorOfOperands, Tuple& tuple);

bool checkIfTupleSatisfiesConditions(Tuple& tuple, Schema& schema, vector<vector<JoinCondition>> &listOflistOfJoinConditions);

bool selectTable(string table_name, SchemaManager &schema_manager, vector<vector<JoinCondition>> &listOfJoinConditions, MainMemory& mem);











