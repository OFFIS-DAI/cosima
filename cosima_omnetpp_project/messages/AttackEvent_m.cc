//
// Generated file, do not edit! Created by opp_msgtool 6.0 from messages/AttackEvent.msg.
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
#include "AttackEvent_m.h"

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

Register_Class(AttackEvent)

AttackEvent::AttackEvent(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

AttackEvent::AttackEvent(const AttackEvent& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

AttackEvent::~AttackEvent()
{
}

AttackEvent& AttackEvent::operator=(const AttackEvent& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void AttackEvent::copy(const AttackEvent& other)
{
    this->attackType = other.attackType;
    this->attacked_module = other.attacked_module;
    this->start = other.start;
    this->stop = other.stop;
    this->probability = other.probability;
}

void AttackEvent::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->attackType);
    doParsimPacking(b,this->attacked_module);
    doParsimPacking(b,this->start);
    doParsimPacking(b,this->stop);
    doParsimPacking(b,this->probability);
}

void AttackEvent::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->attackType);
    doParsimUnpacking(b,this->attacked_module);
    doParsimUnpacking(b,this->start);
    doParsimUnpacking(b,this->stop);
    doParsimUnpacking(b,this->probability);
}

int AttackEvent::getAttackType() const
{
    return this->attackType;
}

void AttackEvent::setAttackType(int attackType)
{
    this->attackType = attackType;
}

const char * AttackEvent::getAttacked_module() const
{
    return this->attacked_module.c_str();
}

void AttackEvent::setAttacked_module(const char * attacked_module)
{
    this->attacked_module = attacked_module;
}

int AttackEvent::getStart() const
{
    return this->start;
}

void AttackEvent::setStart(int start)
{
    this->start = start;
}

int AttackEvent::getStop() const
{
    return this->stop;
}

void AttackEvent::setStop(int stop)
{
    this->stop = stop;
}

double AttackEvent::getProbability() const
{
    return this->probability;
}

void AttackEvent::setProbability(double probability)
{
    this->probability = probability;
}

class AttackEventDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_attackType,
        FIELD_attacked_module,
        FIELD_start,
        FIELD_stop,
        FIELD_probability,
    };
  public:
    AttackEventDescriptor();
    virtual ~AttackEventDescriptor();

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

Register_ClassDescriptor(AttackEventDescriptor)

AttackEventDescriptor::AttackEventDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(AttackEvent)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

AttackEventDescriptor::~AttackEventDescriptor()
{
    delete[] propertyNames;
}

bool AttackEventDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<AttackEvent *>(obj)!=nullptr;
}

const char **AttackEventDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *AttackEventDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int AttackEventDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int AttackEventDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_attackType
        FD_ISEDITABLE,    // FIELD_attacked_module
        FD_ISEDITABLE,    // FIELD_start
        FD_ISEDITABLE,    // FIELD_stop
        FD_ISEDITABLE,    // FIELD_probability
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *AttackEventDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "attackType",
        "attacked_module",
        "start",
        "stop",
        "probability",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int AttackEventDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "attackType") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "attacked_module") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "start") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "stop") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "probability") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *AttackEventDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_attackType
        "string",    // FIELD_attacked_module
        "int",    // FIELD_start
        "int",    // FIELD_stop
        "double",    // FIELD_probability
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **AttackEventDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_attackType: {
            static const char *names[] = { "enum", "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *AttackEventDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_attackType:
            if (!strcmp(propertyName, "enum")) return "AttackType";
            if (!strcmp(propertyName, "enum")) return "AttackType";
            return nullptr;
        default: return nullptr;
    }
}

int AttackEventDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    AttackEvent *pp = omnetpp::fromAnyPtr<AttackEvent>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void AttackEventDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    AttackEvent *pp = omnetpp::fromAnyPtr<AttackEvent>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'AttackEvent'", field);
    }
}

const char *AttackEventDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    AttackEvent *pp = omnetpp::fromAnyPtr<AttackEvent>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string AttackEventDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    AttackEvent *pp = omnetpp::fromAnyPtr<AttackEvent>(object); (void)pp;
    switch (field) {
        case FIELD_attackType: return enum2string(pp->getAttackType(), "AttackType");
        case FIELD_attacked_module: return oppstring2string(pp->getAttacked_module());
        case FIELD_start: return long2string(pp->getStart());
        case FIELD_stop: return long2string(pp->getStop());
        case FIELD_probability: return double2string(pp->getProbability());
        default: return "";
    }
}

void AttackEventDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    AttackEvent *pp = omnetpp::fromAnyPtr<AttackEvent>(object); (void)pp;
    switch (field) {
        case FIELD_attackType: pp->setAttackType((AttackType)string2enum(value, "AttackType")); break;
        case FIELD_attacked_module: pp->setAttacked_module((value)); break;
        case FIELD_start: pp->setStart(string2long(value)); break;
        case FIELD_stop: pp->setStop(string2long(value)); break;
        case FIELD_probability: pp->setProbability(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AttackEvent'", field);
    }
}

omnetpp::cValue AttackEventDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    AttackEvent *pp = omnetpp::fromAnyPtr<AttackEvent>(object); (void)pp;
    switch (field) {
        case FIELD_attackType: return pp->getAttackType();
        case FIELD_attacked_module: return pp->getAttacked_module();
        case FIELD_start: return pp->getStart();
        case FIELD_stop: return pp->getStop();
        case FIELD_probability: return pp->getProbability();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'AttackEvent' as cValue -- field index out of range?", field);
    }
}

void AttackEventDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    AttackEvent *pp = omnetpp::fromAnyPtr<AttackEvent>(object); (void)pp;
    switch (field) {
        case FIELD_attackType: pp->setAttackType((AttackType)value.intValue()); break;
        case FIELD_attacked_module: pp->setAttacked_module(value.stringValue()); break;
        case FIELD_start: pp->setStart(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_stop: pp->setStop(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_probability: pp->setProbability(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AttackEvent'", field);
    }
}

const char *AttackEventDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr AttackEventDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    AttackEvent *pp = omnetpp::fromAnyPtr<AttackEvent>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void AttackEventDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    AttackEvent *pp = omnetpp::fromAnyPtr<AttackEvent>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AttackEvent'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

