//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "pneumoImpl.h"
//------------------------------------------------------------------------------
#include <fstream>
//------------------------------------------------------------------------------
#include <dateutils.hpp>
#include <ExtCtrls.hpp>
//------------------------------------------------------------------------------
#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"
//------------------------------------------------------------------------------
#include "SGrdHlpr.h"
#include "guicon.h"
#include "MyExcpt.hpp"
#include "AnsiStringUtils_.h"

#include "uFormPneumoGrd.h"
#include "MyAbstructTransfer.hpp"
#include "MySynchronize.h"
#include "TransferManage.h"
#include "TaskList.h"
#include "ctrlsys.h"
#include "ModbusAdapter.h"
#include "modbus_.h"
#include "MyModFN.hpp"
#include "Unit1.h"
#include "AppXML.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
#define INI_SEKT  "Пневмоблок"
#define CONC_ID "Концентрация"
//------------------------------------------------------------------------------
namespace Pneumo
{
//------------------------------------------------------------------------------
AnsiString BlowGasAction2Str(unsigned nGas, unsigned blowTimeMSec)
{
    const AnsiString sTime = FormatMillisecond2Minutes(blowTimeMSec);

    const Gases& pgsLst = GetGases();
    if ( nGas < pgsLst.size() )
    {
        const Gas& gas = pgsLst.at(nGas);
        return MYSPRINTF_( "Продувка %s в течение %s мин.", gas.name, sTime );
    } else
        return MYSPRINTF_( "Выключить пневмоблок на %s мин.", sTime );
}
AnsiString PneumoblockGasAction2Str(unsigned nGas)
{
    const Gases& pgsLst = GetGases();
    if ( nGas < pgsLst.size() )
    {
        const Gas& gas = pgsLst.at(nGas);
        return MYSPRINTF_( "Включить клапан №%d, %s", nGas+1, gas.name );
    } else
        return "Выключить пневмоблок";
}
//------------------------------------------------------------------------------
class SwitchTask : public MyTransferTask
{
public:
    SwitchTask( unsigned nGas, TNotifyPneumoGasEvent switchFunctor ) :
        nGas_(nGas), switchFunctor_(switchFunctor)
    {}
	virtual ~SwitchTask() {}
    AnsiString What() const{ return PneumoblockGasAction2Str(nGas_); }
private:
    const unsigned nGas_;
    const TNotifyPneumoGasEvent switchFunctor_;
    virtual bool DoHandleBeginTransfer() { return true; }
    virtual bool DoHandleEndTransfer() { return false; }
    virtual void PerformAction()
    {
        switchFunctor_(nGas_);
    }
};
//------------------------------------------------------------------------------
Gases CreateGasesList(TiXmlElement *xml)
{
    Gases ret;
    for( TiXmlElement* elem = xml->FirstChildElement(); elem; elem = elem->NextSiblingElement() )
    {
    	Gas gas;
        gas.name        = TiXMLAtr::GetStr  (elem, "газ").c_str();
        gas.caption     = TiXMLAtr::GetStr  (elem, "примечание").c_str();
        gas.conc        = Str2FloatDef( TiXMLAtr::GetStr(elem, "концентрация", ""), 0);
        gas.n           = ret.size();
        ret.push_back(gas);
    }
    return ret;
}
//------------------------------------------------------------------------------
class Pneumoblock : public boost::noncopyable
{
public:
	static Pneumoblock& Instance()
    {
    	static Pneumoblock* pRet = NULL;
        if(pRet==NULL) pRet = new Pneumoblock;
        return *pRet;
    }
    void AddSwitchTask(unsigned nGas);
    void Switch(unsigned nGas);
    void SwitchOffIgnoreConnectionError();
    unsigned Current() const { return curentGas_; }
    unsigned CurrentDuration() const { return ::GetTickCount() - curentStartTime_; }
    void Save();

    const Gases& GetGases() { return gases_; }

    TStringGrid *GetControl() { return grd_; }

    void PlaneSwitchGas( unsigned nGas )
    {
        tmngr_.AddTask( new SwitchTask(nGas, &Switch) );
    }

    //boost::shared_ptr<void> ConnectOnAfterSwitchSignal(Pneumo::TOnAfterSwitchHandler method){ return onAfterSwitchSignal_.GetConnections().AutoConnect(method);    }
private:
	explicit Pneumoblock();
    ~Pneumoblock() {}

    TransferManagerT &tmngr_;
    TiXmlElement *xml_;

    unsigned curentGas_;
    unsigned curentStartTime_;

    const Gases gases_;
    //Pneumo::TOnAfterSwitchSignal onAfterSwitchSignal_;

    TFormPneumoGrd *formPneumoGrd_;
    TStringGrid* grd_;
    PVoid connectOnTransferManagerReport_;

    LRESULT Update(unsigned n);
    void OnUserSelectPneumoGas(unsigned nGas);
    void __fastcall Timer1Timer(TObject *Sender);
    void OnTransferManagerReport(unsigned context, const AnsiString& state);
};
//------------------------------------------------------------------------------
Pneumoblock::Pneumoblock()
    :
    tmngr_( TransferManager::Instance() ),
    xml_( GetAppXML().pneumoBlock ),
    curentGas_(-1), curentStartTime_( ::GetTickCount() ),
    gases_( CreateGasesList(xml_) ),
    formPneumoGrd_( new TFormPneumoGrd(NULL, gases_, &OnUserSelectPneumoGas) ),
    grd_( formPneumoGrd_->grdPneumo ),
    connectOnTransferManagerReport_( TransferManager().Instance().
        ConnectOnReportState(OnTransferManagerReport) )
{
    formPneumoGrd_->Timer1->OnTimer = Timer1Timer;
    formPneumoGrd_->Timer1->Interval = 500;
    formPneumoGrd_->Timer1->Enabled = 1;
}
//------------------------------------------------------------------------------
void Pneumoblock::Save()
{
    TiXmlElement* elem = xml_->FirstChildElement();
    for( unsigned i=0; i<gases_.size(); ++i, elem = elem->NextSiblingElement() )
    {
    	const Gas gas = gases_[i] ;
        TiXMLAtr::SetStr( elem, "газ",          gas.name.c_str() );
        TiXMLAtr::SetStr( elem, "примечание",   gas.caption.c_str() );
        TiXMLAtr::SetStr( elem, "концентрация", grd_->Cells[2][i+1].c_str() );
    }
}
//------------------------------------------------------------------------------
void __fastcall Pneumoblock::Timer1Timer(TObject *Sender)
{
    if( !( curentGas_<gases_.size() ) ) return;
    grd_->Cells[5][curentGas_+1] = FormatDateTime( "hh:nn:ss",
        IncMilliSecond( 0, CurrentDuration() ) );    
}
//------------------------------------------------------------------------------
void Pneumoblock::OnUserSelectPneumoGas(unsigned nGas)
{
    if(curentGas_==nGas) nGas = -1;
    AddSwitchTask(nGas);
}
//------------------------------------------------------------------------------
void Pneumoblock::AddSwitchTask(unsigned nGas)
{
    tmngr_.StopThread();
    //tmngr_.GetTaskList().EraseTasksOfClass<SwitchTask>();
    tmngr_.GetTaskList().Clear();
    tmngr_.AddTask( new SwitchTask(nGas, &Switch), TransferManagerT::ADD_TASK_HEAD );
    tmngr_.StartThread();    
}
//------------------------------------------------------------------------------
LRESULT Pneumoblock::Update(unsigned n)
{
    MYCOUT_( "Пневмоблок %d\n", n ));
    //const unsigned oldGas = curentGas_;
    curentGas_ = n;
    curentStartTime_ = ::GetTickCount();
    //onAfterSwitchSignal_.Signal2(oldGas, curentGas_ );
	for( int i=1; i<grd_->RowCount; ++i )
    	my::Grd::SelectRow( grd_, i, i-1!=(int)curentGas_ );
    if( curentGas_<gases_.size() )
        grd_->Cells[4][curentGas_+1] = TimeToStr( Time() );        
    Form1->AddLogAll( Pneumo::PneumoblockGasAction2Str(curentGas_),
        RichTextAttr().TextColor(clMaroon).Time() ) ;
    return 0;
}
//------------------------------------------------------------------------------
void HardSwitchPneumo(unsigned n)
{
    CtrlSysImpl& ctrlSys = CtrlSys::Instance();
    CtrlSysImpl::IOSets io = CtrlSys::Instance().GetIOSets();
    ModbusAdapter& modbus = *io.modbus;
    const unsigned addy = io.pneumoAddy;

    if(!addy) return;

    enum
    {
        MODUS_WRITE_STR_LEN = 7,
    	MODUS_WRITE_REGISTER_COMMAND_CODE = 0x10,
        MODUS_WRITE_REGISTER_ADDR = 32,
        MODUS_WRITE_REGISTERS_COUNT = 1,
        MODUS_WRITE_DATA_LEN = 2
    };
	uInt8 txd[MODUS_WRITE_STR_LEN];
    txd[0] = MODUS_WRITE_REGISTER_ADDR >> 8;
    txd[1] = MODUS_WRITE_REGISTER_ADDR;
    txd[2] = MODUS_WRITE_REGISTERS_COUNT>>8;
    txd[3] = MODUS_WRITE_REGISTERS_COUNT;
    txd[4] = MODUS_WRITE_DATA_LEN;
    // данные
    txd[5] = 0;
    txd[6] = n;
    modbus.PerformTransfer(addy, MODUS_WRITE_REGISTER_COMMAND_CODE, txd, txd+MODUS_WRITE_STR_LEN );
    if( (modbus.AcceptedDataSize()!=4) || !std::equal(txd, txd+4, modbus.AcceptedData()) )
        throw PMyExcpt( new MyTransferException(__FILE_LINE__,
            "Ошибка обмена с пневмоблоком", &modbus.MasterSlave() ) );
}
//------------------------------------------------------------------------------
void Pneumoblock::Switch(unsigned nGas)
{
    //if( nGas==curentGas_ ) return;
    if( nGas>=GetGases().size() ) nGas = -1;
    const AnsiString msg = PneumoblockGasAction2Str(nGas);
    MyWCout( msg+"\n"  );

    // выполнить апаратное переключение

    if(nGas!=-1)
        HardSwitchPneumo(nGas+1);
    else
        HardSwitchPneumo(0);
    my::Synchronize1( &Update, nGas,  __FILE_LINE__ );
}

//------------------------------------------------------------------------------
void Pneumoblock::SwitchOffIgnoreConnectionError()
{
    MyWCout( PneumoblockGasAction2Str(-1)+"\n"  );
    // выполнить апаратное переключение
    try
    {
        HardSwitchPneumo(0);
    } catch(...) {}
    my::Synchronize1( &Update, -1, __FILE_LINE__ );
}

//------------------------------------------------------------------------------
void Pneumoblock::OnTransferManagerReport(unsigned context, const AnsiString& state)
{
    /*
    const bool
        isStartThread    	= context==TransferManagerT::State::START,
        isStopThread        = context==TransferManagerT::State::STOP,
        isError             = context==TransferManagerT::State::ERR
    ;


    if ( isBeginTransfer || isEndTransfer )
    	HandleBeginOrEndTransfer(state, isBeginTransfer);
    */

    //if( isError ) SwitchOff();
}
//------------------------------------------------------------------------------

const Gases& GetGases()
{
    return Pneumoblock::Instance().GetGases();
}
//------------------------------------------------------------------------------
void AddSwitchGasTask(unsigned nGas)
{
    Pneumoblock::Instance().AddSwitchTask(nGas);
}
//------------------------------------------------------------------------------
void PlaneSwitchGasTask( unsigned nGas )
{
    Pneumoblock::Instance().PlaneSwitchGas(nGas);
}
//------------------------------------------------------------------------------

void SwitchGas(unsigned n)
{
    Pneumoblock::Instance().Switch(n);
}
//------------------------------------------------------------------------------
void SwitchOffIgnoreConnectionError()
{
    Pneumoblock::Instance().SwitchOffIgnoreConnectionError();
}
//------------------------------------------------------------------------------
unsigned Current()
{
    return Pneumoblock::Instance().Current();
}
//------------------------------------------------------------------------------
unsigned CurrentDuration()
{
    return Pneumoblock::Instance().CurrentDuration();
}
//------------------------------------------------------------------------------
TStringGrid *GetControl()
{
    return Pneumoblock::Instance().GetControl();
}
//------------------------------------------------------------------------------
double Conc(unsigned nGas)
{
    return nGas<GetGases().size() ? MyStrToFloatDef( GetControl()->Cells[2][nGas+1], 0 ) : 0;
}

void SaveConc()
{
    Pneumoblock::Instance().Save();

}

//------------------------------------------------------------------------------
};   // Pneumo
//------------------------------------------------------------------------------
