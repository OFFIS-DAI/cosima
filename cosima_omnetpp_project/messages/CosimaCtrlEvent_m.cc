//
// Generated file, do not edit! Created by opp_msgtool 6.0 from messages/CosimaCtrlEvent.msg.
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
#include "CosimaCtrlEvent_m.h"

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

Register_Class(CosimaCtrlEvent)

CosimaCtrlEvent::CosimaCtrlEvent(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

CosimaCtrlEvent::CosimaCtrlEvent(const CosimaCtrlEvent& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

CosimaCtrlEvent::~CosimaCtrlEvent()
{
    delete [] this->moduleNames;
}

CosimaCtrlEvent& CosimaCtrlEvent::operator=(const CosimaCtrlEvent& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void CosimaCtrlEvent::copy(const CosimaCtrlEvent& other)
{
    this->ctrlType = other.ctrlType;
    this->source = other.source;
    this->destination = other.destination;
    this->start = other.start;
    this->stop = other.stop;
    this->interval = other.interval;
    this->packetLength = other.packetLength;
    delete [] this->moduleNames;
    this->moduleNames = (other.moduleNames_arraysize==0) ? nullptr : new omnetpp::opp_string[other.moduleNames_arraysize];
    moduleNames_arraysize = other.moduleNames_arraysize;
    for (size_t i = 0; i < moduleNames_arraysize; i++) {
        this->moduleNames[i] = other.moduleNames[i];
    }
}

void CosimaCtrlEvent::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->ctrlType);
    doParsimPacking(b,this->source);
    doParsimPacking(b,this->destination);
    doParsimPacking(b,this->start);
    doParsimPacking(b,this->stop);
    doParsimPacking(b,this->interval);
    doParsimPacking(b,this->packetLength);
    b->pack(moduleNames_arraysize);
    doParsimArrayPacking(b,this->moduleNames,moduleNames_arraysize);
}

void CosimaCtrlEvent::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->ctrlType);
    doParsimUnpacking(b,this->source);
    doParsimUnpacking(b,this->destination);
    doParsimUnpacking(b,this->start);
    doParsimUnpacking(b,this->stop);
    doParsimUnpacking(b,this->interval);
    doParsimUnpacking(b,this->packetLength);
    delete [] this->moduleNames;
    b->unpack(moduleNames_arraysize);
    if (moduleNames_arraysize == 0) {
        this->moduleNames = nullptr;
    } else {
        this->moduleNames = new omnetpp::opp_string[moduleNames_arraysize];
        doParsimArrayUnpacking(b,this->moduleNames,moduleNames_arraysize);
    }
}

int CosimaCtrlEvent::getCtrlType() const
{
    return this->ctrlType;
}

void CosimaCtrlEvent::setCtrlType(int ctrlType)
{
    this->ctrlType = ctrlType;
}

const char * CosimaCtrlEvent::getSource() const
{
    return this->source.c_str();
}

void CosimaCtrlEvent::setSource(const char * source)
{
    this->source = source;
}

const char * CosimaCtrlEvent::getDestination() const
{
    return this->destination.c_str();
}

void CosimaCtrlEvent::setDestination(const char * destination)
{
    this->destination = destination;
}

int CosimaCtrlEvent::getStart() const
{
    return this->start;
}

void CosimaCtrlEvent::setStart(int start)
{
    this->start = start;
}

int CosimaCtrlEvent::getStop() const
{
    return this->stop;
}

void CosimaCtrlEvent::setStop(int stop)
{
    this->stop = stop;
}

int CosimaCtrlEvent::getInterval() const
{
    return this->interval;
}

void CosimaCtrlEvent::setInterval(int interval)
{
    this->interval = interval;
}

int CosimaCtrlEvent::getPacketLength() const
{
    return this->packetLength;
}

void CosimaCtrlEvent::setPacketLength(int packetLength)
{
    this->packetLength = packetLength;
}

size_t CosimaCtrlEvent::getModuleNamesArraySize() const
{
    return moduleNames_arraysize;
}

const char * CosimaCtrlEvent::getModuleNames(size_t k) const
{
    if (k >= moduleNames_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)moduleNames_arraysize, (unsigned long)k);
    return this->moduleNames[k].c_str();
}

void CosimaCtrlEvent::setModuleNamesArraySize(size_t newSize)
{
    omnetpp::opp_string *moduleNames2 = (newSize==0) ? nullptr : new omnetpp::opp_string[newSize];
    size_t minSize = moduleNames_arraysize < newSize ? moduleNames_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        moduleNames2[i] = this->moduleNames[i];
    delete [] this->moduleNames;
    this->moduleNames = moduleNames2;
    moduleNames_arraysize = newSize;
}

void CosimaCtrlEvent::setModuleNames(size_t k, const char * moduleNames)
{
    if (k >= moduleNames_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)moduleNames_arraysize, (unsigned long)k);
    this->moduleNames[k] = moduleNames;
}

void CosimaCtrlEvent::insertModuleNames(size_t k, const char * moduleNames)
{
    if (k > moduleNames_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)moduleNames_arraysize, (unsigned long)k);
    size_t newSize = moduleNames_arraysize + 1;
    omnetpp::opp_string *moduleNames2 = new omnetpp::opp_string[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        moduleNames2[i] = this->moduleNames[i];
    moduleNames2[k] = moduleNames;
    for (i = k + 1; i < newSize; i++)
        moduleNames2[i] = this->moduleNames[i-1];
    delete [] this->moduleNames;
    this->moduleNames = moduleNames2;
    moduleNames_arraysize = newSize;
}

void CosimaCtrlEvent::appendModuleNames(const char * moduleNames)
{
    insertModuleNames(moduleNames_arraysize, moduleNames);
}

void CosimaCtrlEvent::eraseModuleNames(size_t k)
{
    if (k >= moduleNames_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)moduleNames_arraysize, (unsigned long)k);
    size_t newSize = moduleNames_arraysize - 1;
    omnetpp::opp_string *moduleNames2 = (newSize == 0) ? nullptr : new omnetpp::opp_string[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        moduleNames2[i] = this->moduleNames[i];
    for (i = k; i < newSize; i++)
        moduleNames2[i] = this->moduleNames[i+1];
    delete [] this->moduleNames;
    this->moduleNames = moduleNames2;
    moduleNames_arraysize = newSize;
}

class CosimaCtrlEventDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_ctrlType,
        FIELD_source,
        FIELD_destination,
        FIELD_start,
        FIELD_stop,
        FIELD_interval,
        FIELD_packetLength,
        FIELD_moduleNames,
    };
  public:
    CosimaCtrlEventDescriptor();
    virtual ~CosimaCtrlEventDescriptor();

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

Register_ClassDescriptor(CosimaCtrlEventDescriptor)

CosimaCtrlEventDescriptor::CosimaCtrlEventDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(CosimaCtrlEvent)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

CosimaCtrlEventDescriptor::~CosimaCtrlEventDescriptor()
{
    delete[] propertyNames;
}

bool CosimaCtrlEventDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CosimaCtrlEvent *>(obj)!=nullptr;
}

const char **CosimaCtrlEventDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CosimaCtrlEventDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CosimaCtrlEventDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int CosimaCtrlEventDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_ctrlType
        FD_ISEDITABLE,    // FIELD_source
        FD_ISEDITABLE,    // FIELD_destination
        FD_ISEDITABLE,    // FIELD_start
        FD_ISEDITABLE,    // FIELD_stop
        FD_ISEDITABLE,    // FIELD_interval
        FD_ISEDITABLE,    // FIELD_packetLength
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_moduleNames
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *CosimaCtrlEventDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "ctrlType",
        "source",
        "destination",
        "start",
        "stop",
        "interval",
        "packetLength",
        "moduleNames",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int CosimaCtrlEventDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "ctrlType") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "source") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "destination") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "start") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "stop") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "interval") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "packetLength") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "moduleNames") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *CosimaCtrlEventDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_ctrlType
        "string",    // FIELD_source
        "string",    // FIELD_destination
        "int",    // FIELD_start
        "int",    // FIELD_stop
        "int",    // FIELD_interval
        "int",    // FIELD_packetLength
        "string",    // FIELD_moduleNames
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **CosimaCtrlEventDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_ctrlType: {
            static const char *names[] = { "enum", "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *CosimaCtrlEventDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_ctrlType:
            if (!strcmp(propertyName, "enum")) return "ControlType";
            if (!strcmp(propertyName, "enum")) return "ControlType";
            return nullptr;
        default: return nullptr;
    }
}

int CosimaCtrlEventDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CosimaCtrlEvent *pp = omnetpp::fromAnyPtr<CosimaCtrlEvent>(object); (void)pp;
    switch (field) {
        case FIELD_moduleNames: return pp->getModuleNamesArraySize();
        default: return 0;
    }
}

void CosimaCtrlEventDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaCtrlEvent *pp = omnetpp::fromAnyPtr<CosimaCtrlEvent>(object); (void)pp;
    switch (field) {
        case FIELD_moduleNames: pp->setModuleNamesArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CosimaCtrlEvent'", field);
    }
}

const char *CosimaCtrlEventDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CosimaCtrlEvent *pp = omnetpp::fromAnyPtr<CosimaCtrlEvent>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CosimaCtrlEventDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CosimaCtrlEvent *pp = omnetpp::fromAnyPtr<CosimaCtrlEvent>(object); (void)pp;
    switch (field) {
        case FIELD_ctrlType: return enum2string(pp->getCtrlType(), "ControlType");
        case FIELD_source: return oppstring2string(pp->getSource());
        case FIELD_destination: return oppstring2string(pp->getDestination());
        case FIELD_start: return long2string(pp->getStart());
        case FIELD_stop: return long2string(pp->getStop());
        case FIELD_interval: return long2string(pp->getInterval());
        case FIELD_packetLength: return long2string(pp->getPacketLength());
        case FIELD_moduleNames: return oppstring2string(pp->getModuleNames(i));
        default: return "";
    }
}

void CosimaCtrlEventDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaCtrlEvent *pp = omnetpp::fromAnyPtr<CosimaCtrlEvent>(object); (void)pp;
    switch (field) {
        case FIELD_ctrlType: pp->setCtrlType((ControlType)string2enum(value, "ControlType")); break;
        case FIELD_source: pp->setSource((value)); break;
        case FIELD_destination: pp->setDestination((value)); break;
        case FIELD_start: pp->setStart(string2long(value)); break;
        case FIELD_stop: pp->setStop(string2long(value)); break;
        case FIELD_interval: pp->setInterval(string2long(value)); break;
        case FIELD_packetLength: pp->setPacketLength(string2long(value)); break;
        case FIELD_moduleNames: pp->setModuleNames(i,(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CosimaCtrlEvent'", field);
    }
}

omnetpp::cValue CosimaCtrlEventDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CosimaCtrlEvent *pp = omnetpp::fromAnyPtr<CosimaCtrlEvent>(object); (void)pp;
    switch (field) {
        case FIELD_ctrlType: return pp->getCtrlType();
        case FIELD_source: return pp->getSource();
        case FIELD_destination: return pp->getDestination();
        case FIELD_start: return pp->getStart();
        case FIELD_stop: return pp->getStop();
        case FIELD_interval: return pp->getInterval();
        case FIELD_packetLength: return pp->getPacketLength();
        case FIELD_moduleNames: return pp->getModuleNames(i);
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CosimaCtrlEvent' as cValue -- field index out of range?", field);
    }
}

void CosimaCtrlEventDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaCtrlEvent *pp = omnetpp::fromAnyPtr<CosimaCtrlEvent>(object); (void)pp;
    switch (field) {
        case FIELD_ctrlType: pp->setCtrlType((ControlType)value.intValue()); break;
        case FIELD_source: pp->setSource(value.stringValue()); break;
        case FIELD_destination: pp->setDestination(value.stringValue()); break;
        case FIELD_start: pp->setStart(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_stop: pp->setStop(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_interval: pp->setInterval(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_packetLength: pp->setPacketLength(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_moduleNames: pp->setModuleNames(i,value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CosimaCtrlEvent'", field);
    }
}

const char *CosimaCtrlEventDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr CosimaCtrlEventDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CosimaCtrlEvent *pp = omnetpp::fromAnyPtr<CosimaCtrlEvent>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CosimaCtrlEventDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CosimaCtrlEvent *pp = omnetpp::fromAnyPtr<CosimaCtrlEvent>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CosimaCtrlEvent'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

