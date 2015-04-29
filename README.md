# libfix

Libfix 只包含 FIX 报文的封包和解析, 不包括会话管理和网络层相关的内容. 你用自己喜欢的方式, 从网络或者文件读取数据, 用 libfix 来解析. 当你想给网络对端发送数据, 或者想往文件中写入数据时, 用 libfix 来序列化.

## 发送

	#include "fix/fix.h"
	
	fix::Message::set_default_version("FIX.4.2");
	fix::Message msg;
	msg.set(34, 1);
	msg.set(35, "A");
	msg.set(49, "CLIENT06");
	msg.set(56, "SERVER");
	msg.set(52, "20150424-03:06:59.114");
	msg.set(96, "H:12345678:12345678:");
	msg.set(98, 0);
	msg.set(141, "Y");
	msg.set(108, 20);
	
	std::string s = msg.encode();

调用 Message 的 `encode()` 方法后得到可以发送的字节流, 将字节流发送出去, 即相当于向服务器发送了请求.

## 接收

首先, libfix 不包含网络相关的内容, 所以, 你用自己熟悉的方式从网络(socket)中读取数据. 然后, 将读取的数据压入解码器, 解码器解码后得到 FIX 报文.

	#include "fix/fix.h"
	
	char buf[8192];
	int size;
	// 从网络中读取了 size 字节的数据, 然后将数据压入解码器
	
	fix::Decoder decoder;
	decoder.push(buf, size);
	while(1){
		fix::Message tmp;
		int ret = decoder.parse(&tmp);
		if(ret == -1){
			printf("error! ret = %d\n", ret);
			exit(0);
		}else if(ret == 0){
			// 报文未就绪, 继续读网络
			break;
		}
		printf("%s \n", escape(tmp.encode()).c_str());
		// 继续解析, 因为可能一次读取到多个报文
	}

# libfast

__还未实现, 不要使用!__
