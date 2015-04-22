#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <string>
#include <map>


static const unsigned char SIGN_BIT = (unsigned char)('\x40');
static const unsigned char STOP_BIT = (unsigned char)('\x80');
static const unsigned char DATA_BITS = (unsigned char)('\x7F');
static const unsigned char EMPTY_STRING = (unsigned char)('\x80');

static std::string encode_num(int64_t val){
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
	return buffer;
}

static std::string encode_num(uint64_t val){
	std::string buffer;
	uint64_t until = 0ULL;
	unsigned char byte = 0x80;
	while(val != until || byte != 0){
		byte |= (unsigned char)(val & DATA_BITS);
		val >>= 7;
		buffer.push_back(byte);
		byte = 0;
	}
	return buffer;
}

static inline std::string encode_num(int32_t val){
	return encode_num((int64_t)val);
}

static inline std::string encode_num(uint32_t val){
	return encode_num((uint64_t)val);
}


class FastMessage
{
private:
	int template_id_;
	std::map<int, std::string> fields_;
public:
	FastMessage();
	~FastMessage();
	
	int set_template(int template_id);
	int add(int32_t val, int index=-1);
	int add(int64_t val, int index=-1);
	int add(uint32_t val, int index=-1);
	int add(uint64_t val, int index=-1);
	int add(const char *val, int index=-1);
	
	// set field
	// int set_field(int index, int32_t val);
	// ...
	
	std::string encode();
};

FastMessage::FastMessage(){
	template_id_ = 0;
}

FastMessage::~FastMessage(){
}

int FastMessage::set_template(int template_id){
	template_id_ = template_id;
	return template_id;
}

int FastMessage::add(int32_t val, int index){
	return add((int64_t)val, index);
}

int FastMessage::add(int64_t val, int index){
	if(index == -1){
		index = (int)fields_.size();
	}
	std::string enc = encode_num(val);
	fields_[index] = enc;
	return 0;
}

int FastMessage::add(uint32_t val, int index){
	return add((uint64_t)val, index);
}

int FastMessage::add(uint64_t val, int index){
	if(index == -1){
		index = (int)fields_.size();
	}
	std::string enc = encode_num(val);
	fields_[index] = enc;
	return 0;
}

int FastMessage::add(const char *val, int index){
//	printf("%d\n", __LINE__);
	if(index == -1){
		index = (int)fields_.size();
	}

	std::string enc;
	int ret = 0;
	int len = strlen(val);
	if(len == 0){
		printf("%d\n", __LINE__);
		enc.push_back(EMPTY_STRING);
	}else{
		for(int pos = 0; pos < len - 1; ++pos){
			enc.push_back((unsigned char)(val[pos]));
			ret ++;
		}
		enc.push_back(val[len - 1] | STOP_BIT);
		ret ++;
	}
	fields_[index] = enc;
	return ret;
}

std::string FastMessage::encode(){
	int num_fields = 0; // TODO: 根据模板获得
	if(!fields_.empty()){
		std::map<int, std::string>::reverse_iterator it = fields_.rbegin();
		num_fields = it->first;
	}
	std::string buffer;
	
	int num = ceil(num_fields / 7.0);
	buffer.append(num, '\0');

	for(std::map<int, std::string>::iterator it = fields_.begin(); it != fields_.end(); it++){
		int i = it->first;
		// template_id is always set, so i+1
		int byte_offset = (i + 1) / 7;
		int bit_offset = 7 - (i + 1) % 7 - 1;
		buffer[byte_offset] |= (1 << bit_offset);
		printf("%d:%d = 1, %02x\n", byte_offset, bit_offset, buffer[byte_offset]);
	}
	buffer[buffer.size() - 1] |= STOP_BIT;

	buffer[0] |= (1 << 6); // template_id is set
	buffer.append(encode_num(template_id_));

	for(std::map<int, std::string>::iterator it = fields_.begin(); it != fields_.end(); it++){
		printf("%d\n", it->first);
		buffer.append(it->second);
	}

	return buffer;
}


#pragma pack(push, 1)
struct tagFASTHeader
{
    char magic[4];        // equ g_pszMagic
    char type[4];     
    char space[2];
    unsigned int len; 
};
#pragma pack(pop)


int main(int argc, char **argv){
	{
		// 8=FIX.4.2|9=117|35=A|49=CLIENT08|56=SERVER|34=1|52=20150421-21:20:56.000|98=0|108=1600|96=H::135790:|95=10|141=Y|553=|554=|383=99999|10=068|
		FastMessage msg;
		
		
		// pmap
		unsigned char pmap[4] = {0x7f, 0xf0, 0x50, '\0'};
		
#if 1
		msg.set_template(90001);
		msg.add("FIX.4.2");
		msg.add((uint32_t)117);
		msg.add((uint32_t)1);
		msg.add("A");
		msg.add("CLIENT08");
		msg.add("20150421-21:14:34.000");
		
		msg.add("SERVER");
		msg.add((uint32_t)68);
		msg.add("H::135790:");
		msg.add("10");
		msg.add("");
		msg.add("");
		msg.add((uint32_t)91);

		msg.add("Y");
		msg.add((uint32_t)1600);
		msg.add((uint32_t)99999);
#else
		msg.set_template(1);
		msg.add("HelloWorld");
#endif
		
		std::string fast_msg = msg.encode();
		for(int i=0; i<(int)(fast_msg.size()); i++){
			printf("%02x ", (unsigned char)fast_msg[i]);
			if(i == (int)(fast_msg.size()) - 1){
				printf("\n");
			}
		}

		//exit(0);

		int sock;
		const char *ip = "127.0.0.1";
		int port = 8876;
		struct sockaddr_in addr;
		bzero(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons((short)port);
		inet_pton(AF_INET, ip, &addr.sin_addr);

		if((sock = ::socket(AF_INET, SOCK_STREAM, 0)) == -1){
			printf("%d: error\n", __LINE__);
		}
		if(::connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
			printf("%d: error\n", __LINE__);
		}
		
		tagFASTHeader header;
		strcpy(header.magic, "FaSt");
		strcpy(header.type, "A");
		strcpy(header.space, "ki");
		header.len = fast_msg.size();
		
		int ret;
		ret = write(sock, &header, sizeof(header));
		printf("send: %d\n", ret);
		ret = write(sock, fast_msg.data(), fast_msg.size());
		printf("send: %d\n", ret);
		
		char recv_buf[8192];
		ret = ::read(sock, recv_buf, 8192);
		printf("recv: %d\n", ret);

	}
	
	return 0;
}
