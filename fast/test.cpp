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
#include <vector>
#include <algorithm>

#include "message.h"

//#pragma pack(push, 1)
#pragma pack(1)
struct tagFASTHeader
{
    char magic[4];
    char type[4];     
    char space[2];
    unsigned int len; 
};
#pragma pack()
//#pragma pack(pop)


void send_msg(int sock, fix::Message &msg){
	std::string fast_msg = fast::Message::encode_fix_message(&msg, temp);
	for(int i=0; i<(int)(fast_msg.size()); i++){
		printf("%02x", (unsigned char)fast_msg[i]);
		if(i % 2 == 1){
			printf(" ");
		}
		if(i % 16 == 15){
			printf("\n");
		}
		if(i == (int)(fast_msg.size()) - 1){
			printf("\n");
		}
	}
	
	tagFASTHeader header;
	strcpy(header.magic, "FaSt");
	strcpy(header.type,  "A");
	strcpy(header.space, "ki");
	header.len = fast_msg.size();

	unsigned char *p = (unsigned char *)&header;
	for(int i=0; i<sizeof(header); i++){
		printf("%02x", (unsigned char)p[i]);
		if(i % 2 == 1){
			printf(" ");
		}
		if(i % 16 == 15){
			printf("\n");
		}
	}
	printf("\n");
	
	int ret;

	ret = write(sock, &header, sizeof(header));
	printf("send: %d\n", ret);
	ret = write(sock, fast_msg.data(), fast_msg.size());
	printf("send: %d\n", ret);
}

int main(int argc, char **argv){
	int sock;
	const char *ip = "";
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
	
	{
		fast::Template temp(90001);
		temp.add_field(fast::Field(8, fast::Field::String, fast::Field::Mandatory, fast::Field::Constant));
		temp.add_field(fast::Field(9, fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));
		temp.add_field(fast::Field(34, fast::Field::Uint, fast::Field::Mandatory, fast::Field::Increment));
		temp.add_field(fast::Field(35, fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
		temp.add_field(fast::Field(49, fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
		temp.add_field(fast::Field(52, fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
		temp.add_field(fast::Field(56, fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
		temp.add_field(fast::Field(10, fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));

		temp.add_field(fast::Field(96, fast::Field::String, fast::Field::Optional, fast::Field::Noop));
		temp.add_field(fast::Field(95, fast::Field::String, fast::Field::Optional, fast::Field::Noop));
		temp.add_field(fast::Field(553, fast::Field::String, fast::Field::Optional, fast::Field::Noop));
		temp.add_field(fast::Field(554, fast::Field::String, fast::Field::Optional, fast::Field::Noop));
		temp.add_field(fast::Field(98, fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));
		temp.add_field(fast::Field(141, fast::Field::String, fast::Field::Mandatory, fast::Field::Noop));
		temp.add_field(fast::Field(108, fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));
		temp.add_field(fast::Field(383, fast::Field::Uint, fast::Field::Optional, fast::Field::Noop));

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
		send_msg(sock, msg);
	}
	
	{
		/*
		fast::Template temp(90100);
		temp.add_field(fast::Field(8, fast::Field::String, fast::Field::Mandatory, fast::Field::Constant));
		temp.add_field(fast::Field(9, fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));
		temp.add_field(fast::Field(34, fast::Field::Uint, fast::Field::Mandatory, fast::Field::Increment));
		temp.add_field(fast::Field(35, fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
		temp.add_field(fast::Field(49, fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
		temp.add_field(fast::Field(52, fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
		temp.add_field(fast::Field(56, fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
		temp.add_field(fast::Field(10, fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));

		temp.add_field(fast::Field(262, fast::Field::String, fast::Field::Optional, fast::Field::Noop));
		temp.add_field(fast::Field(263, fast::Field::String, fast::Field::Optional, fast::Field::Noop));
		temp.add_field(fast::Field(264, fast::Field::String, fast::Field::Optional, fast::Field::Noop));
		temp.add_field(fast::Field(265, fast::Field::String, fast::Field::Optional, fast::Field::Noop));
		temp.add_field(fast::Field(266, fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));
		temp.add_field(fast::Field(267, fast::Field::String, fast::Field::Mandatory, fast::Field::Noop));
		temp.add_field(fast::Field(269, fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));
		temp.add_field(fast::Field(383, fast::Field::Uint, fast::Field::Optional, fast::Field::Noop));
	// 8=FIX.4.29=12735=V34=249=CLIENT0652=20150424-03:02:12.12656=SERVER1 46=2 55=600446 207=SS 55=000001 207=SZ 262=1 263=1 264=0 267=1 269=0 10=005
		fix::Message msg;
		msg.set(34, 1);
		msg.set(35, "V");
		msg.set(49, "CLIENT06");
		msg.set(56, "SERVER");
		msg.set(52, "20150424-03:22:59.114");
		send_msg(sock, msg);
		*/
	}
	
	while(1){
		char recv_buf[8192];
		ret = read(sock, recv_buf, 8192);
		if(ret == 0){
			printf("connection closed\n");
			break;
		}
		printf("recv: %d\n", ret);
		if(ret == -1){
			printf("error: %s\n", strerror(errno));
		}
		sleep(1);
	}
	
	return 0;
}
