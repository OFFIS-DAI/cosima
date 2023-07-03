//
// Generated file, do not edit! Created by nedtool 5.6 from messages/MosaikCtrlEvent.msg.
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
#include "MosaikCtrlEvent_m.h"

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

Register_Class(MosaikCtrlEvent)

MosaikCtrlEvent::MosaikCtrlEvent(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

MosaikCtrlEvent::MosaikCtrlEvent(const MosaikCtrlEvent& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

MosaikCtrlEvent::~MosaikCtrlEvent()
{
    delete [] this->moduleNames;
}

MosaikCtrlEvent& MosaikCtrlEvent::operator=(const MosaikCtrlEvent& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void MosaikCtrlEvent::copy(const MosaikCtrlEvent& other)
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

void MosaikCtrlEvent::parsimPack(omnetpp::cCommBuffer *b) const
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

void MosaikCtrlEvent::parsimUnpack(omnetpp::cCommBuffer *b)
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

int MosaikCtrlEvent::getCtrlType() const
{
    return this->ctrlType;
}

void MosaikCtrlEvent::setCtrlType(int ctrlType)
{
    this->ctrlType = ctrlType;
}

const char * MosaikCtrlEvent::getSource() const
{
    return this->source.c_str();
}

void MosaikCtrlEvent::setSource(const char * source)
{
    this->source = source;
}

const char * MosaikCtrlEvent::getDestination() const
{
    return this->destination.c_str();
}

void MosaikCtrlEvent::setDestination(const char * destination)
{
    this->destination = destination;
}

int MosaikCtrlEvent::getStart() const
{
    return this->start;
}

void MosaikCtrlEvent::setStart(int start)
{
    this->start = start;
}

int MosaikCtrlEvent::getStop() const
{
    return this->stop;
}

void MosaikCtrlEvent::setStop(int stop)
{
    this->stop = stop;
}

int MosaikCtrlEvent::getInterval() const
{
    return this->interval;
}

void MosaikCtrlEvent::setInterval(int interval)
{
    this->interval = interval;
}

int MosaikCtrlEvent::getPacketLength() const
{
    return this->packetLength;
}

void MosaikCtrlEvent::setPacketLength(int packetLength)
{
    this->packetLength = packetLength;
}

size_t MosaikCtrlEvent::getModuleNamesArraySize() const
{
    return moduleNames_arraysize;
}

const char * MosaikCtrlEvent::getModuleNames(size_t k) const
{
    if (k >= moduleNames_arraysize) throw omnetpp::cRuntimeError("Array of size moduleNames_arraysize indexed by %lu", (unsigned long)k);
    return this->moduleNames[k].c_str();
}

void MosaikCtrlEvent::setModuleNamesArraySize(size_t newSize)
{
    omnetpp::opp_string *moduleNames2 = (newSize==0) ? nullptr : new omnetpp::opp_string[newSize];
    size_t minSize = moduleNames_arraysize < newSize ? moduleNames_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        moduleNames2[i] = this->moduleNames[i];
    delete [] this->moduleNames;
    this->moduleNames = moduleNames2;
    moduleNames_arraysize = newSize;
}

void MosaikCtrlEvent::setModuleNames(size_t k, const char * moduleNames)
{
    if (k >= moduleNames_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
    this->moduleNames[k] = moduleNames;
}

void MosaikCtrlEvent::insertModuleNames(size_t k, const char * moduleNames)
{
    if (k > moduleNames_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
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

void MosaikCtrlEvent::insertModuleNames(const char * moduleNames)
{
    insertModuleNames(moduleNames_arraysize, moduleNames);
}

void MosaikCtrlEvent::eraseModuleNames(size_t k)
{
    if (k >= moduleNames_arraysize) throw omnetpp::cRuntimeError("Array of size  indexed by %lu", (unsigned long)k);
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

class MosaikCtrlEventDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
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
    MosaikCtrlEventDescriptor();
    virtual ~MosaikCtrlEventDescriptor();

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

Register_ClassDescriptor(MosaikCtrlEventDescriptor)

MosaikCtrlEventDescriptor::MosaikCtrlEventDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(MosaikCtrlEvent)), "omnetpp::cPacket")
{
    propertynames = nullptr;
}

MosaikCtrlEventDescriptor::~MosaikCtrlEventDescriptor()
{
    delete[] propertynames;
}

bool MosaikCtrlEventDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MosaikCtrlEvent *>(obj)!=nullptr;
}

const char **MosaikCtrlEventDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MosaikCtrlEventDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MosaikCtrlEventDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 8+basedesc->getFieldCount() : 8;
}

unsigned int MosaikCtrlEventDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_ctrlType
        FD_ISEDITABLE,    // FIELD_source
        FD_ISEDITABLE,    // FIELD_destination
        FD_ISEDITABLE,    // FIELD_start
        FD_ISEDITABLE,    // FIELD_stop
        FD_ISEDITABLE,    // FIELD_interval
        FD_ISEDITABLE,    // FIELD_packetLength
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_moduleNames
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *MosaikCtrlEventDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
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

int MosaikCtrlEventDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'c' && strcmp(fieldName, "ctrlType") == 0) return base+0;
    if (fieldName[0] == 's' && strcmp(fieldName, "source") == 0) return base+1;
    if (fieldName[0] == 'd' && strcmp(fieldName, "destination") == 0) return base+2;
    if (fieldName[0] == 's' && strcmp(fieldName, "start") == 0) return base+3;
    if (fieldName[0] == 's' && strcmp(fieldName, "stop") == 0) return base+4;
    if (fieldName[0] == 'i' && strcmp(fieldName, "interval") == 0) return base+5;
    if (fieldName[0] == 'p' && strcmp(fieldName, "packetLength") == 0) return base+6;
    if (fieldName[0] == 'm' && strcmp(fieldName, "moduleNames") == 0) return base+7;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MosaikCtrlEventDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
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

const char **MosaikCtrlEventDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case FIELD_ctrlType: {
            static const char *names[] = { "enum", "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *MosaikCtrlEventDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case FIELD_ctrlType:
            if (!strcmp(propertyname, "enum")) return "ControlType";
            if (!strcmp(propertyname, "enum")) return "ControlType";
            return nullptr;
        default: return nullptr;
    }
}

int MosaikCtrlEventDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MosaikCtrlEvent *pp = (MosaikCtrlEvent *)object; (void)pp;
    switch (field) {
        case FIELD_moduleNames: return pp->getModuleNamesArraySize();
        default: return 0;
    }
}

const char *MosaikCtrlEventDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MosaikCtrlEvent *pp = (MosaikCtrlEvent *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MosaikCtrlEventDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MosaikCtrlEvent *pp = (MosaikCtrlEvent *)object; (void)pp;
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

bool MosaikCtrlEventDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MosaikCtrlEvent *pp = (MosaikCtrlEvent *)object; (void)pp;
    switch (field) {
        case FIELD_ctrlType: pp->setCtrlType((ControlType)string2enum(value, "ControlType")); return true;
        case FIELD_source: pp->setSource((value)); return true;
        case FIELD_destination: pp->setDestination((value)); return true;
        case FIELD_start: pp->setStart(string2long(value)); return true;
        case FIELD_stop: pp->setStop(string2long(value)); return true;
        case FIELD_interval: pp->setInterval(string2long(value)); return true;
        case FIELD_packetLength: pp->setPacketLength(string2long(value)); return true;
        case FIELD_moduleNames: pp->setModuleNames(i,(value)); return true;
        default: return false;
    }
}

const char *MosaikCtrlEventDescriptor::getFieldStructName(int field) const
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

void *MosaikCtrlEventDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MosaikCtrlEvent *pp = (MosaikCtrlEvent *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

