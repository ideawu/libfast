#ifndef FAST_MESSAGE_H_
#define FAST_MESSAGE_H_

#include <string>
#include <vector>

#include "template.h"

namespace fast{

class Message
{
public:
	Message();
	~Message();
	
	void add(int32_t val, int index=-1){
		add((int64_t)val, index);
	}
	void add(int64_t val, int index=-1);
	void add(const char *val, int index=-1);
	
	std::string encode(const Template &temp);
private:
	std::vector<std::string> fields_;
	void add_encoded(int index, const std::string &str);
};

static const unsigned char SIGN_BIT = (unsigned char)('\x40');
static const unsigned char STOP_BIT = (unsigned char)('\x80');
static const unsigned char DATA_BITS = (unsigned char)('\x7F');
static const unsigned char EMPTY_STRING = (unsigned char)('\x80');

static std::string encode_val(int64_t val){
	std::string buffer;
	int64_t until = 0LL;
	unsigned char sign = 0;
	if(val < 0){
		until = -1LL;
		sign = SIGN_BIT;
	}

	// force at least one byte to be stored
	unsigned char prevByte = ~sign;
	unsigned char byte = 0x80;
	while(val != until || (prevByte & SIGN_BIT) != sign){
		byte |= (unsigned char)(val & DATA_BITS);
		val >>= 7;
		buffer.push_back(byte);
		prevByte = byte;
		byte = 0;
	}
	std::reverse(buffer.begin(), buffer.end());
	return buffer;
}

static std::string encode_val(uint64_t val){
	std::string buffer;
	uint64_t until = 0ULL;
	unsigned char byte = 0x80;
	while(val != until || byte != 0){
		byte |= (unsigned char)(val & DATA_BITS);
		val >>= 7;
		buffer.push_back(byte);
		byte = 0;
	}
	std::reverse(buffer.begin(), buffer.end());
	return buffer;
}

static inline std::string encode_val(char val){
	return encode_val((int64_t)val);
}

static inline std::string encode_val(short val){
	return encode_val((int64_t)val);
}

static inline std::string encode_val(int32_t val){
	return encode_val((int64_t)val);
}

static std::string encode_val(const std::string &val){
	std::string enc;
	int len = (int)val.size();
	for(int pos = 0; pos < len - 1; ++pos){
		enc.push_back((unsigned char)(val[pos]));
	}
	enc.push_back(val[len - 1] | STOP_BIT);
	return enc;
}

}; // end namespace fast

#endif
