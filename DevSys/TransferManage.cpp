#include <map>
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "TransferManage.h"
//------------------------------------------------------------------------------
#include <windows.h>
#include <vcl.h>
#include <DateUtils.hpp>
//------------------------------------------------------------------------------
// loki
#include "Loki\singleton.h"
//------------------------------------------------------------------------------
#include "..\VCLUtils\myThread.h"
#include "TaskList.h"
#include "MyExcpt.hpp"
#include "uFrmWait.h"
#include "AnsiStringUtils_.h"
#include "MyExeptionImpl.h"
#include "guicon.h"
#include "FooCalledOnce.hpp"
#include "MyShllFldr.h"
#include "MySynchronize.h"
#include "AppXML.h"
#include "xmlUtils_.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma option -xp
//------------------------------------------------------------------------------
#define TRANSFERS_SEKT "Трансфер"
#define TRANSFERS_THREAD_PRIORITY_ID "приоритет_второго_потока"
//------------------------------------------------------------------------------

AnsiString FormatTimeSpecial1(unsigned msec)
{
    return FormatDateTime("hh:nn:ss:zzz", IncMilliSecond(0, msec) );
}
//------------------------------------------------------------------------------
LRESULT HideTopmostPanel1(TFrmWait *topPanel)
{
    topPanel->Parent = NULL;
    topPanel->Hide();
    topPanel->pb1->Visible = 0;
    topPanel->pb1->Position = 0;
    topPanel->pb1->Min = 0;
    topPanel->pb1->Max = 0;
    topPanel->Panel1->Visible = 0;
    //throw;
    return 0;
}
void HideTopmostPanel(TFrmWait *topPanel)
{
    my::Synchronize1( &HideTopmostPanel1, topPanel, __FILE_LINE__ );
}
//------------------------------------------------------------------------------
LRESULT SetTopmostPanelCaption( TFrmWait *topPanel, const AnsiString& s )
{
    topPanel->Panel2->Caption = s;
    return 0;
}
//------------------------------------------------------------------------------
LRESULT ShowTopmostPanel1( TFrmWait *topPanel, const AnsiString& s, bool showProgress )
{
    topPanel->Parent = Screen->ActiveForm;
    topPanel->Top = 0;
    topPanel->Align = alTop;
    topPanel->Label2->Caption = "00:00:00";
    if(topPanel->Visible && topPanel->Panel2->Caption == s) return 0;
    topPanel->Panel2->Caption = s;
    if(!topPanel->Visible)
        topPanel->FixPosition();
    topPanel->Position = poScreenCenter;
    topPanel->Show();
    topPanel->Repaint();

    if( showProgress )
    {
        topPanel->pb1->Visible = 1;
        topPanel->pb1->Position = 0;
        topPanel->pb1->Min = 0;
        topPanel->pb1->Max = 0;
        topPanel->Panel1->Visible = 1;

    }


    return 0;
}
void ShowTopmostPanel( TFrmWait *topPanel, const AnsiString& s, bool showProgress )
{
    my::Synchronize3( &ShowTopmostPanel1, topPanel, s, showProgress, __FILE_LINE__ );
}
//------------------------------------------------------------------------------
LRESULT SetTopmostPanelProgress1( TFrmWait *topPanel, unsigned v, unsigned vMax, unsigned vMin )
{
    if( topPanel->pb1->Visible )
    {
        if(v!=-1)
        {
            if(v==-2)
                topPanel->pb1->Position = topPanel->pb1->Position + 1;
            else
                topPanel->pb1->Position = v;
        }
        if(vMax!=-1)
            topPanel->pb1->Max = vMax;
        if(vMin!=-1)
            topPanel->pb1->Min = vMin;

        topPanel->Repaint();
    }
    return 0;
}

void SetTopmostPanelProgress( TFrmWait *topPanel, unsigned v, unsigned vMax, unsigned vMin )
{
    my::Synchronize4( &SetTopmostPanelProgress1, topPanel, v, vMax, vMin, __FILE_LINE__ );
}


class TopmostPanelHelper : public boost::noncopyable
{
public:
    TopmostPanelHelper(TFrmWait *topPanel, const AnsiString& s ) : topPanel_(topPanel)
    {
        ShowTopmostPanel( topPanel, s, false );
    }
    ~TopmostPanelHelper()
    {
        HideTopmostPanel(topPanel_);
    }
private:
    TFrmWait *topPanel_;
};

class FlagRester : public boost::noncopyable
{
public:
    explicit FlagRester(bool& flag_) : flag(flag_) { flag=1; }
    ~FlagRester() { flag = 0; }
private:
    bool& flag;
};

//------------------------------------------------------------------------------
class TransferManagerT::Impl : public boost::noncopyable
{
public:
    explicit Impl();
    ~Impl();
    //--------------------------------------------------------------------------
    // управление потоком
    void StartThread();
    void StopThread();

    void SetPause(bool isPause);
    bool IsPaused() const { return paused_; }

    bool IsThreadStopedOrTerminated() const { return thr_->IsTerminated() || thr_->WasStoped(); }
    bool IsFinalScripPerforming() const { return  isFinalScripPerforming_; }

    //--------------------------------------------------------------------------
	TReportStateSignal::Connections& ReportStateConnections()
    { return signalReportState_.GetConnections(); }
    //---------------------------------------------------------------------------
    TaskList& GetTaskList() { return taskList_; }
    //---------------------------------------------------------------------------
    void DebugConsoleInput();

    // Выполнить трансфер приёмопередачи.
	void PerformTransfer( uInt8 modusAddr, uInt8 commandCode, ByteBuffPtr sendBegin,
    	ByteBuffPtr sendEnd, ByteBuffPtr& reciveBegin, ByteBuffPtr& reciveEnd );

    // упрвление списком - добавить задание
    void AddTask( MyTransferTask *trnsfr, bool addTail = ADD_TASK_TAIL)
    {
        if( addTail==ADD_TASK_TAIL )
            taskList_.PushBackTail( PTransfer(trnsfr) );
        else
            taskList_.PushFrontHead( PTransfer(trnsfr) );
    }

    void AddTask( TVoidMthd mthd, const AnsiString& what, bool addTail );
   
    void SetFinalTask( PTransfer task) { finalTask_ = task; }
    void SetBeginTask( PTransfer task) { beginTask_ = task; }
    void SetRepeatScript(const Scenary& scn) {repeatScript_= scn;}

    void ResetScripts()
    {
        StopThread();
        Scenary noScript;
        SetRepeatScript(noScript);
        finalTask_.reset();
        beginTask_.reset();
        taskList_.Clear();
    }

    void SyncronizedSleep( unsigned tm );

    const MyException* GetLastException() const
    {
        return lastExcpt_.get();
    }

    void HideTopPanel(){ HideTopmostPanel( topPanel_);    }
    void ShowTopPanel( const AnsiString& s, bool showProgress ){ ShowTopmostPanel( topPanel_, s, showProgress); }
    void SetTopPanelProgress( unsigned v, unsigned vMax, unsigned vMin )
    {
        SetTopmostPanelProgress( topPanel_, v, vMax, vMin);
    }
    void SetEnableTopPanelTimeProgress( bool eneble ) { topPanel_->SetEnableTimeProgress(eneble); }
    void SetTopPanelCaption( const AnsiString& s )
    {
        topPanel_->Panel2->Caption = s;
        topPanel_->FixPosition();
    }

private:
    boost::shared_ptr<void> logLifeTime_;
    //--------------------------------------------------------------------------
    //  поток
    //boost::shared_ptr<my::Thread> thr_;
    my::Thread *thr_;
	//--------------------------------------------------------------------------
    // список планируемых трансферов
    TaskList taskList_;
    Scenary repeatScript_;
    PTransfer finalTask_, beginTask_;
    //--------------------------------------------------------------------------
    TReportStateSignal signalReportState_;
    //--------------------------------------------------------------------------
    // последнее зафиксированное исключение
    PMyExcpt lastExcpt_;
    //--------------------------------------------------------------------------
    TFrmWait *topPanel_;
    bool goingStop_, isFinalScripPerforming_;
    bool mutable paused_;

    const unsigned mainThreadId_;

    struct TerminateFromThreadSignal {};


	// потоковая продцедура
	void __fastcall Execute();
    //--------------------------------------------------------------------------
    LRESULT SignalReportState1(unsigned context, const AnsiString& s)
    {
        signalReportState_.Signal2(context, s);
        return 0;
    }
    // отправить отчёт
    void SignalReportState(unsigned context, const AnsiString& s)
    {
        my::Synchronize2( &SignalReportState1, context, s, __FILE_LINE__  );
    }
    //--------------------------------------------------------------------------


    //--------------------------------------------------------------------------
    //подготовить задание из списка - вызывать из потока
    PTransfer PrepareCurentTask();
    //--------------------------------------------------------------------------
    //              реакциИ на событиЯ
    LRESULT HandleBeginTransfer();
    LRESULT HandleEndTransfer();
    LRESULT HandleCurentException();

    void __fastcall OnBtnCloseClick( TObject* )
    {
        StopThread();
    }
};
//------------------------------------------------------------------------------
TransferManagerT::Impl::Impl() :
    logLifeTime_( MakeMyLogQuote("Transfer manager life time") ),
    // оконный объект данного модуля
	thr_( new my::Thread(&Execute) ),
    taskList_(),
    signalReportState_(), paused_(false),
    mainThreadId_( ::GetCurrentThreadId() )
{
    ASSERT_FUNCTION_CALLED_ONCE__;

    thr_->SetPriority( static_cast<TThreadPriority> (
        TiXMLAtr::Get(GetAppXML().config, "приоретет_второго_потока", tpLowest)
        ) );


    topPanel_ = new TFrmWait(NULL);
    topPanel_->Panel2->Font->Size = 14;
    topPanel_->btnClose->OnClick = OnBtnCloseClick;
}
//------------------------------------------------------------------------------
TransferManagerT::Impl::~Impl()
{
	//LOG_FOO_LIFETIME_QUOTES;
    try
    {
        TiXMLAtr::Get(GetAppXML().config, "приоретет_второго_потока", thr_->GetPriority() );
        signalReportState_.GetConnections().DisconnectAll();
        //assert( thr_->WasStoped() );
    }
    catch (...)
    {
        assert(false);
    }

}
//------------------------------------------------------------------------------
void TransferManagerT::Impl::SyncronizedSleep( const unsigned msec )
{
    const unsigned moment0 = ::GetTickCount();
    while( !IsThreadStopedOrTerminated() && (GetTickCount()-moment0 < msec) ) {
        Sleep(1);
        if(! paused_) continue;
        const bool vis = topPanel_->Visible;
        topPanel_->Visible = 0;
        while( paused_ ) ::Sleep(1);
        topPanel_->Visible = vis;
    }
}
//------------------------------------------------------------------------------
// управление потоком
void TransferManagerT::Impl::StartThread()
{
	//LOG_FOO_LIFETIME_QUOTES;
    if( paused_ ) {
        paused_ = false;
        return;
    }

    lastExcpt_.reset();
    thr_->Run();
    isFinalScripPerforming_ = 0;
}

//------------------------------------------------------------------------------
void TransferManagerT::Impl::StopThread()
{
    paused_ = false;
    if(isFinalScripPerforming_) return;


    if( ::GetCurrentThreadId()!=mainThreadId_ )
        throw TerminateFromThreadSignal();


    if( ::InSendMessage()==TRUE )
    {
        goingStop_ = 1;
        return;
    }

    //ShowTopPanel("Выполняется закрытие соединения...");
    const TThreadPriority threadPriority = thr_->GetPriority();
    thr_->SetPriority( tpTimeCritical );
    thr_->Stop();
    thr_->SetPriority( threadPriority );
    SignalReportState( State::STOP, "Опрос остановлен" );
    HideTopPanel();
    isFinalScripPerforming_ = 0;
}
//------------------------------------------------------------------------------
void TransferManagerT::Impl::SetPause(bool isPause)
{
    if( this->IsThreadStopedOrTerminated() || isPause==paused_) return;
    if(isPause)    {
        paused_ = true;
        SignalReportState(State::PAUSED, "");
    } else {
        paused_ = false;
        SignalReportState(State::CONTINUED, "");
    }
}
//------------------------------------------------------------------------------
void __fastcall TransferManagerT::Impl::Execute()
{
    //LOG_FOO_LIFETIME_QUOTES;
    goingStop_ = 0;
    isFinalScripPerforming_ = 0;
    SignalReportState( State::START, "Опрос: старт" );
    
    if(beginTask_) try
    {
        MyWCout( "Инициализация\n", MY_SET_CONSOLE_AQUA_TEXT );
        beginTask_->Perform();
        MyWCout( "Окончание инициализации\n", MY_SET_CONSOLE_AQUA_TEXT );
    }
    catch(...)
    {
        MyWCout( "Инициализация прервана с ошибкой\n", MY_SET_CONSOLE_AQUA_TEXT );
        lastExcpt_ = MY_RETHROW_;
        HandleCurentException();
        goingStop_ = 1;
    }


	for
    (
    	PTransfer curentTask = PrepareCurentTask();
        !thr_->IsTerminated() && !goingStop_;
        curentTask = PrepareCurentTask()
    )
    {
        #define CHECK_STOP_THREAD_ if( IsThreadStopedOrTerminated() || goingStop_) break;

        CHECK_STOP_THREAD_;
    	try
    	{
    	    if(curentTask.get()==NULL)
            {
            	if( taskList_.IsEmpty() )
                    break;
                else
                    continue;
            }
            //обработка события начала трансфера
    		if( my::SynchronizeMthd( &HandleBeginTransfer, __FILE_LINE__ )==false )
            {
            	taskList_.PopFrontBody();
                continue;
            }
            CHECK_STOP_THREAD_;
            // выполнить трансфер
            curentTask->Perform();
            CHECK_STOP_THREAD_;
            //обработка события окончания трансфера
            if( my::SynchronizeMthd( &HandleEndTransfer, __FILE_LINE__ )==true )
            	taskList_.PushBackBody( curentTask ) ;
            taskList_.PopFrontBody();
            CHECK_STOP_THREAD_;
            if(paused_)
                this->SyncronizedSleep(1);

        }// try
        catch(const TerminateFromThreadSignal&)
        {
            MyWCout("Опрос прерван из потоковой продцедуры.\n");
            break;
        }
   	 	catch(...)
    	{
        	lastExcpt_ = MY_RETHROW_;
            HandleCurentException();
            CHECK_STOP_THREAD_;
            SyncronizedSleep(10);
    	}
        #undef CHECK_STOP_THREAD_
    } // for( PTaskData curentTask = taskList_.Front();
    HideTopPanel();

    isFinalScripPerforming_ = 1;
    ShowTopPanel("Завершение", false);

    if(finalTask_) try
    {
        MyWCout( "Завершение\n", MY_SET_CONSOLE_AQUA_TEXT );
        finalTask_->Perform();
        MyWCout( "Окончание завершения\n", MY_SET_CONSOLE_AQUA_TEXT );
    }
    catch(...){}
    
    finalTask_.reset();
    
    isFinalScripPerforming_ = 0;

    HideTopPanel();
    SignalReportState( State::STOP, "Опрос остановлен" );
}
//------------------------------------------------------------------------------
// реакция на событие "начало трансфера"
LRESULT TransferManagerT::Impl::HandleBeginTransfer()
{
	if( thr_->IsTerminated() || taskList_.IsEmpty() ) return 0;
	const PTransfer curentTask = taskList_.FrontBody();
    assert(curentTask);
    const LRESULT ret = static_cast<LRESULT>( curentTask->HandleBeginTransfer() );
    if(ret!=0)
    	SignalReportState(State::BEG, curentTask->What() );
	return ret;
}
//------------------------------------------------------------------------------
// реакция на событие "окончание трансфера"
LRESULT TransferManagerT::Impl::HandleEndTransfer()
{
	if( thr_->IsTerminated() || taskList_.IsEmpty() ) return 0;
	const PTransfer curentTask = taskList_.FrontBody();
    if( curentTask.get() == NULL ) return 0;
    SignalReportState( State::END, curentTask->What() );
    lastExcpt_.reset();
    HideTopPanel();
    return static_cast<LRESULT>( curentTask->HandleEndTransfer() );
}
//------------------------------------------------------------------------------
LRESULT TransferManagerT::Impl::HandleCurentException()
{
	if( thr_->IsTerminated() || !lastExcpt_.get() ) return 0;
    this->HideTopPanel();
    const AnsiString msg = FormatMyExceptionMessages( lastExcpt_.get(), "\n" );
    MyWCout( FormatMyExceptionMessages( lastExcpt_.get(), "\n\t",
        "%eMsg %eLine %eFile")+"\n", MY_SET_CONSOLE_RED_TEXT );
    SignalReportState(State::ERR, lastExcpt_->Message() );
    return 0;
}
//------------------------------------------------------------------------------
//подготовить задание из списка - вызывать из потока
PTransfer TransferManagerT::Impl::PrepareCurentTask()
{
    bool x = taskList_.IsEmpty();
	assert(thr_);
    if( IsThreadStopedOrTerminated() ) return PTransfer();
    taskList_.Merge();
    const bool repeatScriptEmpty = repeatScript_.empty();
    if( taskList_.IsEmpty() && !repeatScriptEmpty )
    {
        taskList_.SetList( repeatScript_ );
        SignalReportState( State::START, "Опрос: старт" );
    }
    PTransfer ret  = taskList_.FrontBody();
    return ret;
}
//------------------------------------------------------------------------------
void TransferManagerT::Impl::AddTask( TVoidMthd mthd, const AnsiString& what, bool addTail )
{
    class SingleAction : public NamedTransfer
    {
    public:
        explicit SingleAction(const AnsiString& what, TVoidMthd mthd)
            :
            NamedTransfer(what), mthd_(mthd)
        {}
        virtual ~SingleAction() {}
    private:
        const TVoidMthd mthd_;
        virtual bool DoHandleBeginTransfer() { return true; }
        virtual bool DoHandleEndTransfer() { return false; }
        virtual void PerformAction()
        {
            mthd_();
        }
    };

    AddTask( new SingleAction( what, mthd ), addTail );
}


/////////////////////////////////////////////////////////////////////////////////
//					class TransferManagerT
/////////////////////////////////////////////////////////////////////////////////
TransferManagerT::TransferManagerT() : impl_(new Impl)
{
}
//------------------------------------------------------------------------------
boost::shared_ptr<void> TransferManagerT::ConnectOnReportState(TVoid_IntStrMthd method)
{
	return impl_->ReportStateConnections().AutoConnect(method);
}
//------------------------------------------------------------------------------
// упрвление списком - добавить задание
void TransferManagerT::AddTask( MyTransferTask *trnsfr, bool addTail)
{
    impl_->AddTask(trnsfr, addTail);
}
void TransferManagerT::AddTask( TVoidMthd mthd, const AnsiString& what,  bool addTail )
{
    impl_->AddTask(mthd, what, addTail);
}
void TransferManagerT::SetFinalTask( PTransfer task)
{
    impl_->SetFinalTask( task);
}

void TransferManagerT::SetBeginTask( PTransfer task)
{
    impl_->SetBeginTask( task);
}

//---------------------------------------------------------------------------
void TransferManagerT::ShowTopPanel(const AnsiString& s, bool showProgress)
{
    impl_->ShowTopPanel(s, showProgress);
}
//------------------------------------------------------------------------------
void TransferManagerT::HideTopPanel()
{
    impl_->HideTopPanel();
}
void TransferManagerT::SetTopPanelProgress( unsigned v, unsigned vMax, unsigned vMin )
{
    impl_->SetTopPanelProgress( v, vMax, vMin );
}

void TransferManagerT::SetEnableTopPanelTimeProgress( bool eneble )
{
    impl_->SetEnableTopPanelTimeProgress( eneble );
}

void TransferManagerT::SetTopPanelCaption( const AnsiString& s )
{
    impl_->SetTopPanelCaption( s );
}

// управление потоком

void TransferManagerT::StartThread() { impl_->StartThread(); }
void TransferManagerT::StopThread() {  impl_->StopThread();  }

void TransferManagerT::SetPause(bool isPause) { impl_->SetPause(isPause); }
bool TransferManagerT::IsPaused() const { return impl_->IsPaused(); }

bool TransferManagerT::IsThreadStopedOrTerminated() const {	return impl_->IsThreadStopedOrTerminated(); }
const MyException* TransferManagerT::GetLastException() const { return impl_->GetLastException(); }

bool TransferManagerT::IsFinalScripPerforming() const { return impl_->IsFinalScripPerforming(); }

void TransferManagerT::SetRepeatScript(const Scenary& scn)
{
    impl_->SetRepeatScript(scn);
}

void TransferManagerT::ResetScripts()
{
    impl_->ResetScripts();
}

//---------------------------------------------------------------------------
/**
*       список трансферов
*/
TaskList& TransferManagerT::GetTaskList()
{
    return impl_->GetTaskList();
}
//------------------------------------------------------------------------------
void TransferManagerT::SyncronizedSleep( unsigned tm )
{
	impl_->SyncronizedSleep( tm );
}

TemporaryStopTransferManager::TemporaryStopTransferManager()
    : wasStarted_( !TransferManager::Instance().IsThreadStopedOrTerminated() )
{
    TransferManager::Instance().StopThread();
}
TemporaryStopTransferManager::~TemporaryStopTransferManager()
{
    if(wasStarted_)
        TransferManager::Instance().StartThread();
}
////////////////////////////////////////////////////////////////////////////////









