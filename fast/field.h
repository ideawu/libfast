#ifndef FAST_FIELD_H_
#define FAST_FIELD_H_

#include <string>

namespace fast{

class Field
{
public:
	typedef enum{
		NullPr    = 0,
		Mandatory = 1,
		Optional  = 2
	}Presence;

	typedef enum{
		Noop      = 0,
		Constant  = 1,
		Copy      = 2,
		Delta     = 3,
		Increment = 4
	}Operator;

	typedef enum{
		Int       = 0,
		Uint      = 1,
		String    = 2
	}Type;

public:
	int index;
	Type type;
	Presence pr;
	Operator op;
	int tag;
	std::string init_val;
	
	std::string last_val;

	Field();
	Field(int tag, Type type, Presence pr, Operator op=Noop);
};

}; // end namespace fast

#endif
