#ifndef FAST_TEMPLATE_H
#define FAST_TEMPLATE_H

#include <vector>
#include "field.h"

namespace fast{
	
class Template
{
public:
	int id;
	
	Template(int id);
	
	int size() const{
		return (int)fields_.size();
	}
	
	void add_field(const Field &field);
	const Field* get_field(int index) const;
	
private:
	std::vector<Field> fields_;
};

}; // end namespace fast

#endif
