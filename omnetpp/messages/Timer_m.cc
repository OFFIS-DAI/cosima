//
// Generated file, do not edit! Created by opp_msgtool 6.0 from messages/Timer.msg.
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
#include "Timer_m.h"

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

Register_Class(Timer)

Timer::Timer(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

Timer::Timer(const Timer& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

Timer::~Timer()
{
}

Timer& Timer::operator=(const Timer& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Timer::copy(const Timer& other)
{
    this->timerType = other.timerType;
    this->receiverPort = other.receiverPort;
    this->receiverName = other.receiverName;
    this->messageId = other.messageId;
}

void Timer::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->timerType);
    doParsimPacking(b,this->receiverPort);
    doParsimPacking(b,this->receiverName);
    doParsimPacking(b,this->messageId);
}

void Timer::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->timerType);
    doParsimUnpacking(b,this->receiverPort);
    doParsimUnpacking(b,this->receiverName);
    doParsimUnpacking(b,this->messageId);
}

int Timer::getTimerType() const
{
    return this->timerType;
}

void Timer::setTimerType(int timerType)
{
    this->timerType = timerType;
}

int Timer::getReceiverPort() const
{
    return this->receiverPort;
}

void Timer::setReceiverPort(int receiverPort)
{
    this->receiverPort = receiverPort;
}

const char * Timer::getReceiverName() const
{
    return this->receiverName.c_str();
}

void Timer::setReceiverName(const char * receiverName)
{
    this->receiverName = receiverName;
}

const char * Timer::getMessageId() const
{
    return this->messageId.c_str();
}

void Timer::setMessageId(const char * messageId)
{
    this->messageId = messageId;
}

class TimerDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_timerType,
        FIELD_receiverPort,
        FIELD_receiverName,
        FIELD_messageId,
    };
  public:
    TimerDescriptor();
    virtual ~TimerDescriptor();

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

Register_ClassDescriptor(TimerDescriptor)

TimerDescriptor::TimerDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Timer)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

TimerDescriptor::~TimerDescriptor()
{
    delete[] propertyNames;
}

bool TimerDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Timer *>(obj)!=nullptr;
}

const char **TimerDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TimerDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TimerDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int TimerDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_timerType
        FD_ISEDITABLE,    // FIELD_receiverPort
        FD_ISEDITABLE,    // FIELD_receiverName
        FD_ISEDITABLE,    // FIELD_messageId
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *TimerDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "timerType",
        "receiverPort",
        "receiverName",
        "messageId",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int TimerDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "timerType") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "receiverPort") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "receiverName") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "messageId") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *TimerDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_timerType
        "int",    // FIELD_receiverPort
        "string",    // FIELD_receiverName
        "string",    // FIELD_messageId
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **TimerDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_timerType: {
            static const char *names[] = { "enum", "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *TimerDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_timerType:
            if (!strcmp(propertyName, "enum")) return "TimerType";
            if (!strcmp(propertyName, "enum")) return "TimerType";
            return nullptr;
        default: return nullptr;
    }
}

int TimerDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Timer *pp = omnetpp::fromAnyPtr<Timer>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void TimerDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Timer *pp = omnetpp::fromAnyPtr<Timer>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Timer'", field);
    }
}

const char *TimerDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Timer *pp = omnetpp::fromAnyPtr<Timer>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TimerDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Timer *pp = omnetpp::fromAnyPtr<Timer>(object); (void)pp;
    switch (field) {
        case FIELD_timerType: return enum2string(pp->getTimerType(), "TimerType");
        case FIELD_receiverPort: return long2string(pp->getReceiverPort());
        case FIELD_receiverName: return oppstring2string(pp->getReceiverName());
        case FIELD_messageId: return oppstring2string(pp->getMessageId());
        default: return "";
    }
}

void TimerDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Timer *pp = omnetpp::fromAnyPtr<Timer>(object); (void)pp;
    switch (field) {
        case FIELD_timerType: pp->setTimerType((TimerType)string2enum(value, "TimerType")); break;
        case FIELD_receiverPort: pp->setReceiverPort(string2long(value)); break;
        case FIELD_receiverName: pp->setReceiverName((value)); break;
        case FIELD_messageId: pp->setMessageId((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Timer'", field);
    }
}

omnetpp::cValue TimerDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Timer *pp = omnetpp::fromAnyPtr<Timer>(object); (void)pp;
    switch (field) {
        case FIELD_timerType: return pp->getTimerType();
        case FIELD_receiverPort: return pp->getReceiverPort();
        case FIELD_receiverName: return pp->getReceiverName();
        case FIELD_messageId: return pp->getMessageId();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Timer' as cValue -- field index out of range?", field);
    }
}

void TimerDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Timer *pp = omnetpp::fromAnyPtr<Timer>(object); (void)pp;
    switch (field) {
        case FIELD_timerType: pp->setTimerType((TimerType)value.intValue()); break;
        case FIELD_receiverPort: pp->setReceiverPort(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_receiverName: pp->setReceiverName(value.stringValue()); break;
        case FIELD_messageId: pp->setMessageId(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Timer'", field);
    }
}

const char *TimerDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr TimerDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Timer *pp = omnetpp::fromAnyPtr<Timer>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TimerDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Timer *pp = omnetpp::fromAnyPtr<Timer>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Timer'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

