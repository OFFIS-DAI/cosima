packet MosaikSchedulerMessage{   	
   	string sender;
   	string receiver;
   	int size;
   	string content;
   	int delay;
   	uint32_t sequenceNumber;
   	string msgId;
   	bool transmission_error;
   	bool timeout = false;
   	bool disconnected_event;
   	bool reconnected_event;  
   	bool connection_change_successful; 
   	int creationTime; 
   	bool falsified = false;
}
