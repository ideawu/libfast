#include "field.h"

namespace fast{

Field::Field(){
}

Field::Field(int tag, Type type, Presence pr, Operator op){
	this->tag = tag;
	this->type = type;
	this->pr = pr;
	this->op = op;
}

}; // end namespace fast
