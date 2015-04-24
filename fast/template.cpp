#include "template.h"

namespace fast{

Template::Template(int id){
	this->id = id;
}

void Template::add_field(const Field &field){
	Field f = field;
	f.index = (int)fields_.size();
	fields_.push_back(f);
}

const Field* Template::get_field(int index) const{
	if(index >= 0 && index < fields_.size()){
		return &fields_[index];
	}
	return NULL;
}

const Field* Template::get_field_by_tag(int tag) const{
	for(int i=0; i<(int)fields_.size(); i++){
		const Field *f = &fields_[i];
		if(f->tag == tag){
			return f;
		}
	}
	return NULL;
}

}; // end namespace fast
