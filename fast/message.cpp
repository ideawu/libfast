#include <algorithm>
#include "message.h"

namespace fast{

Message::Message(){
}

Message::~Message(){
}

void Message::add_encoded(int index, const std::string &str){
	if(index < 0){
		index = (int)fields_.size();
	}
	if(fields_.size() < index + 1){
		fields_.resize(index + 1);
	}
	fields_[index] = str;
}

void Message::add(int64_t val, int index){
	std::string bin((char *)&val, sizeof(val));
	this->add_encoded(index, bin);
}

void Message::add(const char *val, int index){
	std::string bin(val);
	if(bin.empty()){ // 空字符串也要占用一个字节
		bin.push_back('\0');
	}
	this->add_encoded(index, bin);
}

static std::string encode_field_val(const Field *field, const std::string &bin){
	std::string buffer;
	switch(field->type){
		case Field::Int:
		case Field::Uint:{
			if(bin.size() == sizeof(int64_t)){
				int64_t num = *((int64_t *)bin.data());
				//printf("num: %d\n", (int)num);
				if(field->type == Field::Int){
					// if nullable & positive
					if(field->pr == Field::Mandatory && field->op == Field::Noop && num >= 0){
						num += 1;
					}
					buffer.append(encode_val(num));
				}else{
					buffer.append(encode_val((uint64_t)num));
				}
			}else{
				// TODO: report error!
				buffer.append(encode_val(0));
			}
			break;
		}
		case Field::String:{
			buffer.append(encode_val(bin));
			break;
		}
	}
	return buffer;
}

std::string Message::encode(const Template &temp){
	std::string buffer;
	buffer.push_back('\0');
	
	// build pmap
	int offset = 1;
	for(int i=0; i<temp.size(); i++){
		if(i >= fields_.size()){
			break;
		}
		const Field *field = temp.get_field(i);
		if(field->pr == Field::Mandatory && field->op == Field::Constant){
			continue;
		}
		if(field->pr == Field::Mandatory && field->op == Field::Noop){
			continue;
		}
		if(field->pr == Field::Optional && field->op == Field::Noop){
			continue;
		}

		const std::string &val = fields_[i];
		if(val.size() > 0){
			// set bit
			int byte_offset = offset / 7;
			int bit_offset = 7 - offset % 7 - 1;
			if(byte_offset >= buffer.size()){
				buffer.push_back('\0');
			}
			buffer[byte_offset] |= (1 << bit_offset);
			//printf("%d# %d:%d = 1, %02x\n", i, byte_offset, bit_offset, buffer[byte_offset]);

			offset ++;
		}
	}
	buffer[buffer.size() - 1] |= STOP_BIT;
	printf("pmap.size: %d\n", (int)buffer.size());
	
	// template_id is set
	buffer[0] |= (1 << 6);
	buffer.append(encode_val(temp.id));

	// add fields
	for(int i=0; i<temp.size(); i++){
		std::string bin;
		if(i < fields_.size()){
			bin = fields_[i];
		}
		const Field *field = temp.get_field(i);
		if(field->pr == Field::Mandatory && field->op == Field::Constant){
			continue;
		}

		if(field->pr == Field::Optional && field->op == Field::Constant){
			if(!bin.empty()){
				buffer.append(encode_field_val(field, bin));
			}
		}
		if(field->pr == Field::Mandatory){
			buffer.append(encode_field_val(field, bin));
		}
		if(field->pr == Field::Optional && field->op == Field::Noop){
			buffer.append(encode_field_val(field, bin));
		}
	}

	return buffer;
}

}; // end namespace fast
