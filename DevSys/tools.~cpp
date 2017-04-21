//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "tools.h"
//------------------------------------------------------------------------------
#include <algorithm>
#include <math>
//------------------------------------------------------------------------------
#include "boost\assign\list_of.hpp"
//------------------------------------------------------------------------------
#include "ctrlsys.h"
#include "myconv.h"
#include "AnsiStringUtils_.h"
#include "guicon.h"
#include "Unit1.h"
#include "MyVclUtils_.h"
#include "bin_nums.h"
#include "TransferManage.h"
#include "..\my_include\MyNoDestroySingleton.hpp"
#include "MasterSlaveIO.h"
#include "Myformat.hpp"
#include "modbus_.h"
#include "bin_nums.h"
#include "ModbusAdapter.h"
#include "vardef.hpp"
#include "pneumo.hpp"
#include "FooCalledOnce.hpp"
#include "Devices.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
namespace DAK
{
ModbusAdapter& Modbus()
{
    return *CtrlSys().Instance().GetIOSets().modbus;
}
//------------------------------------------------------------------------------
void LogValue(unsigned addy, const AnsiString &s, const AnsiString &what, unsigned nVar, bool enableLog)
{
    Vars::Set(addy, nVar, s);
    if(enableLog) {
        const ConsoleScreenAtrributesHolder cah(MY_SET_CONSOLE_VELVET_TEXT);
        Form1->AddAddyLog( addy, s);
    }
}
//------------------------------------------------------------------------------
ConcT  GetModbusConc(unsigned addy, bool enableLog)
{
    const ConsoleScreenAtrributesHolder cah(MY_SET_CONSOLE_YELLOW_TEXT);
    ConcT ret;

    const ModbusValue1T v = ReadModbusValue1(  Modbus(), addy, 0 );
    ret.conc = v.conc;
    //ret.conc = 1.1+random(10)/10.0;

    ret.sConc = Format::Conc(ret.conc);
    LogValue(addy, ret.sConc, "конц.", Vars::C, enableLog);
    return ret;
}
//------------------------------------------------------------------------------
DevState GetDevState(unsigned addy, bool enableLog)
{
    const ConsoleScreenAtrributesHolder cah(MY_SET_CONSOLE_YELLOW_TEXT);

    CtrlSysImpl& ctrlSys = CtrlSys::Instance();
    DevState ret;

    ReadModbusRegistersData(  Modbus(), ctrlSys.GetIOSets().stendAddy, (addy-1)*2, 2 );
    const unsigned char *dt = Modbus().AcceptedData();

    const int
        acceptedDataSize = Modbus().AcceptedDataSize();


    if( Modbus().AcceptedDataSize()!=5 )
        throw PMyExcpt( new MyTransferException(__FILE_LINE__,
           "Ошибка обмена со стендом", &Modbus().MasterSlave() ) );
    ret.current = 1.0*( dt[1]*256.0+dt[2])/100.0;
    const bool p1 = ( Getbit(dt[4], 1)==0 ), p2 = ( Getbit(dt[4], 2)==0 );

    //const bool p1 = 1, p2 = 1;
    //ret.current = 4+random(16)/16.0;

    ret.sIout = Format::Iout(ret.current);
    ret.rele[RelePin::rele1] = p1;
    ret.rele[RelePin::rele2] = p2;

    LogValue(addy, ret.sIout,   "Iвых", Vars::Iout, enableLog);

    const double conc_i = I2Conc(ret.current);
    ret.sConcI = Format::Conc(conc_i);
    LogValue(addy, ret.sConcI,  "конц.ток", Vars::C_Iout, enableLog);

    ret.sPorog1 = Format::Porog1(p1),
    ret.sPorog2 = Format::Porog2(p2);

    const DAK::Sets sets = DAK::Sets::Get();
    const unsigned isp = StrToIntDef(sets.ispolnenie,0);
    const unsigned skipIsp[6] = { 23, 24, 25, 31, 32, 33};
    if(std::find(skipIsp, skipIsp+5, isp)==skipIsp+5) {
        LogValue(addy, ret.sPorog1, "Порог", Vars::rele1, enableLog);
        LogValue(addy, ret.sPorog2, "Порог", Vars::rele2, enableLog); }
    return ret;
}

void WriteKef(unsigned addy, unsigned kNum)
{
    const DAK::Kefs kefs = DAK::GetKefs();
    const DAK::Kef kef = kefs[kNum];
    const unsigned cmd = ( 0x80 << 8 ) + kef.n;
    const AnsiString sVal = Devs::GetKefOfAddy(addy, kNum);

    AnsiString s;
    s.sprintf("записать кэф.%d %s=%s", kef.n, kefs[kNum].caption, sVal);
    double val;
    if( TryStr2Float( sVal.c_str(), &val ) )
        WriteModbusFloat( Modbus(), addy, cmd, val );
    else
        s += ", некорректное значение";
    Form1->AddAddyLog(addy, s );
}

double ReadKef(unsigned addy, unsigned kNum)
{
    const DAK::Kefs kefs = DAK::GetKefs();
    const DAK::Kef kef = kefs[kNum];

    const double val = ReadModbusFloat( Modbus(), addy, 224 + kef.n*2 );
    //double val = 123;

    Devs::SetKefOfAddy(addy, kNum, val);
    AnsiString s;
    s.sprintf("считан кэф.%d %s=%g", kef.n, kefs[kNum].caption, val);
    Form1->AddAddyLog(addy, s );
    return val;
}

double ReadVar1(unsigned addy, bool enableLog)
{
    const double val = ReadModbusFloat( Modbus(), addy, 16 );
    //double val = 4.567;

    LogValue(addy, val, "var1", Vars::var1, enableLog);
    return val;
}

// ток излучателя
double ReadIlampWorkkRefk (unsigned addy, bool enableLog)
{
        AnsiString s;
    double val = ReadModbusFloat( Modbus(), addy, 4 );
    if(enableLog) {
        s.sprintf("Ilamp=%g", val);
        Form1->AddAddyLog( addy, s);
    }

    val = ReadModbusFloat( Modbus(), addy, 14 );
    if(enableLog) {
        s.sprintf("Workk=%g", val);
        Form1->AddAddyLog( addy, s);
    }

    val = ReadModbusFloat( Modbus(), addy, 12 );
    if(enableLog) {
        s.sprintf("Refk=%g", val);
        Form1->AddAddyLog( addy, s);
    }
    return val;
}

double ReadIlampOn (unsigned addy, bool enableLog){
    AnsiString s;
    double val = ReadModbusFloat( Modbus(), addy, 4 );
    if(enableLog) {
        s.sprintf("Ilamp=%g", val);
        Form1->AddAddyLog( addy, s);
    }
    
    return val;

}

// датчик температуры
double ReadT(unsigned addy, bool enableLog)
{
    const double val = ReadModbusFloat( Modbus(), addy, 2 );
    LogValue(addy, val, "T\"C", Vars::Tpp, enableLog);
    return val;
}


//------------------------------------------------------------------------------
};// namespace Stend_STM30
//------------------------------------------------------------------------------
