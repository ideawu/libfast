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
	void set_version(const std::string version){
		_version = version;
	}
	std::string version() const{
		if(_version.empty()){
			return Message::default_version;
		}else{
			return _version;
		}
	}

private:
	int body_len_;
	int checksum_;
	std::string _version;
	static std::string default_version;
};

}; // namespace fix

#endif
