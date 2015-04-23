#include "field.h"

namespace fast{

Field::Field(){
}

Field::Field(Type type, Presence pr, Operator op){
	this->type = type;
	this->pr = pr;
	this->op = op;
}

}; // end namespace fast
