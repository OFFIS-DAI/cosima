cplusplus {{
 #include "message.pb.h"
}}
//class TicTocMsg extends void; 

packet TikTokPacket{
   	
   	string sender;
   	string receiver;
   	int size;
   	string content;
   	double delay;
   	uint32_t sequenceNumber;
//   	TicTocMsg protomsg;  	
}
