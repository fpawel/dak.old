//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "CheckConcTask.h"
//------------------------------------------------------------------------------

#include <math>

//------------------------------------------------------------------------------
#include "tools.h"
#include "pneumo.hpp"
#include "AnsiStringUtils_.h"
#include "ctrlsys.h"
#include "xmlUtils_.h"
#include "Devices.h"
#include "uFrameTestConcData.h"
#include "MyStrUtils.h"
#include "Unit1.h"
#include "TransferManage.h"
#include "MyModFN.hpp"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
GetModbusConcTask::GetModbusConcTask() : ForEachAddys( "Опросить концентрацию" ) {}
//------------------------------------------------------------------------------
void GetModbusConcTask::PerformActionForAddy(unsigned addy, bool&, bool&) {
    DAK::GetModbusConc(addy, true);
}
//------------------------------------------------------------------------------
GetStendTask::GetStendTask() : ForEachAddys( "Опросить ток и пороги" ) {}
//------------------------------------------------------------------------------
void GetStendTask::PerformActionForAddy(unsigned addy, bool&, bool&) {
    DAK::GetDevState(addy, true);
}
//------------------------------------------------------------------------------
unsigned TryGetTestNum(unsigned n)
{
    return StrToIntDef( MyTransferTask::GetCommonScenaryValue(DAK::Test::NumberVar), -1 );
}
//------------------------------------------------------------------------------
FixTestResultTask::FixTestResultTask(unsigned nTest, unsigned nPGS, const std::vector<unsigned> &nParams ) :
    ForEachAddys( Format(nTest, nPGS, nParams) ),
    nTest_(nTest), nPGS_(nPGS), params_(nParams)
{
    
}
//------------------------------------------------------------------------------
AnsiString FixTestResultTask::What() const
{
    return Format(nTest_, nPGS_, params_);
}
//------------------------------------------------------------------------------
AnsiString FixTestResultTask::Format(unsigned nTest, unsigned nPGS, const std::vector<unsigned> &nParams)
{
    if(nTest>=DAK::Test::count) return "...";
    return MYSPRINTF_("Фиксировать показания в точке №%d, %s, %s", nPGS+1,
        DAK::Test::FormatTestParam(nTest, nPGS, nParams), DAK::Test::name[nTest] );
}
//------------------------------------------------------------------------------
void FixTestResultTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    TFrameDevicePasport *frameDevicePasport = Devs::GetFramePaspOfAddy(addy);
    assert(frameDevicePasport);
    for( unsigned n=0; n<params_.size(); ++n ) {
        const unsigned nParam = params_[n];
        const AnsiString s = DAK::Test::Row::GetVar(addy, nParam );
        Form1->AddAddyLog( addy, s );        
        frameDevicePasport->Set( s, nTest_, nPGS_+1, nParam+1 );
    }
}
//------------------------------------------------------------------------------
void FixTestResultTask::DoSaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set( elem, "номер_проверки", nTest_ );
    TiXMLAtr::Set( elem, "номер_колонки_проверки", nPGS_ );

    TiXMLAtr::Set(elem, "номера_строки_проверки", format_sorted_intervals( params_.begin(),
        params_.end() ).c_str() );
}
//------------------------------------------------------------------------------
void FixTestResultTask::DoLoadXML(TiXmlElement* elem)
{
    nTest_      = TiXMLAtr::Get( elem, "номер_проверки",            nTest_ );
    nPGS_       = TiXMLAtr::Get( elem, "номер_колонки_проверки",    nPGS_ );

    std::string s = TiXMLAtr::Get(elem, "номера_строки_проверки", s);
    ParseArrayOfIntervals(s, params_);
    std::sort(params_.begin(), params_.end());
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
FixTemperatureValTask::FixTemperatureValTask(unsigned gas, unsigned nPt) :
    ForEachAddys( Format(gas, nPt) ), gas_(gas), nPt_(nPt)
{

}
//------------------------------------------------------------------------------
AnsiString FixTemperatureValTask::Format(unsigned gas, unsigned nPt)
{   
    return MYSPRINTF_("Фиксировать для термокомпенсации точка №%d, газ %d", nPt+1,
        gas+1 );
}
//------------------------------------------------------------------------------ 
AnsiString FixTemperatureValTask::What() const
{
    return Format(gas_, nPt_);
}

//------------------------------------------------------------------------------
AnsiString ffloat( double v, int n )    {    return FormatFloat( "#0." + AnsiString::StringOfChar('#', n), v); }
//------------------------------------------------------------------------------
void FixTemperatureValTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    TFrameDevicePasport *frameDevicePasport = Devs::GetFramePaspOfAddy(addy);
    assert(frameDevicePasport);
    const unsigned row = 1 + gas_, col = 1 + nPt_*4;

    const double
        var1 = DAK::ReadVar1(addy, true),
        T = DAK::ReadT(addy, true),
        conc = DAK::GetModbusConc(addy, true).conc,
        i_lamp = DAK::ReadIlampOn (addy, true);

    int ndev = Devs::IndexOfAddy(addy);
    if(ndev>-1 && ndev<Devs::Count()) {
        unsigned knum = DAK::Kef::GetNumberOfKNum(33);
        if(knum!=-1) {  

            frameDevicePasport->Set(DAK::ReadKef(addy, knum),  DAK::Test::temperature, 0, 0);
        }
    }


    frameDevicePasport->Set(T,          DAK::Test::temperature, col+0, row);
    frameDevicePasport->Set(var1,       DAK::Test::temperature, col+1, row);
    frameDevicePasport->Set(conc,       DAK::Test::temperature, col+2, row);
    frameDevicePasport->Set(i_lamp,     DAK::Test::temperature, col+3, row);

    const DAK::Sets sets = DAK::Sets::Get();

    if(nPt_==0 || nPt_==2) {
        int nku_col0 = 5;
        const AnsiString strConc20 = frameDevicePasport->Get( DAK::Test::temperature, nku_col0+2, row );
        double conc20;
        if( DAK::Format::TryGetValue1(strConc20,conc20) ) {
            double dConcT = conc - conc20;
            double errorLimit = 5;
            bool isOk = true;

            if(!sets.isCH) {
                double maxD = sets.maxPogr[gas_];
                double pgs = sets.pgs[gas_];
                dConcT = conc - pgs;
                errorLimit = 0.5 * std::fabs( maxD*(T-20.0) ) / 10.0 ;
            } else {
                if(gas_>0) {
                    errorLimit = std::fabs( conc20 * 0.15 );
                } else {
                    errorLimit = 5;
                }
            }
            isOk = std::fabs(dConcT)<errorLimit;

            AnsiString sconc = "";
            sconc.sprintf( "%s %s%s%s [%s]",
                ffloat(conc,3),
                ffloat(dConcT,3),
                isOk ? "<" : ">",
                ffloat(errorLimit,3),
                isOk ? "+" : "-" );
            frameDevicePasport->Set(sconc,   DAK::Test::temperature, col+2, row);
        }
    }
    Devs::SaveForTermoFile("termo.dev");
    Devs::SaveFile( ( MyGetExePath()+"devices.xml" ).c_str() );
}
//------------------------------------------------------------------------------
void FixTemperatureValTask::DoSaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set( elem, "газ", gas_ );
    TiXMLAtr::Set( elem, "точка", nPt_ );
}
//------------------------------------------------------------------------------
void FixTemperatureValTask::DoLoadXML(TiXmlElement* elem)
{
    gas_ = TiXMLAtr::Get( elem, "газ",            gas_ );
    nPt_ = TiXMLAtr::Get( elem, "точка",    nPt_ );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CheckStabilityTask::CheckStabilityTask() : ForEachAddys("Проверка нестабильности показаний",
    1000*30)
{}
//------------------------------------------------------------------------------
void CheckStabilityTask::PerformBegin()
{
    addys_.clear();    
}
//------------------------------------------------------------------------------
void CheckStabilityTask::PerformEnd()
{
}
//------------------------------------------------------------------------------
void CheckStabilityTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    //CtrlSysImpl& ctrlSys = CtrlSys::Instance();
    //TransferManagerT& tmngr = TransferManager::Instance();
    isComplete = false;
    TFrameDevicePasport *frameDevicePasport = Devs::GetFramePaspOfAddy(addy);
    assert(frameDevicePasport);

    DAK::GetModbusConc(addy, false);
    DAK::GetDevState(addy, false);

    using namespace DAK::Test;
    const bool firstTimeAddy = addys_.find(addy)==addys_.end();

    if(firstTimeAddy) {
        for( unsigned nParametr=Row::conc; nParametr<Row::i+1; ++nParametr ) {
            const AnsiString s = DAK::Test::Row::GetVar(addy,nParametr );
            frameDevicePasport->Set( s, stability, 2, nParametr+1 );
            frameDevicePasport->Set( s, stability, 1, nParametr+1 );
        }
        addys_.insert(addy);
        return;
    }

    for( unsigned nParametr=Row::conc; nParametr<Row::i+1; ++nParametr ) {
        const AnsiString s = DAK::Test::Row::GetVar(addy,nParametr );

        const AnsiString
            s1 = frameDevicePasport->Get( stability, 2, nParametr+1 ),
            s2 = frameDevicePasport->Get( stability, 1, nParametr+1 );
        double v, vmin, vmax;
        const bool
            isV     = DAK::Format::TryGetValue1(s,v),
            isVmin    = DAK::Format::TryGetValue1(s1,vmin),
            isVmax    = DAK::Format::TryGetValue1(s2,vmax);
        if( isV && isVmin && v<vmin ) {
            frameDevicePasport->Set( s, stability, 2, nParametr+1 );
            vmin = v;
        }
        if( isV && isVmax && v>vmax) {
            frameDevicePasport->Set( s, stability, 1, nParametr+1 );
            vmax = v;
        }
        const AnsiString ss = DAK::Format::DeltaStab(vmax-vmin);
        frameDevicePasport->Set( ss, stability, 3, nParametr+1 );

        if(s.Pos("[-]") || ss.Pos("[-]")){
            isComplete = true;
            isFailed  = true;
        }
    }

}
//------------------------------------------------------------------------------
#undef ADD_TASK_
//------------------------------------------------------------------------------


