#pragma once

#include <memory>

class ReferenceBase
{
protected:
    int ref_id;
    std::shared_ptr<void> ref;
public:
    ReferenceBase(int id = 0) : ref_id(id) {}

    ReferenceBase& operator=(int id) {
        ref_id = id;
        return *this;
    }

    template<typename RecordType>
    ReferenceBase& operator=(const RecordType& ref) {
        ref_id = ref->Id->Value();
        return *this;
    }

    bool operator==(const ReferenceBase& other) const {
        return ref_id == other.ref_id;
    }

    int id() const { return ref_id; }

    template<typename RecordType>
    std::shared_ptr<RecordType> Record() const {
        if(ref.get() && ref_id){
           ref = RecordType::Open(ref_id);
        }
        return std::reinterpret_pointer_cast<RecordType>(ref);
    }
};

template<typename CurrentClass>
class Reference : public ReferenceBase
{
public:
    std::shared_ptr<CurrentClass> Value() {
        if(!ref.get() && ref_id){
           ref = CurrentClass::Open(ref_id);
        }
        return std::reinterpret_pointer_cast<CurrentClass>(ref);
    }
    void Value(const std::shared_ptr<CurrentClass>& ref) {
        this->ref = ref;
        ref_id = ref->Id->Value();
    }
    Reference<CurrentClass>& operator=(const std::shared_ptr<CurrentClass>& ref) {
        this->ref = ref;
        ref_id = ref->Id->Value();
        return *this;
    }
};