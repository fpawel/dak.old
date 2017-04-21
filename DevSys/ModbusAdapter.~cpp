//---------------------------------------------------------------------------
#include <windows.h>

//std
#include <sstream>

#pragma hdrstop
//------------------------------------------------------------------------------
#include "ModbusAdapter.h"
//------------------------------------------------------------------------------
// loki
//#include "loki\threads.h"
//------------------------------------------------------------------------------
#include "CRC16.h"
#include "bcd_.h"
#include "AnsiStringUtils_.h"
#include "guicon.h"
#include "iniUtils_.h"
#include "CommSetsIOHelper.h"
#include "TransferManage.h"
#include "FooCalledOnce.hpp"
#include "MasterSlaveIO.h"
#include "MySynchronize.h"

//------------------------------------------------------------------------------
using std::cout;
using std::endl;

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// вывести массив в cout
AnsiString PrintVInt8( const VInt8& v, int lim = -1 )
{
    const unsigned maxIdx = (lim==-1) ? v.size() : std::min(lim, v.size() );
    return MyBuffToStr( v.begin(), v.begin() + maxIdx );
}
//------------------------------------------------------------------------------
bool IsModbusStr(const uInt8 *b, const uInt8 *e, uInt8 devAddr, uInt8 commandCode)
{
	return !(e-b<4) && b[0]==devAddr &&
    ( b[1]==commandCode || b[1]==( commandCode | 0x80 ) ) &&
    GetCRC16( b, e )==0;
}
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
ModbusAdapter::ModbusAdapter(const MasterSlaveIOSettings &sets, MyPort *port ) :
    RS485Adapter( sets, port )
{
}
//------------------------------------------------------------------------------
#define THROW_TRANSFER_EXCEPTION_(MSG)\
	throw PMyExcpt( new MyModbusAdapterException( __FILE_LINE__, MYSPRINTF_("%d: %s", addy, MSG) ) )
//------------------------------------------------------------------------------
bool ModbusAdapter::TestAnswer(const unsigned char* reciveBegin, const unsigned char* reciveEnd)
{
    const unsigned addy = Addy(), commandCode=CommandCode();
	if( addy==0) return false;
	const int rxdLen = reciveEnd - reciveBegin;
    assert(rxdLen>=0);
    if( rxdLen==0 ) THROW_TRANSFER_EXCEPTION_( "не отвечает" );
    if( rxdLen<4 ) {
    	MyCout( AnsiString().sprintf(" длина ответа %s равна %d, должна быть не менее 4",
        		MyBuffToStr1( reciveBegin, reciveEnd ), rxdLen) );
        return true;
    }

    if( (unsigned)reciveBegin[0]!=addy )  {
    	MyCout( AnsiString().sprintf("несовпадение адрессов запроса %d и ответа %d",
        		addy,  reciveBegin[0] ) ) ;
        return true;
    }
    const bool
        cmdCodeMatch = ( (unsigned)reciveBegin[1]==commandCode ),
        modbusError = ( (unsigned)reciveBegin[1]==( commandCode | 0x80 ) );

    if( !cmdCodeMatch && !modbusError ) {
        MyCout( AnsiString().sprintf("несовпадение кодов комманд запроса %d и ответа %d", commandCode, reciveBegin[1] ));
        return true;
    }

    if( GetCRC16( reciveBegin, reciveEnd )!=0 ) {
    	MyCout( AnsiString().sprintf( "ненулевая контрольная сумма ответа"));
        return true;
    }

    const unsigned txdSz = MasterSlave().TxDSize();
    const unsigned char *sendBegin = MasterSlave().TxD();
    const unsigned char ch1 = (commandCode | 0x80);
    if( rxdLen==5 && ( reciveBegin[1] == ch1 ) && txdSz>=4 )
    {
    	if( commandCode==3 && reciveBegin[2]==2 )
        {
        	const unsigned regAdr = (sendBegin[0]<<8) + sendBegin[1];
    		MyCout( AnsiString().sprintf(
            	"команда 3, адрес регистра %d не допустим для данного прибора", regAdr ) );
            return true;
        }

    	if( reciveBegin[2]!=0 ) {
    		MyCout( AnsiString().sprintf(
            	"исключительная ситуация %d.", reciveBegin[2] ) );
            return true;
        }
    }

    assert( IsModbusStr(reciveBegin, reciveEnd, addy, commandCode ) );
    return false;
}
//------------------------------------------------------------------------------
void ModbusAdapter::PrepareTxD( std::vector<unsigned char>& txd, unsigned addy,
        unsigned commandCode, const unsigned char* sendBegin, const unsigned char* sendEnd)
{
	const int sendStrLen = sendEnd - sendBegin;
    assert(sendStrLen<10000);
    assert( commandCode!=3 || sendStrLen==4 );
	// подготовить посылку
    txd.resize(sendEnd - sendBegin + 4);
    // формировать строку с адрессом и контрольной суммой CRC16
    txd[0] = addy;
    txd[1] = commandCode;
    std::copy( sendBegin, sendEnd, txd.begin()+2 );
    const short unsigned int crc16 = GetCRC16 ( txd.begin(), txd.end()-2 );
    *(txd.end()-2) = crc16 >> 8;
    *(txd.end()-1) = crc16;
    assert( IsModbusStr(txd.begin(), txd.end(), addy, commandCode) );

}
//------------------------------------------------------------------------------
unsigned ModbusAdapter::SentDataSize() const
{
    const unsigned txdSize = MasterSlave().TxDSize();
    return txdSize>5 ? txdSize-4 : 0;
}
//------------------------------------------------------------------------------
const unsigned char* ModbusAdapter::SentData() const
{
    return MasterSlave().TxDSize()>5 ? MasterSlave().TxD()+2 : 0;
}
//------------------------------------------------------------------------------
unsigned ModbusAdapter::AcceptedDataSize() const
{
    const unsigned rxdSize = MasterSlave().RxDSize();
    return rxdSize>5 ? rxdSize-4 : 0;
}
//------------------------------------------------------------------------------
const unsigned char* ModbusAdapter::AcceptedData() const
{
    return MasterSlave().RxDSize()>5 ? MasterSlave().RxD()+2 : 0;
}
//------------------------------------------------------------------------------
unsigned ModbusAdapter::Addy() const
{
    return MasterSlave().TxDSize()>0 ? MasterSlave().TxD()[0] : -1;
}
//------------------------------------------------------------------------------
unsigned ModbusAdapter::CommandCode() const
{
    return MasterSlave().TxDSize()>1 ? MasterSlave().TxD()[1] : -1;
}
//------------------------------------------------------------------------------
MyModbusAdapterException::MyModbusAdapterException(const FileLine& loc, const AnsiString& msg,
    const PMyExcpt nest ) : MyException( loc, msg, nest )
{}






