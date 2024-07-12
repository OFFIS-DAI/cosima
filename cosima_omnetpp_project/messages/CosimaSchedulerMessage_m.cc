//
// Generated file, do not edit! Created by opp_msgtool 6.0 from messages/CosimaSchedulerMessage.msg.
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
#include <type_traits>
#include "CosimaSchedulerMessage_m.h"

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

Register_Class(CosimaSchedulerMessage)

CosimaSchedulerMessage::CosimaSchedulerMessage(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

CosimaSchedulerMessage::CosimaSchedulerMessage(const CosimaSchedulerMessage& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

CosimaSchedulerMessage::~CosimaSchedulerMessage()
{
}

CosimaSchedulerMessage& CosimaSchedulerMessage::operator=(const CosimaSchedulerMessage& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void CosimaSchedulerMessage::copy(const CosimaSchedulerMessage& other)
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

void CosimaSchedulerMessage::parsimPack(omnetpp::cCommBuffer *b) const
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

void CosimaSchedulerMessage::parsimUnpack(omnetpp::cCommBuffer *b)
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

const char * CosimaSchedulerMessage::getSender() const
{
    return this->sender.c_str();
}

void CosimaSchedulerMessage::setSender(const char * sender)
{
    this->sender = sender;
}

const char * CosimaSchedulerMessage::getReceiver() const
{
    return this->receiver.c_str();
}

void CosimaSchedulerMessage::setReceiver(const char * receiver)
{
    this->receiver = receiver;
}

int CosimaSchedulerMessage::getSize() const
{
    return this->size;
}

void CosimaSchedulerMessage::setSize(int size)
{
    this->size = size;
}

const char * CosimaSchedulerMessage::getContent() const
{
    return this->content.c_str();
}

void CosimaSchedulerMessage::setContent(const char * content)
{
    this->content = content;
}

int CosimaSchedulerMessage::getDelay() const
{
    return this->delay;
}

void CosimaSchedulerMessage::setDelay(int delay)
{
    this->delay = delay;
}

uint32_t CosimaSchedulerMessage::getSequenceNumber() const
{
    return this->sequenceNumber;
}

void CosimaSchedulerMessage::setSequenceNumber(uint32_t sequenceNumber)
{
    this->sequenceNumber = sequenceNumber;
}

const char * CosimaSchedulerMessage::getMsgId() const
{
    return this->msgId.c_str();
}

void CosimaSchedulerMessage::setMsgId(const char * msgId)
{
    this->msgId = msgId;
}

bool CosimaSchedulerMessage::getTransmission_error() const
{
    return this->transmission_error;
}

void CosimaSchedulerMessage::setTransmission_error(bool transmission_error)
{
    this->transmission_error = transmission_error;
}

bool CosimaSchedulerMessage::getTimeout() const
{
    return this->timeout;
}

void CosimaSchedulerMessage::setTimeout(bool timeout)
{
    this->timeout = timeout;
}

bool CosimaSchedulerMessage::getDisconnected_event() const
{
    return this->disconnected_event;
}

void CosimaSchedulerMessage::setDisconnected_event(bool disconnected_event)
{
    this->disconnected_event = disconnected_event;
}

bool CosimaSchedulerMessage::getReconnected_event() const
{
    return this->reconnected_event;
}

void CosimaSchedulerMessage::setReconnected_event(bool reconnected_event)
{
    this->reconnected_event = reconnected_event;
}

bool CosimaSchedulerMessage::getConnection_change_successful() const
{
    return this->connection_change_successful;
}

void CosimaSchedulerMessage::setConnection_change_successful(bool connection_change_successful)
{
    this->connection_change_successful = connection_change_successful;
}

int CosimaSchedulerMessage::getCreationTime() const
{
    return this->creationTime;
}

void CosimaSchedulerMessage::setCreationTime(int creationTime)
{
    this->creationTime = creationTime;
}

bool CosimaSchedulerMessage::getFalsified() const
{
    return this->falsified;
}

void CosimaSchedulerMessage::setFalsified(bool falsified)
{
    this->falsified = falsified;
}

class CosimaSchedulerMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
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
    CosimaSchedulerMessageDescriptor();
    virtual ~CosimaSchedulerMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(CosimaSchedulerMessageDescriptor)

CosimaSchedulerMessageDescriptor::CosimaSchedulerMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(CosimaSchedulerMessage)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

CosimaSchedulerMessageDescriptor::~CosimaSchedulerMessageDescriptor()
{
    delete[] propertyNames;
}

bool CosimaSchedulerMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CosimaSchedulerMessage *>(obj)!=nullptr;
}

const char **CosimaSchedulerMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CosimaSchedulerMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CosimaSchedulerMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 14+base->getFieldCount() : 14;
}

unsigned int CosimaSchedulerMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
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

const char *CosimaSchedulerMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
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

int CosimaSchedulerMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "sender") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "receiver") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "size") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "content") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "delay") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "sequenceNumber") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "msgId") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "transmission_error") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "timeout") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "disconnected_event") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "reconnected_event") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "connection_change_successful") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "creationTime") == 0) return baseIndex + 12;
    if (strcmp(fieldName, "falsified") == 0) return baseIndex + 13;
    return base ? base->findField(fieldName) : -1;
}

const char *CosimaSchedulerMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
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

const char **CosimaSchedulerMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *CosimaSchedulerMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int CosimaSchedulerMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CosimaSchedulerMessage *pp = omnetpp::fromAnyPtr<CosimaSchedulerMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CosimaSchedulerMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaSchedulerMessage *pp = omnetpp::fromAnyPtr<CosimaSchedulerMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CosimaSchedulerMessage'", field);
    }
}

const char *CosimaSchedulerMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CosimaSchedulerMessage *pp = omnetpp::fromAnyPtr<CosimaSchedulerMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CosimaSchedulerMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CosimaSchedulerMessage *pp = omnetpp::fromAnyPtr<CosimaSchedulerMessage>(object); (void)pp;
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

void CosimaSchedulerMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaSchedulerMessage *pp = omnetpp::fromAnyPtr<CosimaSchedulerMessage>(object); (void)pp;
    switch (field) {
        case FIELD_sender: pp->setSender((value)); break;
        case FIELD_receiver: pp->setReceiver((value)); break;
        case FIELD_size: pp->setSize(string2long(value)); break;
        case FIELD_content: pp->setContent((value)); break;
        case FIELD_delay: pp->setDelay(string2long(value)); break;
        case FIELD_sequenceNumber: pp->setSequenceNumber(string2ulong(value)); break;
        case FIELD_msgId: pp->setMsgId((value)); break;
        case FIELD_transmission_error: pp->setTransmission_error(string2bool(value)); break;
        case FIELD_timeout: pp->setTimeout(string2bool(value)); break;
        case FIELD_disconnected_event: pp->setDisconnected_event(string2bool(value)); break;
        case FIELD_reconnected_event: pp->setReconnected_event(string2bool(value)); break;
        case FIELD_connection_change_successful: pp->setConnection_change_successful(string2bool(value)); break;
        case FIELD_creationTime: pp->setCreationTime(string2long(value)); break;
        case FIELD_falsified: pp->setFalsified(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CosimaSchedulerMessage'", field);
    }
}

omnetpp::cValue CosimaSchedulerMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CosimaSchedulerMessage *pp = omnetpp::fromAnyPtr<CosimaSchedulerMessage>(object); (void)pp;
    switch (field) {
        case FIELD_sender: return pp->getSender();
        case FIELD_receiver: return pp->getReceiver();
        case FIELD_size: return pp->getSize();
        case FIELD_content: return pp->getContent();
        case FIELD_delay: return pp->getDelay();
        case FIELD_sequenceNumber: return (omnetpp::intval_t)(pp->getSequenceNumber());
        case FIELD_msgId: return pp->getMsgId();
        case FIELD_transmission_error: return pp->getTransmission_error();
        case FIELD_timeout: return pp->getTimeout();
        case FIELD_disconnected_event: return pp->getDisconnected_event();
        case FIELD_reconnected_event: return pp->getReconnected_event();
        case FIELD_connection_change_successful: return pp->getConnection_change_successful();
        case FIELD_creationTime: return pp->getCreationTime();
        case FIELD_falsified: return pp->getFalsified();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CosimaSchedulerMessage' as cValue -- field index out of range?", field);
    }
}

void CosimaSchedulerMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaSchedulerMessage *pp = omnetpp::fromAnyPtr<CosimaSchedulerMessage>(object); (void)pp;
    switch (field) {
        case FIELD_sender: pp->setSender(value.stringValue()); break;
        case FIELD_receiver: pp->setReceiver(value.stringValue()); break;
        case FIELD_size: pp->setSize(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_content: pp->setContent(value.stringValue()); break;
        case FIELD_delay: pp->setDelay(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sequenceNumber: pp->setSequenceNumber(omnetpp::checked_int_cast<uint32_t>(value.intValue())); break;
        case FIELD_msgId: pp->setMsgId(value.stringValue()); break;
        case FIELD_transmission_error: pp->setTransmission_error(value.boolValue()); break;
        case FIELD_timeout: pp->setTimeout(value.boolValue()); break;
        case FIELD_disconnected_event: pp->setDisconnected_event(value.boolValue()); break;
        case FIELD_reconnected_event: pp->setReconnected_event(value.boolValue()); break;
        case FIELD_connection_change_successful: pp->setConnection_change_successful(value.boolValue()); break;
        case FIELD_creationTime: pp->setCreationTime(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_falsified: pp->setFalsified(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CosimaSchedulerMessage'", field);
    }
}

const char *CosimaSchedulerMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr CosimaSchedulerMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CosimaSchedulerMessage *pp = omnetpp::fromAnyPtr<CosimaSchedulerMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CosimaSchedulerMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaSchedulerMessage *pp = omnetpp::fromAnyPtr<CosimaSchedulerMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CosimaSchedulerMessage'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

