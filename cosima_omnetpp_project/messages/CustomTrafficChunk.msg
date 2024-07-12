import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

cplusplus{{
    const inet::B CHUNK_LENGTH; 
}}

class CustomTrafficChunk extends inet::FieldsChunk{
    int msgId;
    string sender;
    int senderPort;
    string receiver;
    int receiverPort;
    int packetSize_B;
    int timeSend_ms;   
    bool reply=false; 
    int replyAfter=100;
}
