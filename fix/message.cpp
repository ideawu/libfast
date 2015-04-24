#include "../util/strings.h"
#include "message.h"

namespace fix{

std::string Message::default_version = std::string("FIX.4.2");

void Message::set_default_version(const std::string &ver){
	Message::default_version = ver;
}

Message::Message(){
}

Message::~Message(){
}

void Message::set(int tag, int64_t val){
	fields[tag] = str(val);
}

void Message::set(int tag, const char *val){
	fields[tag] = str(val);
}

const std::string* Message::get(int tag) const{
	std::map<int, std::string>::const_iterator it;
	it = fields.find(tag);
	if(it == fields.end()){
		return NULL;
	}
	return &(it->second);
}

static std::string encode_field(int tag, const std::string &val){
	std::string buffer;
	buffer.append(str(tag));
	buffer.push_back('=');
	buffer.append(val);
	buffer.push_back('\x01');
	return buffer;
}

std::string Message::encode(){
	std::string buffer;
	std::map<int, std::string>::const_iterator it;
	for(it=fields.begin(); it!=fields.end(); it++){
		int tag = it->first;
		if(tag == 8 || tag == 9 || tag == 10){
			continue;
		}
		const std::string &val = it->second;
		buffer.append(encode_field(tag, val));
	}
	body_len_ = (int)buffer.size();

	std::string header;
	header.append(encode_field(8, Message::default_version));
	header.append(encode_field(9, str(body_len_)));
	
	checksum_ = 0;
	for(int i=0; i<(int)header.size(); i++){
		unsigned char byte = (unsigned char)header[i];
		checksum_ += byte;
	}
	for(int i=0; i<(int)buffer.size(); i++){
		unsigned char byte = (unsigned char)buffer[i];
		checksum_ += byte;
	}
	checksum_ %= 256;
	
	char buf[4];
	snprintf(buf, sizeof(buf), "%03d", checksum_);
	buffer.append(encode_field(10, buf));
	
	return header + buffer;
}

}; // end namespace fix
