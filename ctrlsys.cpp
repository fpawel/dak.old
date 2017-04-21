//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "ctrlsys.h"
//------------------------------------------------------------------------------
#include <fstream>
#include <sstream>
#include <numeric>
#include <math>
//------------------------------------------------------------------------------
#include <Math.hpp>
#include <dateutils.hpp>
//------------------------------------------------------------------------------
#include "boost/regex.hpp"
//------------------------------------------------------------------------------
#include "TransferManage.h"
#include "AnsiStringUtils_.h"
#include "guicon.h"
#include "MasterSlaveIO.h"
#include "FooCalledOnce.hpp"
#include "MasterSlaveIOSettings.hpp"
#include "MasterSlaveIOSettingsIni.h"
#include "winRS232.h"
#include "IOProtocols.hpp"
#include "task_.h"
#include "TaskList.h"
#include "Unit1.h"
#include "MyIostream.h"
#include "uAppSetsDlg.h"
#include "MyModFN.hpp"
#include "ModbusAdapter.h"
#include "modbus_.h"
#include "pneumo.hpp"
#include "tools.h"
#include "vardef.hpp"
#include "MyStrValRef.hpp"
#include "MyWindow.h"
#include "WinErr.hpp"
#include "Devices.h"
#include "vardef.hpp"
#include "CommSetsIOHelper.h"
#include "AppXML.h"
#include "xmlUtils_.h"
#include "uFrameDevicePasport.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
#define COMPORT_MODBUS              "Компорт_на_модбас"
#define COMPORT_HEAT_CHAM_SEKT      "Компорт_печки"
#define IO_SETS_MODBUS              "Компорт_на_модбас_дополнительно"
#define HEAT_CHAM_IO_SETS_SEKT      "Компорт_печки_дополнительно"
//------------------------------------------------------------------------------

void HandleError()
{
    MyWCout(FormatMyExceptionMessages( MY_RETHROW_.get(),
        "\n\t", "\"%eMsg\" [%eLine:%eFile]")+"\n", MY_SET_CONSOLE_RED_TEXT);
}
//------------------------------------------------------------------------------
class CommPortNumHlpr : public MyStrValRef
{
public:
    explicit CommPortNumHlpr(my::RS232::Port& prt) : prt_(prt) {}
    virtual AnsiString Get() const { return prt_.GetPortName(); }
private:
    my::RS232::Port &prt_;
    virtual void UpdateReferenceValue()
    {
        prt_.SetPortName( GetStr() );
    }
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class CtrlSysImpl::Impl : public boost::noncopyable
{
public:
    explicit Impl();
    ~Impl();

    void SetupDialog();
    void Adjust(unsigned cmd, double pgs);
    IOSets GetIOSets()
    {
        IOSets ret;
        ret.modbus              = &modbus_;
        ret.heatCham            = &heatChamIO_;
        ret.pneumoAddy          = pneumoAddy_;
        ret.stendAddy           = jurikStendAddy_;
        ret.lastErrorSource     = ErrorSource();
        ret.isDeviceNoAnswer    = IsDeviceNoAnswer();
        return ret;
    }
    DAKSets GetDAKSets() const
    {
        DAKSets ret;
        ret.endScale    = Ceil( MyStrToFloatDef(DAK::MainKef::ExtractScaleOfValue( scale_ ).second,0) );
        ret.isCH        = IsCH();
        ret.gas         = gas_;
        ret.softVer     = softVer_;
        ret.ispolnenie  = isp_;
        ret.scale       = scale_;
        ret.units       = ret.isCH ? "%НКПР" : "%об.д";
        return ret;
    }
    AnsiString FormatTestParam(unsigned nTest, unsigned nPGS, const std::vector<unsigned> &params);
    AnsiString FormatTestParam(unsigned nTest, unsigned nPGS, unsigned param);
private:
    TransferManagerT& tmngr_;
    TiXmlElement *xml_;
    my::RS232::Port comPortModbus_, comPortHeatCham_;
    MasterSlaveIOSettings ioSetsModbus_, ioSetsHeatCham_;

    ModbusAdapter modbus_;
    MasterSlaveIOImpl heatChamIO_;
    unsigned pneumoAddy_, jurikStendAddy_;
    AnsiString scale_, gas_, softVer_, isp_;
    TFrameDevicePasport *frameDevicePasport_;
    
    PMyStrValRef storedRefs_[TFormAppSetsDlg::VALS_COUNT];

    LRESULT OnModbus(const RS485Adapter* thiS, unsigned state);
    void SaveIni();
    void AcceptKefs();
    AnsiString ErrorSource();
    bool IsDeviceNoAnswer();
    bool IsCH() const { return gas_=="CH4" || gas_=="C3H8"; }
};

//------------------------------------------------------------------------------
void LoadComPortXML(my::RS232::Port &port, const AnsiString& portSekt,
    MasterSlaveIOSettings& ioSets, const char* setsSekt)
{
    TiXmlElement *elem = TiXML::ChildElement( GetAppXML().config, setsSekt );
    
    my::RS232::Setings sets = port.GetSettings();
    my::RS232::LoadSetingsXML( elem, sets);
    port.SetSettings(sets);
    LoadMasterSlaveIOSettingsFromFile( ioSets, elem );

    port.SetPortName( TiXMLAtr::GetStr( elem, "порт", "COM1" ).c_str() );
}
void SaveComPortXML(my::RS232::Port &port, const AnsiString& portSekt,
    MasterSlaveIOSettings& ioSets, const char* setsSekt)
{
    TiXmlElement *elem = TiXML::ChildElement( GetAppXML().config, setsSekt );
    elem->SetAttribute( "порт", port.GetPortName().c_str() );
    my::RS232::Setings sets = port.GetSettings();
    my::RS232::SaveSetingsXML( elem, sets);
    SaveMasterSlaveIOSettingsToFile( ioSets, elem );

}
//------------------------------------------------------------------------------
CtrlSysImpl::Impl::Impl() :
    tmngr_( TransferManager::Instance() ),
    xml_( GetAppXML().config ),
    comPortModbus_(), ioSetsModbus_(),
    comPortHeatCham_(), ioSetsHeatCham_(),
    modbus_(ioSetsModbus_, &comPortModbus_),
    heatChamIO_(ioSetsHeatCham_, &comPortHeatCham_),
    pneumoAddy_         ( TiXMLAtr::Get(xml_, "адрес_пневмоблока", 127) ),
    jurikStendAddy_     ( TiXMLAtr::Get(xml_, "адрес_стенда", 127) ),
    scale_              ( TiXMLAtr::GetStr(xml_, "шкала", "0-100" ).c_str() ),
    gas_                ( TiXMLAtr::GetStr(xml_, "газ", "CH" ).c_str() ),
    softVer_            ( TiXMLAtr::GetStr(xml_, "версия_ПО", "" ).c_str() ),
    isp_                ( TiXMLAtr::GetStr(xml_, "исполнение", "" ).c_str() ),
    frameDevicePasport_( new TFrameDevicePasport(NULL) )
{
    ASSERT_FUNCTION_CALLED_ONCE__;
    for( int n=0; n<DAK::GetKefs().size(); ++n) {
        MyWCout(AnsiString().sprintf("%d %d %s\n", n, DAK::GetKefs()[n].n, DAK::GetKefs()[n].caption ));
    }
    LoadComPortXML(comPortModbus_, COMPORT_MODBUS, ioSetsModbus_, IO_SETS_MODBUS);
    modbus_.SetNotifyState( &OnModbus );

    LoadComPortXML(comPortHeatCham_, COMPORT_HEAT_CHAM_SEKT,
        ioSetsHeatCham_, HEAT_CHAM_IO_SETS_SEKT);

    using namespace AppSetsGrdItems::Row;

    #define SSET_(ss,nn) storedRefs_[AppSetsGrdItems::Row::##ss##-1].reset( nn )

    SSET_(COM,              new CommPortNumHlpr( comPortModbus_ ) );
    SSET_(COM_heatcham,     new CommPortNumHlpr( comPortHeatCham_ ) );

    SSET_(Timeout,          CreateMyIntStrValRef(ioSetsModbus_.timeOut_) );
    SSET_(WaitDelay,        CreateMyIntStrValRef(ioSetsModbus_.tmWriteDelay_) );
    SSET_(SilentTime,       CreateMyIntStrValRef(ioSetsModbus_.silentTime_) );
    SSET_(RepeatCount,      CreateMyIntStrValRef(ioSetsModbus_.repeatCount_) );

    SSET_(Timeout_heatcham,         CreateMyIntStrValRef(ioSetsHeatCham_.timeOut_) );
    SSET_(WaitDelay_heatcham,       CreateMyIntStrValRef(ioSetsHeatCham_.tmWriteDelay_) );
    SSET_(SilentTime_heatcham,      CreateMyIntStrValRef(ioSetsHeatCham_.silentTime_) );
    SSET_(RepeatCount_heatcham,     CreateMyIntStrValRef(ioSetsHeatCham_.repeatCount_) );

    SSET_(PneumoAddy,       CreateMyIntStrValRef(pneumoAddy_) );
    SSET_(JurikStendAddy,   CreateMyIntStrValRef(jurikStendAddy_) );

    SSET_(Gas,    new MyStrValRefStr(gas_) );
    SSET_(Scale,  new MyStrValRefStr(scale_) );

    SSET_(SoftVer,  new MyStrValRefStr(softVer_) );
    SSET_(Isp,      new MyStrValRefStr(isp_) );
    #undef SSET_
}
//------------------------------------------------------------------------------
CtrlSysImpl::Impl::~Impl()
{
    ASSERT_FUNCTION_CALLED_ONCE__;
    LOG_FOO_LIFETIME_QUOTES;
    tmngr_.StopThread();
    SaveIni();
}
//------------------------------------------------------------------------------
AnsiString CtrlSysImpl::Impl::FormatTestParam(unsigned nTest, unsigned nPGS, const std::vector<unsigned> &params)
{
    if(nTest>=DAK::Test::count) return "";
    AnsiString s;
    for(unsigned n=0; n<params.size(); ++n) {
        s += frameDevicePasport_->Get(nTest, 0, params[n]+1 );
        if(n!=params.size()-1) s+=", ";
    }
    s += " для "+frameDevicePasport_->Get(nTest, nPGS+1, 0 );
    return s;
}
AnsiString CtrlSysImpl::Impl::FormatTestParam(unsigned nTest, unsigned nPGS, unsigned param)
{
    if(nTest>=DAK::Test::count) return "";
    return frameDevicePasport_->Get(nTest, 0, param+1 )+ ", "+frameDevicePasport_->Get(nTest, nPGS+1, 0 );
}
//------------------------------------------------------------------------------
void CtrlSysImpl::Impl::SaveIni()
{
    SaveComPortXML(comPortModbus_,      COMPORT_MODBUS,
        ioSetsModbus_,      IO_SETS_MODBUS);
    SaveComPortXML(comPortHeatCham_,    COMPORT_HEAT_CHAM_SEKT,
        ioSetsHeatCham_,    HEAT_CHAM_IO_SETS_SEKT);
    TiXMLAtr::Set( xml_, "адрес_пневмоблока", pneumoAddy_);
    TiXMLAtr::Set( xml_, "адрес_стенда", jurikStendAddy_);
    TiXMLAtr::Set( xml_, "шкала", scale_ );
    TiXMLAtr::Set( xml_, "газ", gas_ );
    TiXMLAtr::Set( xml_, "версия_ПО", softVer_ );
    TiXMLAtr::Set( xml_, "исполнение", isp_ );
}
//------------------------------------------------------------------------------
AnsiString CtrlSysImpl::Impl::ErrorSource()
{
    const MyException *excpt = tmngr_.GetLastException();
    const MyTransferException *e = excpt->As<MyTransferException>();
    if(!e) return "";

    if( e->IO()==&modbus_.MasterSlave() )
    {
        const unsigned addy = modbus_.Addy();
        if( addy== pneumoAddy_)
            return "пневмоблок";
        else if(addy== jurikStendAddy_)
            return "стенд";
        else if( Devs::IndexOfAddy(addy)!=-1 )
            return "прибор";
    }
    else if( e->IO()==&heatChamIO_ )
        return "термокамера";
    return "";
}
//------------------------------------------------------------------------------
bool CtrlSysImpl::Impl::IsDeviceNoAnswer()
{
    const MyException *excpt = tmngr_.GetLastException();
    const MyNoAnswerException *e = excpt->As<MyNoAnswerException>();
    return e && e->IO()==&modbus_.MasterSlave();
}
//------------------------------------------------------------------------------
LRESULT CtrlSysImpl::Impl::OnModbus(const RS485Adapter* adpt, unsigned state)
{
    const unsigned addy = adpt->Addy();
    if( state==RS485Adapter::State::NO_ANSWER )
    {
        Devs::SetNotAnswered( addy );
        return 0;
    }
    if( state==RS485Adapter::State::END )
        Devs::SetAnswered( addy );

    return 0;
}
//------------------------------------------------------------------------------

void CtrlSysImpl::Impl::AcceptKefs()
{
    const DAK::MainKef::Scale0K scale = DAK::MainKef::ExtractScaleOfValue( scale_ );
    const DAKSets sets = GetDAKSets();


    for(unsigned nDev=0; nDev<Devs::Count(); ++nDev) {
        #define SSET_(ss,nn) Devs::SetKef(nDev, MAIN_KEF_N_(ss), nn )
        SSET_(SoftVer, sets.softVer );
        SSET_(Year, YearOf(Now()) );
        SSET_(Gas, MAIN_KEF_(Gas).Key2Val( sets.gas ) );
        SSET_(Units, MAIN_KEF_(Units).Key2Val(sets.units)  );

        SSET_(Scale,    MAIN_KEF_(Scale).Key2Val(sets.scale) );
        SSET_(Scale0,   scale.first );
        SSET_(ScaleK,   scale.second );
        SSET_(Diap,     scale.second );

        SSET_(Isp, isp_ );
        SSET_(DevType, 11 );

        SSET_(PGS1, Pneumo::Conc(0) );
        SSET_(PGS3, Pneumo::Conc(2) );

        SSET_(Serial, Devs::Serial(nDev) );

        #undef SSET_
    }
}

//------------------------------------------------------------------------------
void CtrlSysImpl::Impl::SetupDialog()
{
    struct H
    {
        H() : tmngr_(TransferManager::Instance()), OnDestruct_(NULL),
            wasStarted_( !tmngr_.IsPaused() )
        {
            if(wasStarted_)
                tmngr_.SetPause(true);
        }
        ~H() {
            if(OnDestruct_) OnDestruct_();
            if(wasStarted_) tmngr_.SetPause(false);
        }
        TransferManagerT& tmngr_;
        const bool wasStarted_;
        void (__closure *OnDestruct_)();
    };
    H h;
    boost::shared_ptr<TFormAppSetsDlg> dlg( new TFormAppSetsDlg(NULL, storedRefs_) );
    using namespace AppSetsGrdItems::Row;
    #define VVAl_(nn) dlg->grd1->Cells[2][AppSetsGrdItems::Row::##nn##]
    VVAl_(ShowComPortLog) =  ioSetsModbus_.mustLogData_ ? "Да" : "Нет" ;

    dlg->ShowModal();
    if( dlg->ModalResult==mrOk )
    {
        ioSetsModbus_.mustLogData_ = VVAl_(ShowComPortLog)=="Да";
        ioSetsHeatCham_.mustLogData_ = ioSetsModbus_.mustLogData_;
        if(dlg->CheckBox1->Checked)
            h.OnDestruct_ = &AcceptKefs;
    }
    #undef VVAL_
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
CtrlSysImpl::CtrlSysImpl() : impl_( new Impl ) {}
//------------------------------------------------------------------------------
void CtrlSysImpl::SetupDialog()
{
    impl_->SetupDialog();
}
//------------------------------------------------------------------------------
CtrlSysImpl::IOSets CtrlSysImpl::GetIOSets()
{
    return impl_->GetIOSets();
}
CtrlSysImpl::DAKSets CtrlSysImpl::GetDAKSets()
{
    return impl_->GetDAKSets();
}
void CtrlSysImpl::Adjust(unsigned cmd, double pgs)
{
    impl_->Adjust(cmd, pgs);
}
//------------------------------------------------------------------------------
AnsiString CtrlSysImpl::FormatTestParam(unsigned nTest, unsigned nPGS, const std::vector<unsigned> &params)
{
    return impl_->FormatTestParam(nTest, nPGS, params);
}

AnsiString CtrlSysImpl::FormatTestParam(unsigned nTest, unsigned nPGS, unsigned param)
{
    return impl_->FormatTestParam(nTest, nPGS, param);
}
//------------------------------------------------------------------------------
bool IsNotAnsweredAddy(unsigned addy)
{
    return CtrlSys().Instance().GetIOSets().modbus->IsNotAnsweredAddy(addy);
}
//------------------------------------------------------------------------------
void AdjustSensorsNull()
{
    CtrlSysImpl &ctrlSys = CtrlSys::Instance();
    ctrlSys.Adjust( DAK::Cmd::Code::adj_0, Pneumo::Conc(Scn::NULL_GAS_IDX) );
}
//------------------------------------------------------------------------------
void AdjustSensorsEnd()
{
    CtrlSysImpl &ctrlSys = CtrlSys::Instance();
    ctrlSys.Adjust( DAK::Cmd::Code::adj_s, Pneumo::Conc(Scn::SENS_GAS_IDX) );
}
//------------------------------------------------------------------------------



