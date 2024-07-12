//
// Generated file, do not edit! Created by opp_msgtool 6.0 from messages/CosimaApplicationChunk.msg.
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
#include "CosimaApplicationChunk_m.h"

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

Register_Class(CosimaApplicationChunk)

CosimaApplicationChunk::CosimaApplicationChunk() : ::inet::FieldsChunk()
{
    this->setChunkLength(CHUNK_LENGTH);

}

CosimaApplicationChunk::CosimaApplicationChunk(const CosimaApplicationChunk& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CosimaApplicationChunk::~CosimaApplicationChunk()
{
}

CosimaApplicationChunk& CosimaApplicationChunk::operator=(const CosimaApplicationChunk& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CosimaApplicationChunk::copy(const CosimaApplicationChunk& other)
{
    this->creationTimeOmnetpp = other.creationTimeOmnetpp;
    this->content = other.content;
    this->receiver = other.receiver;
    this->sender = other.sender;
    this->msgId = other.msgId;
    this->creationTimeCoupling = other.creationTimeCoupling;
    this->isTrafficMessage_ = other.isTrafficMessage_;
    this->isFalsified_ = other.isFalsified_;
}

void CosimaApplicationChunk::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->creationTimeOmnetpp);
    doParsimPacking(b,this->content);
    doParsimPacking(b,this->receiver);
    doParsimPacking(b,this->sender);
    doParsimPacking(b,this->msgId);
    doParsimPacking(b,this->creationTimeCoupling);
    doParsimPacking(b,this->isTrafficMessage_);
    doParsimPacking(b,this->isFalsified_);
}

void CosimaApplicationChunk::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->creationTimeOmnetpp);
    doParsimUnpacking(b,this->content);
    doParsimUnpacking(b,this->receiver);
    doParsimUnpacking(b,this->sender);
    doParsimUnpacking(b,this->msgId);
    doParsimUnpacking(b,this->creationTimeCoupling);
    doParsimUnpacking(b,this->isTrafficMessage_);
    doParsimUnpacking(b,this->isFalsified_);
}

omnetpp::simtime_t CosimaApplicationChunk::getCreationTimeOmnetpp() const
{
    return this->creationTimeOmnetpp;
}

void CosimaApplicationChunk::setCreationTimeOmnetpp(omnetpp::simtime_t creationTimeOmnetpp)
{
    handleChange();
    this->creationTimeOmnetpp = creationTimeOmnetpp;
}

const char * CosimaApplicationChunk::getContent() const
{
    return this->content.c_str();
}

void CosimaApplicationChunk::setContent(const char * content)
{
    handleChange();
    this->content = content;
}

const char * CosimaApplicationChunk::getReceiver() const
{
    return this->receiver.c_str();
}

void CosimaApplicationChunk::setReceiver(const char * receiver)
{
    handleChange();
    this->receiver = receiver;
}

const char * CosimaApplicationChunk::getSender() const
{
    return this->sender.c_str();
}

void CosimaApplicationChunk::setSender(const char * sender)
{
    handleChange();
    this->sender = sender;
}

const char * CosimaApplicationChunk::getMsgId() const
{
    return this->msgId.c_str();
}

void CosimaApplicationChunk::setMsgId(const char * msgId)
{
    handleChange();
    this->msgId = msgId;
}

int CosimaApplicationChunk::getCreationTimeCoupling() const
{
    return this->creationTimeCoupling;
}

void CosimaApplicationChunk::setCreationTimeCoupling(int creationTimeCoupling)
{
    handleChange();
    this->creationTimeCoupling = creationTimeCoupling;
}

bool CosimaApplicationChunk::isTrafficMessage() const
{
    return this->isTrafficMessage_;
}

void CosimaApplicationChunk::setIsTrafficMessage(bool isTrafficMessage)
{
    handleChange();
    this->isTrafficMessage_ = isTrafficMessage;
}

bool CosimaApplicationChunk::isFalsified() const
{
    return this->isFalsified_;
}

void CosimaApplicationChunk::setIsFalsified(bool isFalsified)
{
    handleChange();
    this->isFalsified_ = isFalsified;
}

class CosimaApplicationChunkDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_creationTimeOmnetpp,
        FIELD_content,
        FIELD_receiver,
        FIELD_sender,
        FIELD_msgId,
        FIELD_creationTimeCoupling,
        FIELD_isTrafficMessage,
        FIELD_isFalsified,
    };
  public:
    CosimaApplicationChunkDescriptor();
    virtual ~CosimaApplicationChunkDescriptor();

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

Register_ClassDescriptor(CosimaApplicationChunkDescriptor)

CosimaApplicationChunkDescriptor::CosimaApplicationChunkDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(CosimaApplicationChunk)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CosimaApplicationChunkDescriptor::~CosimaApplicationChunkDescriptor()
{
    delete[] propertyNames;
}

bool CosimaApplicationChunkDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CosimaApplicationChunk *>(obj)!=nullptr;
}

const char **CosimaApplicationChunkDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CosimaApplicationChunkDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CosimaApplicationChunkDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int CosimaApplicationChunkDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_creationTimeOmnetpp
        FD_ISEDITABLE,    // FIELD_content
        FD_ISEDITABLE,    // FIELD_receiver
        FD_ISEDITABLE,    // FIELD_sender
        FD_ISEDITABLE,    // FIELD_msgId
        FD_ISEDITABLE,    // FIELD_creationTimeCoupling
        FD_ISEDITABLE,    // FIELD_isTrafficMessage
        FD_ISEDITABLE,    // FIELD_isFalsified
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *CosimaApplicationChunkDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "creationTimeOmnetpp",
        "content",
        "receiver",
        "sender",
        "msgId",
        "creationTimeCoupling",
        "isTrafficMessage",
        "isFalsified",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int CosimaApplicationChunkDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "creationTimeOmnetpp") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "content") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "receiver") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "sender") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "msgId") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "creationTimeCoupling") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "isTrafficMessage") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "isFalsified") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *CosimaApplicationChunkDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "omnetpp::simtime_t",    // FIELD_creationTimeOmnetpp
        "string",    // FIELD_content
        "string",    // FIELD_receiver
        "string",    // FIELD_sender
        "string",    // FIELD_msgId
        "int",    // FIELD_creationTimeCoupling
        "bool",    // FIELD_isTrafficMessage
        "bool",    // FIELD_isFalsified
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **CosimaApplicationChunkDescriptor::getFieldPropertyNames(int field) const
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

const char *CosimaApplicationChunkDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int CosimaApplicationChunkDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CosimaApplicationChunk *pp = omnetpp::fromAnyPtr<CosimaApplicationChunk>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CosimaApplicationChunkDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaApplicationChunk *pp = omnetpp::fromAnyPtr<CosimaApplicationChunk>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CosimaApplicationChunk'", field);
    }
}

const char *CosimaApplicationChunkDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CosimaApplicationChunk *pp = omnetpp::fromAnyPtr<CosimaApplicationChunk>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CosimaApplicationChunkDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CosimaApplicationChunk *pp = omnetpp::fromAnyPtr<CosimaApplicationChunk>(object); (void)pp;
    switch (field) {
        case FIELD_creationTimeOmnetpp: return simtime2string(pp->getCreationTimeOmnetpp());
        case FIELD_content: return oppstring2string(pp->getContent());
        case FIELD_receiver: return oppstring2string(pp->getReceiver());
        case FIELD_sender: return oppstring2string(pp->getSender());
        case FIELD_msgId: return oppstring2string(pp->getMsgId());
        case FIELD_creationTimeCoupling: return long2string(pp->getCreationTimeCoupling());
        case FIELD_isTrafficMessage: return bool2string(pp->isTrafficMessage());
        case FIELD_isFalsified: return bool2string(pp->isFalsified());
        default: return "";
    }
}

void CosimaApplicationChunkDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaApplicationChunk *pp = omnetpp::fromAnyPtr<CosimaApplicationChunk>(object); (void)pp;
    switch (field) {
        case FIELD_creationTimeOmnetpp: pp->setCreationTimeOmnetpp(string2simtime(value)); break;
        case FIELD_content: pp->setContent((value)); break;
        case FIELD_receiver: pp->setReceiver((value)); break;
        case FIELD_sender: pp->setSender((value)); break;
        case FIELD_msgId: pp->setMsgId((value)); break;
        case FIELD_creationTimeCoupling: pp->setCreationTimeCoupling(string2long(value)); break;
        case FIELD_isTrafficMessage: pp->setIsTrafficMessage(string2bool(value)); break;
        case FIELD_isFalsified: pp->setIsFalsified(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CosimaApplicationChunk'", field);
    }
}

omnetpp::cValue CosimaApplicationChunkDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CosimaApplicationChunk *pp = omnetpp::fromAnyPtr<CosimaApplicationChunk>(object); (void)pp;
    switch (field) {
        case FIELD_creationTimeOmnetpp: return pp->getCreationTimeOmnetpp().dbl();
        case FIELD_content: return pp->getContent();
        case FIELD_receiver: return pp->getReceiver();
        case FIELD_sender: return pp->getSender();
        case FIELD_msgId: return pp->getMsgId();
        case FIELD_creationTimeCoupling: return pp->getCreationTimeCoupling();
        case FIELD_isTrafficMessage: return pp->isTrafficMessage();
        case FIELD_isFalsified: return pp->isFalsified();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CosimaApplicationChunk' as cValue -- field index out of range?", field);
    }
}

void CosimaApplicationChunkDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaApplicationChunk *pp = omnetpp::fromAnyPtr<CosimaApplicationChunk>(object); (void)pp;
    switch (field) {
        case FIELD_creationTimeOmnetpp: pp->setCreationTimeOmnetpp(value.doubleValue()); break;
        case FIELD_content: pp->setContent(value.stringValue()); break;
        case FIELD_receiver: pp->setReceiver(value.stringValue()); break;
        case FIELD_sender: pp->setSender(value.stringValue()); break;
        case FIELD_msgId: pp->setMsgId(value.stringValue()); break;
        case FIELD_creationTimeCoupling: pp->setCreationTimeCoupling(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_isTrafficMessage: pp->setIsTrafficMessage(value.boolValue()); break;
        case FIELD_isFalsified: pp->setIsFalsified(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CosimaApplicationChunk'", field);
    }
}

const char *CosimaApplicationChunkDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr CosimaApplicationChunkDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CosimaApplicationChunk *pp = omnetpp::fromAnyPtr<CosimaApplicationChunk>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CosimaApplicationChunkDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaApplicationChunk *pp = omnetpp::fromAnyPtr<CosimaApplicationChunk>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CosimaApplicationChunk'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

