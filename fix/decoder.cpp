#include "../util/strings.h"
#include "fix.h"

namespace fix{

int Decoder::push(const char *buf, int len){
	buffer.append(buf, len);
	return len;
}

int Decoder::parse(Message *msg){
	msg->reset();
	
	// 对于某些特殊的标明长度的 tag, 它指明下一个 tag 对应的 value 长度,
	// 必须根据长度读取"下一个tag", 而不能通过分隔符.
	// 90 SecureDataLen
	// 95 RawDataLength
	int len_tag = 0;
	int len_len = 0;
	
	const char *key = buffer.data();
	int size = (int)buffer.size();
	int key_len;
	int val_len;

	while(1){
		const char *val = (const char *)memchr(key, fix::SEPARATE_BYTE, size);
		if(val == NULL){
			break;
		}
		val ++;
	
		key_len = val - key - 1;
		size -= key_len + 1;

		const char *end;
		std::string key_s(key, key_len);
		int tag = str_to_int(key_s);
		if(len_tag > 0 && tag == len_tag + 1){
			if(len_len >= size){
				break;
			}
			end = val + len_len + 1;
			len_tag = 0;
			len_len = 0;
		}else{
			end = (const char *)memchr(val, fix::STOP_BYTE, size);
			if(end == NULL){
				break;
			}
			end ++;
		}
		//printf("%d key: %d, end: %d, %d\n", __LINE__, key, end, end - key);
	
		val_len = end - val - 1;
		size -= val_len + 1;

		std::string val_s(val, val_len);
		msg->set(tag, val_s);
		
		if(tag == 10){ // checksum
			// 一个完整的报文解析结束, 从缓冲区清除已经解析了的数据
			msg->encode();
			if(msg->checksum() != str_to_int(val_s)){
				// 错误, 校验不通过!
				return -1;
			}
			
			buffer = std::string(end, size);
			return 1;
		}
		if(tag == 90 || tag == 95){
			len_tag = tag;
			len_len = str_to_int(val_s);
		}

		key = end;
	}
	return 0;
}

}; // namespace fix
