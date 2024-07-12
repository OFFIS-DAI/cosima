//
// Generated file, do not edit! Created by opp_msgtool 6.0 from messages/CustomTrafficChunk.msg.
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
#include "CustomTrafficChunk_m.h"

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

Register_Class(CustomTrafficChunk)

CustomTrafficChunk::CustomTrafficChunk() : ::inet::FieldsChunk()
{
}

CustomTrafficChunk::CustomTrafficChunk(const CustomTrafficChunk& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CustomTrafficChunk::~CustomTrafficChunk()
{
}

CustomTrafficChunk& CustomTrafficChunk::operator=(const CustomTrafficChunk& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CustomTrafficChunk::copy(const CustomTrafficChunk& other)
{
    this->msgId = other.msgId;
    this->sender = other.sender;
    this->senderPort = other.senderPort;
    this->receiver = other.receiver;
    this->receiverPort = other.receiverPort;
    this->packetSize_B = other.packetSize_B;
    this->timeSend_ms = other.timeSend_ms;
    this->reply = other.reply;
    this->replyAfter = other.replyAfter;
}

void CustomTrafficChunk::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->msgId);
    doParsimPacking(b,this->sender);
    doParsimPacking(b,this->senderPort);
    doParsimPacking(b,this->receiver);
    doParsimPacking(b,this->receiverPort);
    doParsimPacking(b,this->packetSize_B);
    doParsimPacking(b,this->timeSend_ms);
    doParsimPacking(b,this->reply);
    doParsimPacking(b,this->replyAfter);
}

void CustomTrafficChunk::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->msgId);
    doParsimUnpacking(b,this->sender);
    doParsimUnpacking(b,this->senderPort);
    doParsimUnpacking(b,this->receiver);
    doParsimUnpacking(b,this->receiverPort);
    doParsimUnpacking(b,this->packetSize_B);
    doParsimUnpacking(b,this->timeSend_ms);
    doParsimUnpacking(b,this->reply);
    doParsimUnpacking(b,this->replyAfter);
}

int CustomTrafficChunk::getMsgId() const
{
    return this->msgId;
}

void CustomTrafficChunk::setMsgId(int msgId)
{
    handleChange();
    this->msgId = msgId;
}

const char * CustomTrafficChunk::getSender() const
{
    return this->sender.c_str();
}

void CustomTrafficChunk::setSender(const char * sender)
{
    handleChange();
    this->sender = sender;
}

int CustomTrafficChunk::getSenderPort() const
{
    return this->senderPort;
}

void CustomTrafficChunk::setSenderPort(int senderPort)
{
    handleChange();
    this->senderPort = senderPort;
}

const char * CustomTrafficChunk::getReceiver() const
{
    return this->receiver.c_str();
}

void CustomTrafficChunk::setReceiver(const char * receiver)
{
    handleChange();
    this->receiver = receiver;
}

int CustomTrafficChunk::getReceiverPort() const
{
    return this->receiverPort;
}

void CustomTrafficChunk::setReceiverPort(int receiverPort)
{
    handleChange();
    this->receiverPort = receiverPort;
}

int CustomTrafficChunk::getPacketSize_B() const
{
    return this->packetSize_B;
}

void CustomTrafficChunk::setPacketSize_B(int packetSize_B)
{
    handleChange();
    this->packetSize_B = packetSize_B;
}

int CustomTrafficChunk::getTimeSend_ms() const
{
    return this->timeSend_ms;
}

void CustomTrafficChunk::setTimeSend_ms(int timeSend_ms)
{
    handleChange();
    this->timeSend_ms = timeSend_ms;
}

bool CustomTrafficChunk::getReply() const
{
    return this->reply;
}

void CustomTrafficChunk::setReply(bool reply)
{
    handleChange();
    this->reply = reply;
}

int CustomTrafficChunk::getReplyAfter() const
{
    return this->replyAfter;
}

void CustomTrafficChunk::setReplyAfter(int replyAfter)
{
    handleChange();
    this->replyAfter = replyAfter;
}

class CustomTrafficChunkDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_msgId,
        FIELD_sender,
        FIELD_senderPort,
        FIELD_receiver,
        FIELD_receiverPort,
        FIELD_packetSize_B,
        FIELD_timeSend_ms,
        FIELD_reply,
        FIELD_replyAfter,
    };
  public:
    CustomTrafficChunkDescriptor();
    virtual ~CustomTrafficChunkDescriptor();

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

Register_ClassDescriptor(CustomTrafficChunkDescriptor)

CustomTrafficChunkDescriptor::CustomTrafficChunkDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(CustomTrafficChunk)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CustomTrafficChunkDescriptor::~CustomTrafficChunkDescriptor()
{
    delete[] propertyNames;
}

bool CustomTrafficChunkDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CustomTrafficChunk *>(obj)!=nullptr;
}

const char **CustomTrafficChunkDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CustomTrafficChunkDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CustomTrafficChunkDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 9+base->getFieldCount() : 9;
}

unsigned int CustomTrafficChunkDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_msgId
        FD_ISEDITABLE,    // FIELD_sender
        FD_ISEDITABLE,    // FIELD_senderPort
        FD_ISEDITABLE,    // FIELD_receiver
        FD_ISEDITABLE,    // FIELD_receiverPort
        FD_ISEDITABLE,    // FIELD_packetSize_B
        FD_ISEDITABLE,    // FIELD_timeSend_ms
        FD_ISEDITABLE,    // FIELD_reply
        FD_ISEDITABLE,    // FIELD_replyAfter
    };
    return (field >= 0 && field < 9) ? fieldTypeFlags[field] : 0;
}

const char *CustomTrafficChunkDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "msgId",
        "sender",
        "senderPort",
        "receiver",
        "receiverPort",
        "packetSize_B",
        "timeSend_ms",
        "reply",
        "replyAfter",
    };
    return (field >= 0 && field < 9) ? fieldNames[field] : nullptr;
}

int CustomTrafficChunkDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "msgId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "sender") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "senderPort") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "receiver") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "receiverPort") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "packetSize_B") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "timeSend_ms") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "reply") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "replyAfter") == 0) return baseIndex + 8;
    return base ? base->findField(fieldName) : -1;
}

const char *CustomTrafficChunkDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_msgId
        "string",    // FIELD_sender
        "int",    // FIELD_senderPort
        "string",    // FIELD_receiver
        "int",    // FIELD_receiverPort
        "int",    // FIELD_packetSize_B
        "int",    // FIELD_timeSend_ms
        "bool",    // FIELD_reply
        "int",    // FIELD_replyAfter
    };
    return (field >= 0 && field < 9) ? fieldTypeStrings[field] : nullptr;
}

const char **CustomTrafficChunkDescriptor::getFieldPropertyNames(int field) const
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

const char *CustomTrafficChunkDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int CustomTrafficChunkDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CustomTrafficChunk *pp = omnetpp::fromAnyPtr<CustomTrafficChunk>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CustomTrafficChunkDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CustomTrafficChunk *pp = omnetpp::fromAnyPtr<CustomTrafficChunk>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CustomTrafficChunk'", field);
    }
}

const char *CustomTrafficChunkDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CustomTrafficChunk *pp = omnetpp::fromAnyPtr<CustomTrafficChunk>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CustomTrafficChunkDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CustomTrafficChunk *pp = omnetpp::fromAnyPtr<CustomTrafficChunk>(object); (void)pp;
    switch (field) {
        case FIELD_msgId: return long2string(pp->getMsgId());
        case FIELD_sender: return oppstring2string(pp->getSender());
        case FIELD_senderPort: return long2string(pp->getSenderPort());
        case FIELD_receiver: return oppstring2string(pp->getReceiver());
        case FIELD_receiverPort: return long2string(pp->getReceiverPort());
        case FIELD_packetSize_B: return long2string(pp->getPacketSize_B());
        case FIELD_timeSend_ms: return long2string(pp->getTimeSend_ms());
        case FIELD_reply: return bool2string(pp->getReply());
        case FIELD_replyAfter: return long2string(pp->getReplyAfter());
        default: return "";
    }
}

void CustomTrafficChunkDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CustomTrafficChunk *pp = omnetpp::fromAnyPtr<CustomTrafficChunk>(object); (void)pp;
    switch (field) {
        case FIELD_msgId: pp->setMsgId(string2long(value)); break;
        case FIELD_sender: pp->setSender((value)); break;
        case FIELD_senderPort: pp->setSenderPort(string2long(value)); break;
        case FIELD_receiver: pp->setReceiver((value)); break;
        case FIELD_receiverPort: pp->setReceiverPort(string2long(value)); break;
        case FIELD_packetSize_B: pp->setPacketSize_B(string2long(value)); break;
        case FIELD_timeSend_ms: pp->setTimeSend_ms(string2long(value)); break;
        case FIELD_reply: pp->setReply(string2bool(value)); break;
        case FIELD_replyAfter: pp->setReplyAfter(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CustomTrafficChunk'", field);
    }
}

omnetpp::cValue CustomTrafficChunkDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CustomTrafficChunk *pp = omnetpp::fromAnyPtr<CustomTrafficChunk>(object); (void)pp;
    switch (field) {
        case FIELD_msgId: return pp->getMsgId();
        case FIELD_sender: return pp->getSender();
        case FIELD_senderPort: return pp->getSenderPort();
        case FIELD_receiver: return pp->getReceiver();
        case FIELD_receiverPort: return pp->getReceiverPort();
        case FIELD_packetSize_B: return pp->getPacketSize_B();
        case FIELD_timeSend_ms: return pp->getTimeSend_ms();
        case FIELD_reply: return pp->getReply();
        case FIELD_replyAfter: return pp->getReplyAfter();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CustomTrafficChunk' as cValue -- field index out of range?", field);
    }
}

void CustomTrafficChunkDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CustomTrafficChunk *pp = omnetpp::fromAnyPtr<CustomTrafficChunk>(object); (void)pp;
    switch (field) {
        case FIELD_msgId: pp->setMsgId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sender: pp->setSender(value.stringValue()); break;
        case FIELD_senderPort: pp->setSenderPort(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_receiver: pp->setReceiver(value.stringValue()); break;
        case FIELD_receiverPort: pp->setReceiverPort(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_packetSize_B: pp->setPacketSize_B(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_timeSend_ms: pp->setTimeSend_ms(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_reply: pp->setReply(value.boolValue()); break;
        case FIELD_replyAfter: pp->setReplyAfter(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CustomTrafficChunk'", field);
    }
}

const char *CustomTrafficChunkDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr CustomTrafficChunkDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CustomTrafficChunk *pp = omnetpp::fromAnyPtr<CustomTrafficChunk>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CustomTrafficChunkDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CustomTrafficChunk *pp = omnetpp::fromAnyPtr<CustomTrafficChunk>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CustomTrafficChunk'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

