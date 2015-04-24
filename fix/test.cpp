#include "message.h"

int main(int argc, char **argv){
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
	for(int i=0; i<(int)s.size(); i++){
		char c = s[i];
		if(c == '\x01'){ // SOH
			printf("|");
		}else{
			printf("%c", c);
		}
	}
	printf("\n");

	return 0;
}
