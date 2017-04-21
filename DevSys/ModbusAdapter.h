//---------------------------------------------------------------------------
#ifndef ModbusAdapterH
#define ModbusAdapterH
//---------------------------------------------------------------------------
//std

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

// my
#include "..\my_include\free_types_.h"
#include "..\my_include\MyPort.hpp"
#include "MasterSlaveIOSettings.hpp"

#include "MyExcpt.hpp"
#include "MasterSlaveIO.h"

class MasterSlaveIOImpl;
class TransferManagerT;

class MyModbusAdapterException : public MyException
{
public:
    MyModbusAdapterException(const FileLine& loc, const AnsiString& msg,
        const PMyExcpt nest = PMyExcpt() );
};

class ModbusAdapter : public RS485Adapter
{
public:
    explicit ModbusAdapter(const MasterSlaveIOSettings &sets, MyPort *port);
    virtual ~ModbusAdapter() {}

    virtual const unsigned char* SentData() const;
    virtual unsigned SentDataSize() const;
    virtual const unsigned char* AcceptedData() const;
    virtual unsigned AcceptedDataSize() const;

    virtual unsigned Addy() const;
    virtual unsigned CommandCode() const;

    virtual void PrepareTxD( std::vector<unsigned char>& txd, unsigned addy,
        unsigned cmdCode, const unsigned char* sendBegin, const unsigned char* sendEnd);
private:
    virtual bool TestAnswer(const unsigned char* reciveBegin, const unsigned char* reciveEnd);

};

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------

