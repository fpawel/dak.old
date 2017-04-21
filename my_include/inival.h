#ifndef MY_INI_VALUE_HEADER_INCLUDED
#define MY_INI_VALUE_HEADER_INCLUDED

#include <inifiles.hpp>
#include <utility>

typedef std::pair<TIniFile*,AnsiString> IniSekt;
typedef std::pair<IniSekt,AnsiString> IniItem;
#define INIITEM_INI first.first
#define INIITEM_SEKT first.second
#define INIITEM_KEY second

void operator<<(const IniItem& itm, int val)
{
    itm.INIITEM_INI->WriteInteger( itm.INIITEM_SEKT, itm.INIITEM_KEY, val);
}

int GetIniItemValue(const IniItem& itm, int val)
{
    return itm.INIITEM_INI->ReadInteger( itm.INIITEM_SEKT, itm.INIITEM_KEY, val);
}

void operator<<(const IniItem& itm, const AnsiString& val)
{
    itm.INIITEM_INI->WriteString( itm.INIITEM_SEKT, itm.INIITEM_KEY, val);
}

AnsiString GetIniItemValue(const IniItem& itm, const AnsiString& val)
{
    return itm.INIITEM_INI->ReadString( itm.INIITEM_SEKT, itm.INIITEM_KEY, val);
}

void operator<<(const IniItem& itm, double val)
{
    itm.INIITEM_INI->WriteFloat( itm.INIITEM_SEKT, itm.INIITEM_KEY, val);
}

double GetIniItemValue(const IniItem& itm, double val)
{
    return itm.INIITEM_INI->ReadFloat( itm.INIITEM_SEKT, itm.INIITEM_KEY, val);
}



template< typename T >
struct FloatIniValueHelper
{
    explicit FloatIniValueHelper(const IniItem& iniItem, const T& val) : iniItem_(iniItem),
        val_( val )
    {
        T& v = const_cast<T&>(val_);
        v = GetIniItemValue(iniItem_, v);
    }
    ~FloatIniValueHelper()
    {
        iniItem_ << val_;
    }

    const T& val_;
    const IniItem iniItem_;
};

template< typename T >
struct IntIniValueHelper
{
    explicit IntIniValueHelper(const IniItem& iniItem, const T& val) : iniItem_(iniItem),
        val_( val )
    {
        T& v = const_cast<T&>(val_);
        v = iniItem_.INIITEM_INI->ReadInteger( iniItem_.INIITEM_SEKT, iniItem_.INIITEM_KEY, v);
    }
    ~IntIniValueHelper()
    {
        iniItem_.INIITEM_INI->WriteInteger( iniItem_.INIITEM_SEKT, iniItem_.INIITEM_KEY, val_);
    }

    const T& val_;
    const IniItem iniItem_;
};



#endif

