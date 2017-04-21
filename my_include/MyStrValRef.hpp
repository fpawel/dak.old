//-----------------------------------------------------------------------------/
#ifndef MY_STRING_VALUE_REFERENCE_HEADER_INCLUDED________
#define MY_STRING_VALUE_REFERENCE_HEADER_INCLUDED________
//-----------------------------------------------------------------------------/
#include <system.hpp>
//-----------------------------------------------------------------------------/
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
//-----------------------------------------------------------------------------/
#include "AnsiStringUtils_.h"
//----------------------------------------------------------------------------//
class MyStrValRef : public boost::noncopyable
{
public:
    explicit MyStrValRef() : s_() {}
	virtual AnsiString Get() const = 0;
    AnsiString GetStr() const { return s_; }
    void Set (const AnsiString& s)
    {
        s_ = s;
        UpdateReferenceValue();
    }
    virtual ~MyStrValRef() {}
private:
    AnsiString s_;

    virtual void UpdateReferenceValue() = 0;
};
//----------------------------------------------------------------------------//
typedef boost::shared_ptr<MyStrValRef> PMyStrValRef;
//----------------------------------------------------------------------------//
template<typename IntegerType>
class MyIntStrValRef : public MyStrValRef
{
public:
    explicit MyIntStrValRef(IntegerType& ref) :
        ref_(ref)
    {}
    virtual AnsiString Get() const { return ref_; }
private:
    IntegerType &ref_;
    virtual void UpdateReferenceValue()
    {
        ref_ = StrToIntDef( GetStr(), 0 );
    }
};
//----------------------------------------------------------------------------//
template<typename IntegerType>
MyIntStrValRef<IntegerType>* CreateMyIntStrValRef(IntegerType& ref)
{
    return new MyIntStrValRef<IntegerType>(ref);
}
//----------------------------------------------------------------------------//
class MyStrStrValRef : public MyStrValRef
{
public:
    explicit MyStrStrValRef(AnsiString& ref) : ref_(ref) {}
    virtual AnsiString Get() const { return ref_; }
private:
    AnsiString &ref_;
    virtual void UpdateReferenceValue()
    {
        ref_ = GetStr();
    }
};
//-----------------------------------------------------------------------------/
template<typename RealType>
class MyFloatStrValRef : public MyStrValRef
{
public:
    explicit MyFloatStrValRef(RealType& ref) : ref_(ref) {}
    virtual AnsiString Get() const { return ref_; }
private:
    RealType &ref_;
    virtual void UpdateReferenceValue()
    {
        ref_ = MyStrToFloatDef( GetStr(), 0 );
    }
};
//-----------------------------------------------------------------------------/
template<typename RealType>
MyFloatStrValRef<RealType>* CreateMyFloatStrValRef(RealType& ref)
{
    return new MyFloatStrValRef<RealType>(ref);
}
//-----------------------------------------------------------------------------/
class MyStrValRefStr : public MyStrValRef
{
public:
    explicit MyStrValRefStr(AnsiString& ref) : ref_(ref)
    {}
    virtual AnsiString Get() const { return ref_; }
private:
    AnsiString &ref_;
    virtual void UpdateReferenceValue()
    {
        ref_ = GetStr();
    }
};
#endif