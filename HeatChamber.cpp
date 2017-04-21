//------------------------------------------------------------------------------
#include <vector>
#include <algorithm>
#include <numeric>
#pragma hdrstop
#include "HeatChamber.h"
#include "AnsiStringUtils_.h"
#include "guicon.h"
#include "my_array_size.h"
#include "ctrlsys.h"
#include "MasterSlaveIO.h"
#include "MyExcpt.hpp"
#include "Unit1.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
typedef std::vector<unsigned char> VInt8;
//------------------------------------------------------------------------------
namespace HeatCham
{
//------------------------------------------------------------------------------
unsigned HexToAsci( const char* beg, const char* end )
{
	AnsiString s = "0x";
	for( ; beg!=end; ++beg )
	{
		const char ch = *beg;
		s = s + AnsiString(ch);
	}
	return StrToIntDef(s,0);
}
//------------------------------------------------------------------------------
VInt8 StrToAsciInt(const AnsiString &str)
{
    VInt8 ret;
	for( unsigned i=1; (int)i<str.Length() + 1; ++i ){
		ret.push_back( str[i] );
	}
	return ret;
}
//------------------------------------------------------------------------------
VInt8 MakeRequest(const VInt8& dat)
{
	VInt8 res(1,2);
	res.insert( res.end(), dat.begin(), dat.end() );
	const unsigned char sum  = std::accumulate(res.begin()+1,res.end(),0);

	AnsiString sumStr = IntToHex( sum, 2 );
	const char s1 = sumStr[1], s2 = sumStr[2];
	res.push_back(0x0D);
	res.push_back(0x0A);
	return res;
}
//------------------------------------------------------------------------------
VInt8 MakeRequest(const AnsiString& cmd)
{
	return MakeRequest( StrToAsciInt(cmd) );
}
//------------------------------------------------------------------------------
bool SuccessedWRDCommand(const char* rxd, unsigned len)
{
    const unsigned char goodAnswerOnWRDCommand[] =
	    {0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x4F, 0x4B, 0x0D, 0x0A};

    return len==array_size(goodAnswerOnWRDCommand) &&
	    std::equal( rxd, rxd+len, goodAnswerOnWRDCommand );
}
//------------------------------------------------------------------------------
bool SuccsecedGetTCommand( const char* rxd, unsigned len )
{
    static const byte goodOtvet[] =
		{0x02, 0x30, 0x31, 0x52, 0x52, 0x44, 0x2C };

    return std::equal( rxd, rxd+array_size(goodOtvet), goodOtvet );
}
//------------------------------------------------------------------------------
namespace Cmd
{
    const AnsiString
		getT 		= "01RRD,02,0001,0002", 	// 	"Запрос текущей темературы камеры"
		setFxMd 	= "01WRD,01,0104,0001",		// "Перевод камеры в режим фиксированной работы
		setStart	= "01WRD,01,0101,0001",		// "Старт"
		setStop		= "01WRD,01,0101,0004",		// "стоп"
		// Задание уставки val в режиме фиксированной работы
		setT = "01WRD,01,0102,";
};
//------------------------------------------------------------------------------
MasterSlaveIOImpl& IO()
{
    return *CtrlSys().Instance().GetIOSets().heatCham;
}
//------------------------------------------------------------------------------
void SendControlRequest(const AnsiString &cmd, const AnsiString &msg)
{
    const VInt8 req = MakeRequest(cmd);
    MYCOUT_("Управление термокамерой: %s, %s\n", msg, cmd ));
    IO().Send( req.begin(), req.end(), true );
    if(  !SuccessedWRDCommand( IO().RxD(), IO().RxDSize() ) )
        MY_THROW_("Ошибка термокамеры!");
}
//------------------------------------------------------------------------------
void SetSetpoint(int t)
{
    //const AnsiString sT = t>=0 ? IntToHex( t,2) : "-"+IntToHex( std::abs(t),2);
    //const AnsiString sT = IntToHex( t,4);

    //const AnsiString sT = t>=0 ? IntToHex( t,4) : "-"+IntToHex( std::abs(t),4);
    /*
    if (t>=0) {
        SendControlRequest( Cmd::setT + IntToHex( t,4), MYSPRINTF_("уставка %g", t/10.0) );
    } else {
        SendControlRequest( Cmd::setT + "-"+IntToHex( std::abs(t),4), MYSPRINTF_("уставка %g", t/10.0) );
    }
    */
    std::string s = AnsiString::IntToHex( t,4).c_str();
    while(s.size()<4)
        s = "00" + s;
    while (s.size()>4)   {
        std::rotate(s.begin(), s.begin() + 1, s.end());
        s = s.substr(0, s.size()-1);
    }

    SendControlRequest( Cmd::setT + s.c_str(), MYSPRINTF_("уставка %g", t/10.0) );


    Form1->btnHeatchamSetpoint->Caption = t/10.0;
    Form1->AddLogAll( MYSPRINTF_("Уставка темокамеры %g", t/10.0));
}
void Start()
{
    SendControlRequest( Cmd::setStart, "старт внешнего управления" );
}
void Stop()
{
    SendControlRequest( Cmd::setStop, "остановка внешнего управления" );
}
void Fix()
{
    SendControlRequest( Cmd::setFxMd, "перевод камеры в режим фиксированной работы" );
}
//------------------------------------------------------------------------------

double GetTemperature1()
{
    const VInt8 req = MakeRequest( Cmd::getT );
    MYCOUT_("Запрос температуры темокамеры %s\n", Cmd::getT ));
    IO().Send( req.begin(), req.end(), true );
    const char *rxd = IO().RxD();
    MYCOUT_("Термокамера говорит: \"%s\"\n", AnsiString(rxd, IO().RxDSize() ) ));
    if(  !SuccsecedGetTCommand( rxd, IO().RxDSize() ) )
        MY_THROW_("Ошибка термокамеры!");

    const char* pStrTemperature = rxd+10;
    const AnsiString sTemperature(pStrTemperature,4);
    
    return myHexToInt(sTemperature)/10.0;
}
double GetTemperature()
{
    Form1->btnHeatchamT->Font->Color = clGreen;
    Form1->btnHeatchamT->Caption = "...";

    try
    {
        const double ret = GetTemperature1();
        Form1->btnHeatchamT->Caption = ret;
        return ret;
    }
    catch(...)
    {
        Form1->btnHeatchamT->Font->Color = clRed;
        Form1->btnHeatchamT->Caption ="ошибка";
        throw;
    }
}

//------------------------------------------------------------------------------
};  // namespace HeatCham
//------------------------------------------------------------------------------
