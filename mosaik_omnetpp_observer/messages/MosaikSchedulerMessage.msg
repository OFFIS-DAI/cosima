packet MosaikSchedulerMessage{   	
   	string sender;
   	string receiver;
   	int size;
   	string content;
   	int delay;
   	uint32_t sequenceNumber;
   	string msgId;
   	bool transmission_error;
}
