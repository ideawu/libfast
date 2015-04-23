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
	fast::Template temp(90001);
	temp.add_field(fast::Field(fast::Field::String, fast::Field::Mandatory, fast::Field::Constant));
	temp.add_field(fast::Field(fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));
	temp.add_field(fast::Field(fast::Field::Uint, fast::Field::Mandatory, fast::Field::Increment));
	temp.add_field(fast::Field(fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
	temp.add_field(fast::Field(fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
	temp.add_field(fast::Field(fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
	temp.add_field(fast::Field(fast::Field::String, fast::Field::Mandatory, fast::Field::Copy));
	temp.add_field(fast::Field(fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));

	temp.add_field(fast::Field(fast::Field::String, fast::Field::Optional, fast::Field::Noop));
	temp.add_field(fast::Field(fast::Field::String, fast::Field::Optional, fast::Field::Noop));
	temp.add_field(fast::Field(fast::Field::String, fast::Field::Optional, fast::Field::Noop));
	temp.add_field(fast::Field(fast::Field::String, fast::Field::Optional, fast::Field::Noop));
	temp.add_field(fast::Field(fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));
	temp.add_field(fast::Field(fast::Field::String, fast::Field::Mandatory, fast::Field::Noop));
	temp.add_field(fast::Field(fast::Field::Uint, fast::Field::Mandatory, fast::Field::Noop));
	temp.add_field(fast::Field(fast::Field::Uint, fast::Field::Optional, fast::Field::Noop));
	
	// 8=FIX.4.2|9=117|35=A|49=CLIENT08|56=SERVER|34=1|52=20150421-21:20:56.000|98=0|108=1600|96=H::135790:|95=10|141=Y|553=|554=|383=99999|10=068|
	fast::Message msg;
	
	
#if 1
	msg.add("FIX.4.2");
	msg.add(99);
	msg.add(1);
	msg.add("A");
	msg.add("CLIENT06");
	msg.add("20150423-07:01:35.995");
	
	msg.add("SERVER");
	msg.add(33); // checksum
	msg.add("H:12345678:12345678:");
	msg.add("");
	msg.add("");
	msg.add("");
	msg.add(0);

	msg.add("Y");
	msg.add(20);
	msg.add(0);
#else
	msg.set_template(1);
	msg.add("HelloWorld");
#endif
	
	std::string fast_msg = msg.encode(temp);
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
	printf("error: %s\n", strerror(errno));
	
	return 0;
}
