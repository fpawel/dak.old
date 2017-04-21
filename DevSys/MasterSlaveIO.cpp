//---------------------------------------------------------------------------
#include <windows.h>

//std
#include <sstream>

#pragma hdrstop
//------------------------------------------------------------------------------
#include "MasterSlaveIO.h"
//------------------------------------------------------------------------------
// loki
#include "loki\threads.h"
//------------------------------------------------------------------------------
#include <forms.hpp>
//------------------------------------------------------------------------------

#include "AnsiStringUtils_.h"
#include "guicon.h"
#include "TransferManage.h"
#include "MySynchronize.h"

//------------------------------------------------------------------------------
using std::cout;
using std::endl;
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// вывести массив в cout
AnsiString PrintVInt8( const std::vector<char>& v, int lim = -1 )
{
    const unsigned maxIdx = (lim==-1) ? v.size() : std::min(lim, v.size() );
    return MyBuffToStr( v.begin(), v.begin() + maxIdx );
}
//------------------------------------------------------------------------------
void PerformDelay(TBoolMthd mthd, unsigned tm)
{
	const unsigned tmStart = GetTickCount();
    while( (GetTickCount()-tmStart < tm) && !mthd() )
        ::Sleep(1);
}
//---------------------------------------------------------------------------
MasterSlaveIOImpl::MasterSlaveIOImpl(const MasterSlaveIOSettings &sets, MyPort* port) :
    mainThreadId_( ::GetCurrentThreadId() ),
    tmngr_( TransferManager::Instance() ),
    onAnswer_(NULL),
    port_(port), sets_(sets),
    connectOnTransferManagerReport_
    	( tmngr_.ConnectOnReportState(OnTransferManagerReport) )
{
}
//------------------------------------------------------------------------------
MasterSlaveIOImpl::~MasterSlaveIOImpl()
{
}
//------------------------------------------------------------------------------
void MasterSlaveIOImpl::OnTransferManagerReport(unsigned context, const AnsiString& state)
{
    if(context==TransferManagerT::State::STOP)
        port_->Disconnect();
}
//------------------------------------------------------------------------------
bool MasterSlaveIOImpl::IsTransferManagerWasStopedOrTerminated() const
{
    return ::GetCurrentThreadId()!=mainThreadId_ && tmngr_.IsThreadStopedOrTerminated() &&
        !tmngr_.IsFinalScripPerforming();

}
//------------------------------------------------------------------------------
void MasterSlaveIOImpl::LogOut( const AnsiString& s)
{
	if( !sets_.mustLogData_ ) return;
    MyWCout( s );
}
//------------------------------------------------------------------------------
// Передать txd, считать данные приёмника в rxd, вернуть длительность трансфера
void MasterSlaveIOImpl::Send(	const char* sendBegin, const char* sendEnd, bool needAnswer )
{
    if( IsTransferManagerWasStopedOrTerminated() )
    {
        MyWCout( "Отмена трансфера! Поток остановлен!\n" );
        return;
    }
    txd_.assign( sendBegin, sendEnd );
    DoBeforSendTxD();
    if(!needAnswer)
        SendTxD();
    else
        SendTxDAndGetAnswer();
}
//------------------------------------------------------------------------------
void MasterSlaveIOImpl::SendWithTimout( const char* sendBegin, const char* sendEnd, unsigned timeOut )
{
    if( IsTransferManagerWasStopedOrTerminated() )
    {
        MyWCout( "Отмена трансфера! Поток остановлен!\n" );
        return;
    }
    txd_.assign( sendBegin, sendEnd );
    DoBeforSendTxD();
    SendTxDAndGetAnswer(timeOut);
}
//------------------------------------------------------------------------------
void MasterSlaveIOImpl::DoBeforSendTxD()
{
    if( ::GetCurrentThreadId()!=mainThreadId_ )
        tmngr_.SyncronizedSleep( sets_.tmWriteDelay_ );
    else
        ::Sleep(sets_.tmWriteDelay_);
    OpenPort();
}
//------------------------------------------------------------------------------
void MasterSlaveIOImpl::SendTxD()
{
    // очистка буфферов
    port_->Purge();
    rxd_.clear();
    // вывести txd в cout
    LogOut( "txd:\n"+ PrintVInt8( txd_ )+" " );
    // отправить запрос
    const unsigned txdSz = txd_.size();
    assert(txdSz>0);
    void *pTxdBuf = reinterpret_cast<void*>( const_cast<char*>(&txd_.at(0)) );
    port_->Write( pTxdBuf, txdSz );
}
//------------------------------------------------------------------------------
void MasterSlaveIOImpl::SendTxDAndGetAnswer(unsigned timeOutArg)
{
    const unsigned timeOut = timeOutArg==-1 ? sets_.timeOut_ : timeOutArg;

    beginProcedureLable:
    SendTxD();
    // запомнить момент начала считывания из RxD
    unsigned tmStart = GetTickCount();
    while( !IsTransferManagerWasStopedOrTerminated() &&
        (GetTickCount() - tmStart < timeOut) )
    {
		// определим количество байтов в буффере порта
        const int rxdSize = port_->GetRxDSize();
        // если RxD пуст, продолжаем мониторинг
        if( rxdSize==0 )
        {
            ::Sleep(1);
            continue;
        }
        // в приёмнике появились данные. Считываем
        const unsigned oldReadBuffSize = rxd_.size();
        rxd_.resize(oldReadBuffSize + rxdSize);
        void *pReadBuff = reinterpret_cast<void*>( &rxd_.at(oldReadBuffSize) );
        port_->Read( pReadBuff, rxdSize );
        //LogOut( "часть: "+ MyBuffToStr( (const char *)pReadBuff, (const char *)pReadBuff+rxdSize )+"\n");
        // ждём ещё некоторое время silentTime_ и проверяем длину RxD.
        // Если не 0, продолжаем считывание
        // иначе считаем что посылка завершена
        ::Sleep(sets_.silentTime_);
        if( port_->GetRxDSize()==0 ) break;
        tmStart = ::GetTickCount();
    }
    LogOut( MYSPRINTF_("%d мсек", GetTickCount() - tmStart)  );
    const bool timeOutFixed = (GetTickCount() - tmStart >= timeOut);

    static unsigned timeOutCount = 0;
    if( timeOutFixed )
    {
    	++timeOutCount;
        LogOut( MYSPRINTF_(" Таймаут %d.", timeOutCount ));
        if(timeOutCount<sets_.repeatCount_)
        {
            LogOut( MYSPRINTF_(" Повтор №%d из %d.\n", timeOutCount, sets_.repeatCount_ ));
        	goto beginProcedureLable;
        }
    }
    timeOutCount = 0;
    if( !rxd_.empty() )
    {
    	LogOut( "\nrxd:\n"+ PrintVInt8( rxd_ )+"\n" );
        
    }
    else
    {
    	LogOut( " Нет ответа!\n" );

        if(onAnswer_) onAnswer_(false);
        throw PMyExcpt( new MyNoAnswerException(
            __FILE_LINE__, port_->Description()+" не отвечает" , this ) );
    }
    if(onAnswer_) onAnswer_(true);
}
//------------------------------------------------------------------------------
// открыть канал связи с текущими настройками
void MasterSlaveIOImpl::OpenPort()
{
    assert(port_);
	if( port_->IsConnected() ) return;
    try
    {
    	port_->Connect();
        MYCOUT_( "Соединение с \"%s\" установлено\n", port_->Description() ));
    }
    catch(...)
    {
    	// подождать секунду, чтобы постоянно не дёргать нерабочий порт
    	Sleep(1000);
        throw;
    }
}
//------------------------------------------------------------------------------
void MasterSlaveIOImpl::ClosePort()
{
	if( !port_->IsConnected() ) return;
    port_->Purge();
    port_->Disconnect();
    MYCOUT_( "Соединение с \"%s\" закрыто.\n", port_->Description() ));
}
//------------------------------------------------------------------------------
const char* MasterSlaveIOImpl::TxD() const
{
    return txd_.empty() ? NULL : &txd_.at(0);
}
//------------------------------------------------------------------------------
const char* MasterSlaveIOImpl::RxD() const
{
    return rxd_.empty() ? NULL : &rxd_.at(0);
}
//------------------------------------------------------------------------------
unsigned MasterSlaveIOImpl::TxDSize() const { return txd_.size(); }
unsigned MasterSlaveIOImpl::RxDSize() const { return rxd_.size(); }
//------------------------------------------------------------------------------
MyTransferException::MyTransferException(const FileLine& loc, const AnsiString& msg,
    const MasterSlaveIOImpl* io, const PMyExcpt nest ) : MyException( loc, msg, nest ), io_(io)
{}

MyNoAnswerException::MyNoAnswerException(const FileLine& loc, const AnsiString& msg,
    const MasterSlaveIOImpl* io,
    const PMyExcpt nest ) : MyTransferException( loc, msg, io, nest )
{}
//------------------------------------------------------------------------------
unsigned char MyTransferException::FirstByte() const
{
    return io_->TxDSize() ? io_->TxD()[0] : 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void RS485Adapter::SetAddy(unsigned char addy, bool isAnswered)
{
    notAnswered_[addy] = !isAnswered;
    answered_[addy] = isAnswered;
}
//------------------------------------------------------------------------------
bool RS485Adapter::IsNotAnsweredAddy(unsigned addy) const { return notAnswered_[addy];  }
//------------------------------------------------------------------------------
void RS485Adapter::OnTransferManagerReport(unsigned context, const AnsiString& state)
{
    if(context==TransferManagerT::State::START)
    {
        std::fill( notAnswered_, notAnswered_+256, false );
        std::fill( answered_, answered_+256, false );
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
RS485Adapter::RS485Adapter(const MasterSlaveIOSettings &sets, MyPort *port ) :
    masterSlaveIO_( new MasterSlaveIOImpl(sets, port) ), notifyState_(NULL),
    tmngr_( TransferManager::Instance() ),
    connectOnTransferManagerReport_
        ( tmngr_.ConnectOnReportState(OnTransferManagerReport) )
{
    masterSlaveIO_->SetOnAnswer( &OnAnswer );
}
//------------------------------------------------------------------------------
const MasterSlaveIOImpl& RS485Adapter::MasterSlave() const
{
    return *masterSlaveIO_.get();
}
//------------------------------------------------------------------------------
void RS485Adapter::SetSignalState(unsigned state)
{
    if( notifyState_ && !masterSlaveIO_->IsTransferManagerWasStopedOrTerminated() )
        my::Synchronize2( notifyState_, this, state, __FILE_LINE__ );
}
//------------------------------------------------------------------------------
void RS485Adapter::OnAnswer(bool isAnswer)
{
    if(!isAnswer && !tmngr_.IsThreadStopedOrTerminated() )
    {
        SetAddy( this->Addy(), false );
        SetSignalState(State::NO_ANSWER);
    }
}
//------------------------------------------------------------------------------
void RS485Adapter::SetNotifyState( TOnNotifyState foo )
{
    notifyState_ = foo;
}
//------------------------------------------------------------------------------
void RS485Adapter::PerformTransfer( unsigned addy, unsigned commandCode,
    const unsigned char* sendBegin, const unsigned char* sendEnd, unsigned timeOut )
{
    int repeatCount = masterSlaveIO_->Sets().repeatCount_;
    lx:
    SetSignalState(State::START);

    std::vector<unsigned char> txd;

    PrepareTxD(txd, addy, commandCode, sendBegin, sendEnd);

    const bool needAnswer = addy!=0;
    if(!needAnswer)
        masterSlaveIO_->Send( txd.begin(), txd.end(), false );
    else
        masterSlaveIO_->SendWithTimout(txd.begin(), txd.end(), timeOut);

    if( !needAnswer || masterSlaveIO_->IsTransferManagerWasStopedOrTerminated()  ) return;
    const unsigned rxdSize = masterSlaveIO_->RxDSize();
    const unsigned char *rxd = masterSlaveIO_->RxD(), *rxdEnd = rxd + rxdSize;

    // проверка корректности rxd_ - возбуждаются исключения
    if ( TestAnswer(rxd, rxdEnd) &&  (repeatCount>0) ) {
       --repeatCount;
       goto lx;
    }
    SetAddy( this->Addy(), true );
    SetSignalState(State::END);
}
//------------------------------------------------------------------------------





