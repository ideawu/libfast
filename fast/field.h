#ifndef FAST_FIELD_H_
#define FAST_FIELD_H_

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
	Type type;
	Presence pr;
	Operator op;

	Field();
	Field(Type type, Presence pr, Operator op=Noop);
};

}; // end namespace fast

#endif
