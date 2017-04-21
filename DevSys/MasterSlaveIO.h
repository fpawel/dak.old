//------------------------------------------------------------------------------
#ifndef MasterSlaveIOH
#define MasterSlaveIOH
//------------------------------------------------------------------------------
//std
#include <vector>

// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"

// my
#include "..\my_include\MyPort.hpp"
#include "MasterSlaveIOSettings.hpp"
#include "MyExcpt.hpp"
//------------------------------------------------------------------------------
class MasterSlaveIOImpl;

class MyTransferException : public MyException
{
public:
    MyTransferException(const FileLine& loc, const AnsiString& msg,
        const MasterSlaveIOImpl* io,
        const PMyExcpt nest = PMyExcpt() );
    const MasterSlaveIOImpl* IO() const { return io_; }
    unsigned char FirstByte() const;

private:
    const MasterSlaveIOImpl* io_;
};

class MyNoAnswerException : public MyTransferException
{
public:
    MyNoAnswerException(const FileLine& loc, const AnsiString& msg,
        const MasterSlaveIOImpl* io,
        const PMyExcpt nest = PMyExcpt() );
};
//------------------------------------------------------------------------------
class TransferManagerT;
//------------------------------------------------------------------------------
class MasterSlaveIOImpl : public boost::noncopyable
{
typedef void (__closure *TOnAnswer)(bool isAnswered);
public:
    explicit MasterSlaveIOImpl( const MasterSlaveIOSettings &sets, MyPort* port );
    ~MasterSlaveIOImpl();

    void Send( const char* sendBegin, const char* sendEnd, bool needAnswer );
    void SendWithTimout( const char* sendBegin, const char* sendEnd, unsigned timeOut );

    const char* TxD() const;
    const char* RxD() const;
    unsigned TxDSize() const;
    unsigned RxDSize() const;
    bool IsTransferManagerWasStopedOrTerminated() const;

    void SetOnAnswer( TOnAnswer onAnswer)
    {
        onAnswer_ = onAnswer;
    }

    MasterSlaveIOSettings Sets()
    {
        return sets_;
    }

private:
    const unsigned mainThreadId_;
	TransferManagerT& tmngr_;
    MyPort* port_;
    std::vector<char> rxd_, txd_;
    TOnAnswer onAnswer_;

    boost::shared_ptr<void> connectOnTransferManagerReport_;

    const MasterSlaveIOSettings& sets_;

    void OpenPort();
    void ClosePort();

    void OnTransferManagerReport(unsigned context, const AnsiString& state);
    void DoBeforSendTxD();
    void SendTxD();
    void SendTxDAndGetAnswer(unsigned timeOut = -1);
    void LogOut( const AnsiString& );
};
//------------------------------------------------------------------------------
class RS485Adapter : public boost::noncopyable
{

public:
    struct State { enum { START, END, NO_ANSWER }; };

    explicit RS485Adapter(const MasterSlaveIOSettings &sets, MyPort *port );
    virtual ~RS485Adapter() {}
    // Передать txd, считать данные приёмника в rxd,
    // вернуть GetCRC16(rxd)==0
    void PerformTransfer( unsigned addy, unsigned commandCode,
        const unsigned char* sendBegin, const unsigned char* sendEnd, unsigned timeOut=-1 );

    virtual const unsigned char* SentData() const = 0;
    virtual unsigned SentDataSize() const = 0;
    virtual const unsigned char* AcceptedData() const = 0;
    virtual unsigned AcceptedDataSize() const = 0;

    virtual unsigned Addy() const = 0;
    virtual unsigned CommandCode() const = 0;

    virtual void PrepareTxD( std::vector<unsigned char>& txd, unsigned addy,
        unsigned cmdCode, const unsigned char* sendBegin, const unsigned char* sendEnd) = 0;

    typedef LRESULT (__closure *TOnNotifyState )(const RS485Adapter* thiS, unsigned state);
    void SetNotifyState( TOnNotifyState );

    bool IsNotAnsweredAddy(unsigned addy) const ;

    const MasterSlaveIOImpl& MasterSlave() const;

private:

    boost::shared_ptr<MasterSlaveIOImpl> masterSlaveIO_;
    TOnNotifyState notifyState_;
    TransferManagerT& tmngr_;
    boost::shared_ptr<void> connectOnTransferManagerReport_;
    bool notAnswered_[256], answered_[256];

    void OnTransferManagerReport(unsigned context, const AnsiString& state);
    void SetAddy(unsigned char addy, bool isAnswered);

    void OnAnswer(bool isAnswer);
    void SetSignalState(unsigned);

    virtual bool TestAnswer(const unsigned char* reciveBegin,
        const unsigned char* reciveEnd ) = 0;
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------

