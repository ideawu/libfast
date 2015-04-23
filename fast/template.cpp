#include "template.h"

namespace fast{

Template::Template(int id){
	this->id = id;
}

void Template::add_field(const Field &field){
	fields_.push_back(field);
}

const Field* Template::get_field(int index) const{
	if(index >= 0 && index < fields_.size()){
		return &fields_[index];
	}
	return NULL;
}

}; // end namespace fast
