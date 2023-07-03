//
// Generated file, do not edit! Created by nedtool 5.6 from messages/MosaikApplicationChunk.msg.
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
#include "MosaikApplicationChunk_m.h"

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

Register_Class(MosaikApplicationChunk)

MosaikApplicationChunk::MosaikApplicationChunk() : ::inet::FieldsChunk()
{
    this->setChunkLength(CHUNK_LENGTH);

}

MosaikApplicationChunk::MosaikApplicationChunk(const MosaikApplicationChunk& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

MosaikApplicationChunk::~MosaikApplicationChunk()
{
}

MosaikApplicationChunk& MosaikApplicationChunk::operator=(const MosaikApplicationChunk& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void MosaikApplicationChunk::copy(const MosaikApplicationChunk& other)
{
    this->creationTime = other.creationTime;
    this->content = other.content;
    this->receiver = other.receiver;
    this->sender = other.sender;
    this->msgId = other.msgId;
    this->creationTimeMosaik = other.creationTimeMosaik;
    this->isTrafficMessage_ = other.isTrafficMessage_;
    this->isFalsified_ = other.isFalsified_;
}

void MosaikApplicationChunk::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->creationTime);
    doParsimPacking(b,this->content);
    doParsimPacking(b,this->receiver);
    doParsimPacking(b,this->sender);
    doParsimPacking(b,this->msgId);
    doParsimPacking(b,this->creationTimeMosaik);
    doParsimPacking(b,this->isTrafficMessage_);
    doParsimPacking(b,this->isFalsified_);
}

void MosaikApplicationChunk::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->creationTime);
    doParsimUnpacking(b,this->content);
    doParsimUnpacking(b,this->receiver);
    doParsimUnpacking(b,this->sender);
    doParsimUnpacking(b,this->msgId);
    doParsimUnpacking(b,this->creationTimeMosaik);
    doParsimUnpacking(b,this->isTrafficMessage_);
    doParsimUnpacking(b,this->isFalsified_);
}

omnetpp::simtime_t MosaikApplicationChunk::getCreationTime() const
{
    return this->creationTime;
}

void MosaikApplicationChunk::setCreationTime(omnetpp::simtime_t creationTime)
{
    handleChange();
    this->creationTime = creationTime;
}

const char * MosaikApplicationChunk::getContent() const
{
    return this->content.c_str();
}

void MosaikApplicationChunk::setContent(const char * content)
{
    handleChange();
    this->content = content;
}

const char * MosaikApplicationChunk::getReceiver() const
{
    return this->receiver.c_str();
}

void MosaikApplicationChunk::setReceiver(const char * receiver)
{
    handleChange();
    this->receiver = receiver;
}

const char * MosaikApplicationChunk::getSender() const
{
    return this->sender.c_str();
}

void MosaikApplicationChunk::setSender(const char * sender)
{
    handleChange();
    this->sender = sender;
}

const char * MosaikApplicationChunk::getMsgId() const
{
    return this->msgId.c_str();
}

void MosaikApplicationChunk::setMsgId(const char * msgId)
{
    handleChange();
    this->msgId = msgId;
}

int MosaikApplicationChunk::getCreationTimeMosaik() const
{
    return this->creationTimeMosaik;
}

void MosaikApplicationChunk::setCreationTimeMosaik(int creationTimeMosaik)
{
    handleChange();
    this->creationTimeMosaik = creationTimeMosaik;
}

bool MosaikApplicationChunk::isTrafficMessage() const
{
    return this->isTrafficMessage_;
}

void MosaikApplicationChunk::setIsTrafficMessage(bool isTrafficMessage)
{
    handleChange();
    this->isTrafficMessage_ = isTrafficMessage;
}

bool MosaikApplicationChunk::isFalsified() const
{
    return this->isFalsified_;
}

void MosaikApplicationChunk::setIsFalsified(bool isFalsified)
{
    handleChange();
    this->isFalsified_ = isFalsified;
}

class MosaikApplicationChunkDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_creationTime,
        FIELD_content,
        FIELD_receiver,
        FIELD_sender,
        FIELD_msgId,
        FIELD_creationTimeMosaik,
        FIELD_isTrafficMessage,
        FIELD_isFalsified,
    };
  public:
    MosaikApplicationChunkDescriptor();
    virtual ~MosaikApplicationChunkDescriptor();

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

Register_ClassDescriptor(MosaikApplicationChunkDescriptor)

MosaikApplicationChunkDescriptor::MosaikApplicationChunkDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(MosaikApplicationChunk)), "inet::FieldsChunk")
{
    propertynames = nullptr;
}

MosaikApplicationChunkDescriptor::~MosaikApplicationChunkDescriptor()
{
    delete[] propertynames;
}

bool MosaikApplicationChunkDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MosaikApplicationChunk *>(obj)!=nullptr;
}

const char **MosaikApplicationChunkDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MosaikApplicationChunkDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MosaikApplicationChunkDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 8+basedesc->getFieldCount() : 8;
}

unsigned int MosaikApplicationChunkDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        0,    // FIELD_creationTime
        FD_ISEDITABLE,    // FIELD_content
        FD_ISEDITABLE,    // FIELD_receiver
        FD_ISEDITABLE,    // FIELD_sender
        FD_ISEDITABLE,    // FIELD_msgId
        FD_ISEDITABLE,    // FIELD_creationTimeMosaik
        FD_ISEDITABLE,    // FIELD_isTrafficMessage
        FD_ISEDITABLE,    // FIELD_isFalsified
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *MosaikApplicationChunkDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "creationTime",
        "content",
        "receiver",
        "sender",
        "msgId",
        "creationTimeMosaik",
        "isTrafficMessage",
        "isFalsified",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int MosaikApplicationChunkDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 'c' && strcmp(fieldName, "creationTime") == 0) return base+0;
    if (fieldName[0] == 'c' && strcmp(fieldName, "content") == 0) return base+1;
    if (fieldName[0] == 'r' && strcmp(fieldName, "receiver") == 0) return base+2;
    if (fieldName[0] == 's' && strcmp(fieldName, "sender") == 0) return base+3;
    if (fieldName[0] == 'm' && strcmp(fieldName, "msgId") == 0) return base+4;
    if (fieldName[0] == 'c' && strcmp(fieldName, "creationTimeMosaik") == 0) return base+5;
    if (fieldName[0] == 'i' && strcmp(fieldName, "isTrafficMessage") == 0) return base+6;
    if (fieldName[0] == 'i' && strcmp(fieldName, "isFalsified") == 0) return base+7;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MosaikApplicationChunkDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "omnetpp::simtime_t",    // FIELD_creationTime
        "string",    // FIELD_content
        "string",    // FIELD_receiver
        "string",    // FIELD_sender
        "string",    // FIELD_msgId
        "int",    // FIELD_creationTimeMosaik
        "bool",    // FIELD_isTrafficMessage
        "bool",    // FIELD_isFalsified
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **MosaikApplicationChunkDescriptor::getFieldPropertyNames(int field) const
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

const char *MosaikApplicationChunkDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int MosaikApplicationChunkDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MosaikApplicationChunk *pp = (MosaikApplicationChunk *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MosaikApplicationChunkDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MosaikApplicationChunk *pp = (MosaikApplicationChunk *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MosaikApplicationChunkDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MosaikApplicationChunk *pp = (MosaikApplicationChunk *)object; (void)pp;
    switch (field) {
        case FIELD_creationTime: return simtime2string(pp->getCreationTime());
        case FIELD_content: return oppstring2string(pp->getContent());
        case FIELD_receiver: return oppstring2string(pp->getReceiver());
        case FIELD_sender: return oppstring2string(pp->getSender());
        case FIELD_msgId: return oppstring2string(pp->getMsgId());
        case FIELD_creationTimeMosaik: return long2string(pp->getCreationTimeMosaik());
        case FIELD_isTrafficMessage: return bool2string(pp->isTrafficMessage());
        case FIELD_isFalsified: return bool2string(pp->isFalsified());
        default: return "";
    }
}

bool MosaikApplicationChunkDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MosaikApplicationChunk *pp = (MosaikApplicationChunk *)object; (void)pp;
    switch (field) {
        case FIELD_content: pp->setContent((value)); return true;
        case FIELD_receiver: pp->setReceiver((value)); return true;
        case FIELD_sender: pp->setSender((value)); return true;
        case FIELD_msgId: pp->setMsgId((value)); return true;
        case FIELD_creationTimeMosaik: pp->setCreationTimeMosaik(string2long(value)); return true;
        case FIELD_isTrafficMessage: pp->setIsTrafficMessage(string2bool(value)); return true;
        case FIELD_isFalsified: pp->setIsFalsified(string2bool(value)); return true;
        default: return false;
    }
}

const char *MosaikApplicationChunkDescriptor::getFieldStructName(int field) const
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

void *MosaikApplicationChunkDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MosaikApplicationChunk *pp = (MosaikApplicationChunk *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

