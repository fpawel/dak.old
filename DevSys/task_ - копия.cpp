//---------------------------------------------------------------------------
#include <numeric>
#include <math>
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "task_.h"
//------------------------------------------------------------------------------
#include "unit1.h"
//------------------------------------------------------------------------------
#include "tools.h"
//------------------------------------------------------------------------------
//std
#include <iostream>
#include <dateutils.hpp>
#include "AnsiStringUtils_.h"
#include "IOProtocols.hpp"
#include "TransferManage.h"
#include "guicon.h"
#include "Unit1.h"
#include "MyExcpt.hpp"
#include "MySynchronize.h"
#include "myconv.h"
#include "ctrlsys.h"
#include "MasterSlaveIO.h"
#include "pneumo.hpp"
#include "MyWindow.h"

#include "uFrmWait.h"
#include "SGrdHlpr.h"
#include "TaskList.h"
#include "modbus_.h"
#include "vardef.hpp"
#include "bin_nums.h"
#include "tinyxml.h"
#include "xmlUtils_.h"

#include "HeatChamber.h"
#include "Devices.h"
#include "..\my_include\win\MyModFN.hpp"
//---------------------------------------------------------------------------
#include "boost\assign\list_of.hpp"

#include "MyStrUtils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
//---------------------------------------------------------------------------
using std::cout;
//---------------------------------------------------------------------------
#define RETURN_IF_STOPED_ if( MyTransferTask::MustCancelOperation() )return;
//---------------------------------------------------------------------------
#define SYNCHRONIZED_SLEEP_1_(mms)\
    MyTransferTask::Sleep(mms, true);\
    RETURN_IF_STOPED_;
#define SYNCHRONIZED_SLEEP_2_(mms, ss)\
    MyTransferTask::Sleep(mms, true, ss);\
    RETURN_IF_STOPED_;
//------------------------------------------------------------------------------
ModbusAdapter& Modbus(){ return *CtrlSys::Instance().GetIOSets().modbus; }


int MyFRound(double number)
{
    const double
        down = std::floor(number),
        up = std::ceil(number),
        dDown = std::fabs(number-down),
        dUp = std::fabs(number-up);
    return dDown<dUp ? down : up;
}
//------------------------------------------------------------------------------
void SetGas(unsigned gas_)
{
    //const unsigned deviceCount = Devs::GetSelectedAddys().size(),gasCount = Pneumo::GetGases().size();
    //const double value = ( gas_<gasCount ? (deviceCount*0.5*100.0)/30.0 : 0 );
    Pneumo::SwitchGas(gas_);
}
//------------------------------------------------------------------------------

LRESULT ShowErrorMessage(const AnsiString& msg, const AnsiString& ttl, UINT uType )
{
	return ::MessageBox(Application->Handle, msg.c_str(), ttl.c_str(), uType );
}

//------------------------------------------------------------------------------
CheckDeviceConnectTask::CheckDeviceConnectTask(unsigned msec) :
    ForEachAddys("Проверка наличия связи с приборами", msec)
{}
//------------------------------------------------------------------------------
void CheckDeviceConnectTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    DAK::GetModbusConc(addy);
    isComplete = 0;
}
void CheckDeviceConnectTask::DoSaveXML(TiXmlElement* elem) const
{
    tmHlpr_.SaveXML(elem);
}
//------------------------------------------------------------------------------
void CheckDeviceConnectTask::DoLoadXML(TiXmlElement* elem)
{
    tmHlpr_.LoadXML(elem);
}
//------------------------------------------------------------------------------
bool CheckDeviceConnectTask::DoSetupParams()
{
    return SetupParamsExt();
}
//------------------------------------------------------------------------------
AnsiString CheckDeviceConnectTask::What() const
{
    return MYSPRINTF_("Проверка наличия связи с приборами %s", tmHlpr_.ToStr() );
}
//------------------------------------------------------------------------------
MessageBoxTask::MessageBoxTask(const AnsiString& title, const AnsiString& msg,
    unsigned flags) :
    ScenaryAction(title+": " + msg), title_(title), msg_(msg), flags_(flags)
{}
//------------------------------------------------------------------------------
void MessageBoxTask::PerformAction()
{
    MyMessageBox(NULL, msg_.c_str(), title_.c_str(), MB_APPLMODAL |
        MB_SYSTEMMODAL | MB_TASKMODAL | flags_ );
}
//------------------------------------------------------------------------------
AnsiString MessageBoxTask::What() const
{
    return msg_;
}
//------------------------------------------------------------------------------
void MessageBoxTask::DoSaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set( elem, "заголовок", title_ );
    TiXMLAtr::Set( elem, "текст",     msg_ );
    TiXMLAtr::Set( elem, "флаги", flags_ );

}
//------------------------------------------------------------------------------
void MessageBoxTask::DoLoadXML(TiXmlElement* elem)
{
    title_ = TiXMLAtr::Get( elem, "заголовок", title_ );
    msg_ = TiXMLAtr::Get( elem, "текст",     msg_ );
    flags_ = TiXMLAtr::Get( elem, "флаги", flags_ );
}
//------------------------------------------------------------------------------
void MessageBoxTask::OnBeginTransfer(bool)
{
    
}
//------------------------------------------------------------------------------
BlowGas::BlowGas(int gas, unsigned blowTimeMSec) :
    ForEachAddys( Pneumo::BlowGasAction2Str(gas, blowTimeMSec), blowTimeMSec),
    gas_(gas)
{
}
//------------------------------------------------------------------------------
AnsiString BlowGas::What() const
{
    return Pneumo::BlowGasAction2Str(gas_, tmHlpr_.MSec() );
}
//------------------------------------------------------------------------------
void BlowGas::PerformBegin()
{
    SetGas(gas_);
}
void BlowGas::PerformEnd()
{
    int a = 0;

    
}
void BlowGas::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    DAK::GetModbusConc(addy, false);
    //DAK::GetDevState(addy, false);
    isComplete = false;
}
//------------------------------------------------------------------------------
void BlowGas::DoSaveXML(TiXmlElement* elem) const
{
    tmHlpr_.SaveXML(elem);
    TiXMLAtr::Set( elem, "номер_клапана", gas_ );
}
//------------------------------------------------------------------------------
void BlowGas::DoLoadXML(TiXmlElement* elem)
{
    tmHlpr_.LoadXML(elem);
    gas_ = TiXMLAtr::Get( elem, "номер_клапана", gas_);
}
//------------------------------------------------------------------------------
bool BlowGas::DoSetupParams()
{
    return ForEachAddys::SetupParamsExt();    
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
SwitchOffPneumoTask::SwitchOffPneumoTask() : ScenaryAction("Отключить газ")
{
}
void SwitchOffPneumoTask::PerformAction()
{
    SetGas(-1);
}
//------------------------------------------------------------------------------
DelayTask::DelayTask(unsigned msec) : ScenaryAction( "Задержка "+ TimeLimitedTaskHelper(msec).ToStr() ),
    tmHlpr_(msec)
{
}
//------------------------------------------------------------------------------
void DelayTask::PerformAction()
{
    SYNCHRONIZED_SLEEP_1_( tmHlpr_.MSec() );
}
//------------------------------------------------------------------------------
void DelayTask::DoSaveXML(TiXmlElement* elem) const
{
    tmHlpr_.SaveXML(elem);
}
//------------------------------------------------------------------------------
void DelayTask::DoLoadXML(TiXmlElement* elem)
{
    tmHlpr_.LoadXML(elem);
}
//------------------------------------------------------------------------------
AnsiString DelayTask::What() const
{
    return "Задержка "+ tmHlpr_.ToStr();
}



#define CHECKK_(rro) if( my::Grd::IsRowSelected( Form1->grdVars, rro+1)  )
ManualTestTask::ManualTestTask() : ForEachAddys("Опрос выходных параметров")
{
}
//------------------------------------------------------------------------------
void ManualTestTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    CHECKK_(0) DAK::GetModbusConc(addy, false);
    CHECKK_(1) DAK::GetDevState(addy, false);

    CHECKK_(5) DAK::ReadVar1(addy, false);
    CHECKK_(6) DAK::ReadT(addy, false);



    isComplete = false;
}
//------------------------------------------------------------------------------
#undef CHECKK_
//------------------------------------------------------------------------------
ManualGetTemperatureTask::ManualGetTemperatureTask() :
    ScenaryAction("Опрос значения температуры термокамеры")
{}
//------------------------------------------------------------------------------
void ManualGetTemperatureTask::PerformAction()
{
    try
    {
        HeatCham::GetTemperature();
    }
    catch(...) {}

}
//------------------------------------------------------------------------------
WriteModbusTask::WriteModbusTask(unsigned cmd, double val) :
    ForEachAddys( MYSPRINTF_("%s, %d, %2.2f", DAK::Cmd::Code2Str(cmd), cmd, val)), cmd_(cmd), val_(val)
{
}
//------------------------------------------------------------------------------
void WriteModbusTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    WriteModbusFloat(  *CtrlSys::Instance().GetIOSets().modbus, addy, cmd_, val_);
}
//------------------------------------------------------------------------------
AnsiString WriteModbusTask::What() const
{
    return MYSPRINTF_("%s, %d, %2.2f", DAK::Cmd::Code2Str(cmd_), cmd_, val_);
}
//------------------------------------------------------------------------------
void WriteModbusTask::DoSaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set( elem, "код", cmd_);
    TiXMLAtr::Set( elem, "аргумент", val_);
}
//------------------------------------------------------------------------------
void WriteModbusTask::DoLoadXML(TiXmlElement* elem)
{
    cmd_ = TiXMLAtr::Get( elem, "код", cmd_);
    val_ = TiXMLAtr::Get( elem, "аргумент", val_);
}

WriteModbusBroadcastTask::WriteModbusBroadcastTask(unsigned cmd, double val) :
    ScenaryAction( MYSPRINTF_("%s, %d, %2.2f", DAK::Cmd::Code2Str(cmd), cmd, val)), cmd_(cmd), val_(val)
{
}
//------------------------------------------------------------------------------
void WriteModbusBroadcastTask::PerformAction()
{
    WriteModbusFloat(  *CtrlSys::Instance().GetIOSets().modbus, 0, cmd_, val_);
    MyTransferTask::Sleep(1000);
}
//------------------------------------------------------------------------------
AnsiString WriteModbusBroadcastTask::What() const
{
    return MYSPRINTF_("%s, %d, %2.2f", DAK::Cmd::Code2Str(cmd_), cmd_, val_);
}
//------------------------------------------------------------------------------
void WriteModbusBroadcastTask::DoSaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set( elem, "код", cmd_);
    TiXMLAtr::Set( elem, "аргумент", val_);
}
//------------------------------------------------------------------------------
void WriteModbusBroadcastTask::DoLoadXML(TiXmlElement* elem)
{
    cmd_ = TiXMLAtr::Get( elem, "код", cmd_);
    val_ = TiXMLAtr::Get( elem, "аргумент", val_);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
SetAddyTask::SetAddyTask(unsigned addy) :
    ScenaryAction(MYSPRINTF_("Установка сетвого адреса %d", addy)), addy_(addy)
{}
//------------------------------------------------------------------------------
void SetAddyTask::PerformAction()
{
    WriteModbusFloat(  Modbus(), 0, DAK::Cmd::Code::addy, addy_);
    MyTransferTask::Sleep(2000);
    DAK::GetModbusConc(addy_);
}
//------------------------------------------------------------------------------
AnsiString SendHeatChamControlCommandTask::FormatCmd(SendHeatChamControlCommandTask::Cmd cmd)
{
    if(cmd==SendHeatChamControlCommandTask::setpoint)
        return "Установка уставки термокамеры";
    if(cmd==SendHeatChamControlCommandTask::start)
        return "Старт внешнего управления термокамерой";
    if(cmd==SendHeatChamControlCommandTask::stop)
        return "Остановка внешнего управления термокамерой";
    if(cmd==SendHeatChamControlCommandTask::fix)
        return "Перевод камеры в режим фиксированной работы";
    else return "...";
}

SendHeatChamControlCommandTask::SendHeatChamControlCommandTask(Cmd cmd, int t) :
    ScenaryAction(MYSPRINTF_("%s %g", FormatCmd(cmd), t/10.0) ), cmd_(cmd), t_(t)
{}
void SendHeatChamControlCommandTask::PerformAction()
{
    if(cmd_==start)
        HeatCham::Start();
    else if(cmd_==stop)
        HeatCham::Stop();
    else if(cmd_==fix)
        HeatCham::Fix();
    else if(cmd_==setpoint)
    {
        //HeatCham::Stop();
        HeatCham::SetSetpoint(t_);
        //HeatCham::Start();
    }
}
//------------------------------------------------------------------------------
SendHeatChamSetpointTask::SendHeatChamSetpointTask(int t) :
    ForEachAddys(MYSPRINTF_("Уставка термокамеры %g\"С", t/10.0), 2*60*60*1000 ), t_(t)
{}
AnsiString SendHeatChamSetpointTask::What() const
{
    return MYSPRINTF_("Уставка термокамеры %g\"С", t_/10.0);
}
void SendHeatChamSetpointTask::PerformBegin()
{
    HeatCham::Stop();
    HeatCham::SetSetpoint(t_);
    HeatCham::Start();
}
//------------------------------------------------------------------------------
bool SendHeatChamSetpointTask::IsComplete() const
{
    return std::fabs( HeatCham::GetTemperature() - t_/10.0)<=2.0;
}
//------------------------------------------------------------------------------
void SendHeatChamSetpointTask::PerformEnd()
{
    if( !IsComplete() ) Form1->LogError(
        MYSPRINTF_("Не достигнута требуемая температура термокамеры %g\"С!",
            t_/10.0 ) );
}
//------------------------------------------------------------------------------
void SendHeatChamSetpointTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    DAK::GetModbusConc(addy, false);
    isComplete = IsComplete();
}
//------------------------------------------------------------------------------
void SendHeatChamSetpointTask::DoSaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set( elem, "температура_градусов_цельсия", t_ );
}
//------------------------------------------------------------------------------
void SendHeatChamSetpointTask::DoLoadXML(TiXmlElement* elem)
{
    t_ = TiXMLAtr::Get( elem, "температура_градусов_цельсия", t_);
}
//------------------------------------------------------------------------------;
bool SendHeatChamSetpointTask::DoSetupParams()
{
    SetupParamsExt();
    AnsiString sS = t_/10.0;
	if( !InputQuery( What(), What()+". Введите значение температуры в градусах Цельсия.", sS) )
        return 0;
    t_ = MyFRound( 10*MyStrToFloatDef(sS, 20) );
    return 1;
}
//------------------------------------------------------------------------------;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
ProccessKoefs::ProccessKoefs(const AnsiString& what, const std::vector<unsigned>& kefsNums)
    : ForEachAddys(what), kefsNums_(kefsNums)
{}
//------------------------------------------------------------------------------
AnsiString ProccessKoefs::What() const
{
    return ForEachAddys::What()+", "+format_sorted_intervals(kefsNums_.begin(),
        kefsNums_.end() ).c_str();
}
void ProccessKoefs::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{

    TransferManagerT& tmngr = TransferManager::Instance();

    tmngr.ShowTopPanel( What(), true );
    tmngr.SetTopPanelProgress( 0, kefsNums_.size(), 0 );
    tmngr.SetEnableTopPanelTimeProgress(1);


    const DAK::Kefs kefs = DAK::GetKefs();
    for( unsigned idxKefNum=0; idxKefNum<kefsNums_.size() &&
        !TransferManager::Instance().IsThreadStopedOrTerminated(); ++idxKefNum )
    {   
        const unsigned kNum = kefsNums_[idxKefNum];
        if( !(kNum< kefs.size()) ) {
            continue;
        }
        const unsigned n = kefs[kNum].n;
        AnsiString s;
        s.sprintf("кэф %d %s", n, kefs[kNum].caption);
        tmngr.SetTopPanelCaption( MYSPRINTF_("%s, %s", What(), s) );
        ProccessItem( addy, kNum );
        tmngr.SetTopPanelProgress( -2 ); }
    tmngr.HideTopPanel();
}
//------------------------------------------------------------------------------
void ProccessKoefs::DoSaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set(elem, "номера_кефов", format_sorted_intervals( kefsNums_.begin(),
        kefsNums_.end() ).c_str() );
}
//------------------------------------------------------------------------------
void ProccessKoefs::DoLoadXML(TiXmlElement* elem)
{
    std::string s = TiXMLAtr::Get(elem, "номера_кефов", s);
    ParseArrayOfIntervals(s, kefsNums_);
    std::sort(kefsNums_.begin(), kefsNums_.end());
}


WriteKefs1Task::WriteKefs1Task(const std::vector<unsigned>& kefs)
    : ForEachAddys("Запись коэффициентов"), kefs_(kefs)
{
    
}
//------------------------------------------------------------------------------
AnsiString WriteKefs1Task::What() const
{
    return ForEachAddys::What()+", "+format_sorted_intervals(kefs_.begin(),
        kefs_.end() ).c_str();
}
void WriteKefs1Task::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{

    for (int n=0; n<kefs_.size(); ++n){

        MyWCout(AnsiString().sprintf(" -- %d\n", kefs_[n]) );
    }

    TransferManagerT& tmngr = TransferManager::Instance();

    tmngr.ShowTopPanel( What(), true );
    tmngr.SetTopPanelProgress( 0, kefs_.size(), 0 );
    tmngr.SetEnableTopPanelTimeProgress(1);

    const DAK::Kefs kefs = DAK::GetKefs();

    for( unsigned n=0; n<kefs_.size() &&
        !TransferManager::Instance().IsThreadStopedOrTerminated(); ++n )
    {
        bool ok = false;
        for(int i=0; i<kefs.size(); ++i) {
            if(kefs[i].n==kefs_[n]) {
                AnsiString s;
                tmngr.SetTopPanelCaption( MYSPRINTF_("%s, %s", What(), s) );
                DAK::WriteKef(addy, i);
                tmngr.SetTopPanelProgress( -2 );
                ok = true;
            }
        }
        if(!ok) {
            AnsiString s;
            s.sprintf("кэф %d - не найден\n", kefs_[n]);
            MyWCout(s);

        }
    }
    tmngr.HideTopPanel();
}
//------------------------------------------------------------------------------
void WriteKefs1Task::DoSaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set(elem, "номера_кефов", format_sorted_intervals( kefs_.begin(),
        kefs_.end() ).c_str() );
}
//------------------------------------------------------------------------------
void WriteKefs1Task::DoLoadXML(TiXmlElement* elem)
{
    std::string s = TiXMLAtr::Get(elem, "номера_кефов", s);
    ParseArrayOfIntervals(s, kefs_);
    std::sort(kefs_.begin(), kefs_.end());
}

//------------------------------------------------------------------------------
ReadKoefsTask::ReadKoefsTask(const std::vector<unsigned>& kefsNums):
    ProccessKoefs( "Чтение кэфов", kefsNums )
{}
//------------------------------------------------------------------------------
void ReadKoefsTask::ProccessItem(unsigned addy, unsigned kNum)
{
    DAK::ReadKef(addy, kNum);
}
//------------------------------------------------------------------------------
WriteKoefsTask::WriteKoefsTask(const std::vector<unsigned>& kefsNums) :
    ProccessKoefs( "Запись кэфов", kefsNums )
{}
//------------------------------------------------------------------------------
void WriteKoefsTask::ProccessItem(unsigned addy, unsigned kNum)
{
    DAK::WriteKef(addy, kNum);
}
//------------------------------------------------------------------------------
AdjustCurrentTask::AdjustCurrentTask(unsigned msec) :
    ForEachAddys("Калибровка тока"), tm_(msec)
{
}
//------------------------------------------------------------------------------
#define SLEEP_ MyTransferTask::Sleep(tm_.MSec() ); if( MyTransferTask::MustCancelOperation() ) return;
void AdjustCurrentTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    TransferManagerT& tmngr = TransferManager::Instance();

    enum
    {
        cmd_set_k_336 = ( 0x80 << 8 ) + 336,
        cmd_set_k_335 = ( 0x80 << 8 ) + 335
    };
    tmngr.ShowTopPanel(IntToStr(addy)+": " +What(), true );

    WriteModbusFloat( Modbus(), addy, cmd_set_k_335, 0);
    WriteModbusFloat( Modbus(), addy, cmd_set_k_336, 1);
    SLEEP_;

    WriteModbusFloat( Modbus(), addy, DAK::Cmd::Code::set_tok, 4);
    SLEEP_;
    const double i1 = DAK::GetDevState(addy).current;
    Form1->AddAddyLog( addy, MYSPRINTF_("калибровка тока I1=%g", i1) );

    WriteModbusFloat( Modbus(), addy, DAK::Cmd::Code::set_tok, 20);
    SLEEP_;
    const double i2 = DAK::GetDevState(addy).current;
    Form1->AddAddyLog( addy, MYSPRINTF_("калибровка тока I2=%g", i2) );

    const double k336 = 16/(i2-i1);
    Form1->AddAddyLog( addy, MYSPRINTF_("калибровка тока K336=%g", k336) );
    WriteModbusFloat( Modbus(), addy, cmd_set_k_336, k336);
    
    WriteModbusFloat( Modbus(), addy, DAK::Cmd::Code::set_tok, 4);
    SLEEP_;
    const double i3 = DAK::GetDevState(addy).current;
    Form1->AddAddyLog( addy, MYSPRINTF_("калибровка тока I3=%g", i3) );

    const double k335 = 4-i3;
    Form1->AddAddyLog( addy, MYSPRINTF_("калибровка тока K335=%g", k335) );
    WriteModbusFloat( Modbus(), addy, cmd_set_k_335, k335);
    SLEEP_;
    WriteModbusFloat( Modbus(), addy, DAK::Cmd::Code::test_watch_dog1, 0);

    tmngr.HideTopPanel();
}
#undef SLEEP_
//------------------------------------------------------------------------------
void AdjustCurrentTask::PerformBegin()
{
    Pneumo::SwitchGas(-1);
}
void AdjustCurrentTask::PerformEnd()
{
    WriteModbusFloat( Modbus(), 0, DAK::Cmd::Code::test_watch_dog, 0);
}
//------------------------------------------------------------------------------
SetPorogsTask::SetPorogsTask(unsigned kind): ForEachAddys( Format(kind) ),
    kind_(kind)
{
}
//------------------------------------------------------------------------------
void SetPorogsTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    const DAK::Sets sets = DAK::Sets::Get();

    double porog1, porog2;
    if( kind_==k90percent) {
        porog1 = sets.pgs[1]*0.9;
        porog2 = sets.pgs[2]*0.9;
    } else if( kind_==kNominal) {
        porog1 = sets.porog1;
        porog2 = sets.porog2;
    } else if( kind_==k5percent) {
        porog1 = sets.endScale*0.05;
        porog2 = sets.endScale*0.05;
    }

    Form1->AddAddyLog(addy, MYSPRINTF_("Установка порогов %g,%g", porog1, porog2) );

    WriteModbusFloat( Modbus(), addy, DAK::Cmd::Code::por_1, porog1);
    WriteModbusFloat( Modbus(), addy, DAK::Cmd::Code::por_2, porog2);

    const unsigned porog1KNum = MAIN_KEF_N_(Porog1), porog2KNum = MAIN_KEF_N_(Porog2);
    DAK::ReadKef(addy, porog1KNum);
    DAK::ReadKef(addy, porog2KNum);


}
//------------------------------------------------------------------------------
void SetPorogsTask::DoSaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set( elem, "тип", kind_ );

}
//------------------------------------------------------------------------------
void SetPorogsTask::DoLoadXML(TiXmlElement* elem)
{
    kind_ = TiXMLAtr::Get( elem, "тип", kind_ );
}
//------------------------------------------------------------------------------
AdjustConcTask::AdjustConcTask(bool isEndScale): ForEachAddys( Format(isEndScale) ),
    isEndScale_(isEndScale)
{
}
//------------------------------------------------------------------------------
void AdjustConcTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    const DAK::Sets sets = DAK::Sets::Get();
    using namespace DAK::Cmd::Code;
    unsigned cmd, gasIdx;
    if(isEndScale_) {
        cmd = adj_s;
        gasIdx = 2;
    } else {
        cmd = adj_0;
        gasIdx = 0;
    }
    const double val = sets.pgs[gasIdx];
    WriteModbusFloat( Modbus(), addy, cmd, val );
}
//------------------------------------------------------------------------------
void AdjustConcTask::DoSaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set( elem, "конец", isEndScale_ );

}
//------------------------------------------------------------------------------
void AdjustConcTask::DoLoadXML(TiXmlElement* elem)
{
    isEndScale_ = TiXMLAtr::Get( elem, "конец", isEndScale_ );
}
void SelectGasTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    const DAK::Sets sets = DAK::Sets::Get();
    const double arg = sets.gas=="CH4" ? 1 : ( sets.gas=="CO2" ? 0 : 15);
    WriteModbusFloat( Modbus(), addy, DAK::Cmd::Code::select_gas, arg);
    WriteModbusFloat( Modbus(), addy, DAK::Cmd::Code::reset, 0);
}


void runTestHart()
{
    Form1->Hide();

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    AnsiString path =
        AnsiString(MyGetExePath().c_str() )+ "\\MIL82\\MIL82.exe";

    // Start the child process.
    if( !CreateProcess( NULL,   // No module name (use command line)
        path.c_str(),   // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    )
    {
        AnsiString s;
        s.sprintf("CreateProcess failed (%d).\n", GetLastError());

        Form1->AddLog(s);
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    Form1->Show();
}


