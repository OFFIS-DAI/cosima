//
// Generated file, do not edit! Created by nedtool 5.6 from messages/MosaikSchedulerMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include "MosaikSchedulerMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace {
template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)(static_cast<const omnetpp::cObject *>(t));
}

template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && !std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)dynamic_cast<const void *>(t);
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)static_cast<const void *>(t);
}

}


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule to generate operator<< for shared_ptr<T>
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const std::shared_ptr<T>& t) { return out << t.get(); }

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');

    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(MosaikSchedulerMessage)

MosaikSchedulerMessage::MosaikSchedulerMessage(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

MosaikSchedulerMessage::MosaikSchedulerMessage(const MosaikSchedulerMessage& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

MosaikSchedulerMessage::~MosaikSchedulerMessage()
{
}

MosaikSchedulerMessage& MosaikSchedulerMessage::operator=(const MosaikSchedulerMessage& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void MosaikSchedulerMessage::copy(const MosaikSchedulerMessage& other)
{
    this->sender = other.sender;
    this->receiver = other.receiver;
    this->size = other.size;
    this->content = other.content;
    this->delay = other.delay;
    this->sequenceNumber = other.sequenceNumber;
    this->msgId = other.msgId;
    this->transmission_error = other.transmission_error;
    this->timeout = other.timeout;
    this->disconnected_event = other.disconnected_event;
    this->reconnected_event = other.reconnected_event;
    this->connection_change_successful = other.connection_change_successful;
    this->creationTime = other.creationTime;
    this->falsified = other.falsified;
}

void MosaikSchedulerMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->sender);
    doParsimPacking(b,this->receiver);
    doParsimPacking(b,this->size);
    doParsimPacking(b,this->content);
    doParsimPacking(b,this->delay);
    doParsimPacking(b,this->sequenceNumber);
    doParsimPacking(b,this->msgId);
    doParsimPacking(b,this->transmission_error);
    doParsimPacking(b,this->timeout);
    doParsimPacking(b,this->disconnected_event);
    doParsimPacking(b,this->reconnected_event);
    doParsimPacking(b,this->connection_change_successful);
    doParsimPacking(b,this->creationTime);
    doParsimPacking(b,this->falsified);
}

void MosaikSchedulerMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->sender);
    doParsimUnpacking(b,this->receiver);
    doParsimUnpacking(b,this->size);
    doParsimUnpacking(b,this->content);
    doParsimUnpacking(b,this->delay);
    doParsimUnpacking(b,this->sequenceNumber);
    doParsimUnpacking(b,this->msgId);
    doParsimUnpacking(b,this->transmission_error);
    doParsimUnpacking(b,this->timeout);
    doParsimUnpacking(b,this->disconnected_event);
    doParsimUnpacking(b,this->reconnected_event);
    doParsimUnpacking(b,this->connection_change_successful);
    doParsimUnpacking(b,this->creationTime);
    doParsimUnpacking(b,this->falsified);
}

const char * MosaikSchedulerMessage::getSender() const
{
    return this->sender.c_str();
}

void MosaikSchedulerMessage::setSender(const char * sender)
{
    this->sender = sender;
}

const char * MosaikSchedulerMessage::getReceiver() const
{
    return this->receiver.c_str();
}

void MosaikSchedulerMessage::setReceiver(const char * receiver)
{
    this->receiver = receiver;
}

int MosaikSchedulerMessage::getSize() const
{
    return this->size;
}

void MosaikSchedulerMessage::setSize(int size)
{
    this->size = size;
}

const char * MosaikSchedulerMessage::getContent() const
{
    return this->content.c_str();
}

void MosaikSchedulerMessage::setContent(const char * content)
{
    this->content = content;
}

int MosaikSchedulerMessage::getDelay() const
{
    return this->delay;
}

void MosaikSchedulerMessage::setDelay(int delay)
{
    this->delay = delay;
}

uint32_t MosaikSchedulerMessage::getSequenceNumber() const
{
    return this->sequenceNumber;
}

void MosaikSchedulerMessage::setSequenceNumber(uint32_t sequenceNumber)
{
    this->sequenceNumber = sequenceNumber;
}

const char * MosaikSchedulerMessage::getMsgId() const
{
    return this->msgId.c_str();
}

void MosaikSchedulerMessage::setMsgId(const char * msgId)
{
    this->msgId = msgId;
}

bool MosaikSchedulerMessage::getTransmission_error() const
{
    return this->transmission_error;
}

void MosaikSchedulerMessage::setTransmission_error(bool transmission_error)
{
    this->transmission_error = transmission_error;
}

bool MosaikSchedulerMessage::getTimeout() const
{
    return this->timeout;
}

void MosaikSchedulerMessage::setTimeout(bool timeout)
{
    this->timeout = timeout;
}

bool MosaikSchedulerMessage::getDisconnected_event() const
{
    return this->disconnected_event;
}

void MosaikSchedulerMessage::setDisconnected_event(bool disconnected_event)
{
    this->disconnected_event = disconnected_event;
}

bool MosaikSchedulerMessage::getReconnected_event() const
{
    return this->reconnected_event;
}

void MosaikSchedulerMessage::setReconnected_event(bool reconnected_event)
{
    this->reconnected_event = reconnected_event;
}

bool MosaikSchedulerMessage::getConnection_change_successful() const
{
    return this->connection_change_successful;
}

void MosaikSchedulerMessage::setConnection_change_successful(bool connection_change_successful)
{
    this->connection_change_successful = connection_change_successful;
}

int MosaikSchedulerMessage::getCreationTime() const
{
    return this->creationTime;
}

void MosaikSchedulerMessage::setCreationTime(int creationTime)
{
    this->creationTime = creationTime;
}

bool MosaikSchedulerMessage::getFalsified() const
{
    return this->falsified;
}

void MosaikSchedulerMessage::setFalsified(bool falsified)
{
    this->falsified = falsified;
}

class MosaikSchedulerMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_sender,
        FIELD_receiver,
        FIELD_size,
        FIELD_content,
        FIELD_delay,
        FIELD_sequenceNumber,
        FIELD_msgId,
        FIELD_transmission_error,
        FIELD_timeout,
        FIELD_disconnected_event,
        FIELD_reconnected_event,
        FIELD_connection_change_successful,
        FIELD_creationTime,
        FIELD_falsified,
    };
  public:
    MosaikSchedulerMessageDescriptor();
    virtual ~MosaikSchedulerMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MosaikSchedulerMessageDescriptor)

MosaikSchedulerMessageDescriptor::MosaikSchedulerMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(MosaikSchedulerMessage)), "omnetpp::cPacket")
{
    propertynames = nullptr;
}

MosaikSchedulerMessageDescriptor::~MosaikSchedulerMessageDescriptor()
{
    delete[] propertynames;
}

bool MosaikSchedulerMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MosaikSchedulerMessage *>(obj)!=nullptr;
}

const char **MosaikSchedulerMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MosaikSchedulerMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MosaikSchedulerMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 14+basedesc->getFieldCount() : 14;
}

unsigned int MosaikSchedulerMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_sender
        FD_ISEDITABLE,    // FIELD_receiver
        FD_ISEDITABLE,    // FIELD_size
        FD_ISEDITABLE,    // FIELD_content
        FD_ISEDITABLE,    // FIELD_delay
        FD_ISEDITABLE,    // FIELD_sequenceNumber
        FD_ISEDITABLE,    // FIELD_msgId
        FD_ISEDITABLE,    // FIELD_transmission_error
        FD_ISEDITABLE,    // FIELD_timeout
        FD_ISEDITABLE,    // FIELD_disconnected_event
        FD_ISEDITABLE,    // FIELD_reconnected_event
        FD_ISEDITABLE,    // FIELD_connection_change_successful
        FD_ISEDITABLE,    // FIELD_creationTime
        FD_ISEDITABLE,    // FIELD_falsified
    };
    return (field >= 0 && field < 14) ? fieldTypeFlags[field] : 0;
}

const char *MosaikSchedulerMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sender",
        "receiver",
        "size",
        "content",
        "delay",
        "sequenceNumber",
        "msgId",
        "transmission_error",
        "timeout",
        "disconnected_event",
        "reconnected_event",
        "connection_change_successful",
        "creationTime",
        "falsified",
    };
    return (field >= 0 && field < 14) ? fieldNames[field] : nullptr;
}

int MosaikSchedulerMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 's' && strcmp(fieldName, "sender") == 0) return base+0;
    if (fieldName[0] == 'r' && strcmp(fieldName, "receiver") == 0) return base+1;
    if (fieldName[0] == 's' && strcmp(fieldName, "size") == 0) return base+2;
    if (fieldName[0] == 'c' && strcmp(fieldName, "content") == 0) return base+3;
    if (fieldName[0] == 'd' && strcmp(fieldName, "delay") == 0) return base+4;
    if (fieldName[0] == 's' && strcmp(fieldName, "sequenceNumber") == 0) return base+5;
    if (fieldName[0] == 'm' && strcmp(fieldName, "msgId") == 0) return base+6;
    if (fieldName[0] == 't' && strcmp(fieldName, "transmission_error") == 0) return base+7;
    if (fieldName[0] == 't' && strcmp(fieldName, "timeout") == 0) return base+8;
    if (fieldName[0] == 'd' && strcmp(fieldName, "disconnected_event") == 0) return base+9;
    if (fieldName[0] == 'r' && strcmp(fieldName, "reconnected_event") == 0) return base+10;
    if (fieldName[0] == 'c' && strcmp(fieldName, "connection_change_successful") == 0) return base+11;
    if (fieldName[0] == 'c' && strcmp(fieldName, "creationTime") == 0) return base+12;
    if (fieldName[0] == 'f' && strcmp(fieldName, "falsified") == 0) return base+13;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MosaikSchedulerMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_sender
        "string",    // FIELD_receiver
        "int",    // FIELD_size
        "string",    // FIELD_content
        "int",    // FIELD_delay
        "uint32_t",    // FIELD_sequenceNumber
        "string",    // FIELD_msgId
        "bool",    // FIELD_transmission_error
        "bool",    // FIELD_timeout
        "bool",    // FIELD_disconnected_event
        "bool",    // FIELD_reconnected_event
        "bool",    // FIELD_connection_change_successful
        "int",    // FIELD_creationTime
        "bool",    // FIELD_falsified
    };
    return (field >= 0 && field < 14) ? fieldTypeStrings[field] : nullptr;
}

const char **MosaikSchedulerMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MosaikSchedulerMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MosaikSchedulerMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MosaikSchedulerMessage *pp = (MosaikSchedulerMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MosaikSchedulerMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MosaikSchedulerMessage *pp = (MosaikSchedulerMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MosaikSchedulerMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MosaikSchedulerMessage *pp = (MosaikSchedulerMessage *)object; (void)pp;
    switch (field) {
        case FIELD_sender: return oppstring2string(pp->getSender());
        case FIELD_receiver: return oppstring2string(pp->getReceiver());
        case FIELD_size: return long2string(pp->getSize());
        case FIELD_content: return oppstring2string(pp->getContent());
        case FIELD_delay: return long2string(pp->getDelay());
        case FIELD_sequenceNumber: return ulong2string(pp->getSequenceNumber());
        case FIELD_msgId: return oppstring2string(pp->getMsgId());
        case FIELD_transmission_error: return bool2string(pp->getTransmission_error());
        case FIELD_timeout: return bool2string(pp->getTimeout());
        case FIELD_disconnected_event: return bool2string(pp->getDisconnected_event());
        case FIELD_reconnected_event: return bool2string(pp->getReconnected_event());
        case FIELD_connection_change_successful: return bool2string(pp->getConnection_change_successful());
        case FIELD_creationTime: return long2string(pp->getCreationTime());
        case FIELD_falsified: return bool2string(pp->getFalsified());
        default: return "";
    }
}

bool MosaikSchedulerMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MosaikSchedulerMessage *pp = (MosaikSchedulerMessage *)object; (void)pp;
    switch (field) {
        case FIELD_sender: pp->setSender((value)); return true;
        case FIELD_receiver: pp->setReceiver((value)); return true;
        case FIELD_size: pp->setSize(string2long(value)); return true;
        case FIELD_content: pp->setContent((value)); return true;
        case FIELD_delay: pp->setDelay(string2long(value)); return true;
        case FIELD_sequenceNumber: pp->setSequenceNumber(string2ulong(value)); return true;
        case FIELD_msgId: pp->setMsgId((value)); return true;
        case FIELD_transmission_error: pp->setTransmission_error(string2bool(value)); return true;
        case FIELD_timeout: pp->setTimeout(string2bool(value)); return true;
        case FIELD_disconnected_event: pp->setDisconnected_event(string2bool(value)); return true;
        case FIELD_reconnected_event: pp->setReconnected_event(string2bool(value)); return true;
        case FIELD_connection_change_successful: pp->setConnection_change_successful(string2bool(value)); return true;
        case FIELD_creationTime: pp->setCreationTime(string2long(value)); return true;
        case FIELD_falsified: pp->setFalsified(string2bool(value)); return true;
        default: return false;
    }
}

const char *MosaikSchedulerMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *MosaikSchedulerMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MosaikSchedulerMessage *pp = (MosaikSchedulerMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

