#ifndef FIX_MESSAGE_H_
#define FIX_MESSAGE_H_

#include <string>
#include <map>

namespace fix{

class Message{
public:
	static void set_default_version(const std::string &ver);
	std::map<int, std::string> fields;

	Message();
	~Message();
	
	void reset(){
		fields.clear();
	}
	void set(int tag, int32_t val){
		set(tag, (int64_t)val);
	}
	void set(int tag, int64_t val);
	void set(int tag, const char *val);
	void set(int tag, const std::string &val);
	const std::string* get(int tag) const;
	
	std::string encode();
	// call after encode()
	int body_len() const{
		return body_len_;
	}
	// call after encode()
	int checksum(){
		return checksum_;
	}
	std::string version() const{
		return Message::default_version;
	}

private:
	int body_len_;
	int checksum_;
	static std::string default_version;
};

}; // namespace fix

#endif
