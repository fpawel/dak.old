//------------------------------------------------------------------------------
#ifndef TransferManageH
#define TransferManageH
//------------------------------------------------------------------------------
// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
//------------------------------------------------------------------------------
// my
#include "MyAbstructTransfer.hpp"
#include "..\my_include\free_types_.h"
#include "..\my_include\MyNoDestroySingleton.hpp"
#include "TransferSignal.hpp"
#include "MyExcpt.hpp"
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// тип формы диалога настроек
class TFormCtrlPrgrmmDlg;
class TFrmWait;
typedef boost::shared_ptr<TFormCtrlPrgrmmDlg> PFormCtrlPrgrmmDlg;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// список трансферов
class TaskList;


class TransferManagerT : public boost::noncopyable
{
public:
    enum{ ADD_TASK_HEAD = false, ADD_TASK_TAIL = true };

    // упрвление списком - добавить задание
	void AddTask( MyTransferTask *trnsfr, bool addTail = ADD_TASK_TAIL );
    void AddTask( TVoidMthd mthd, const AnsiString& what = "",  bool addTail = ADD_TASK_TAIL );

    void SetFinalTask( PTransfer );
    void SetBeginTask(PTransfer);
    void SetRepeatScript(const Scenary& scn);
    void ResetScripts();


	// управление потоком
    void StartThread();
	void StopThread();

    void SetPause(bool isPause);
    bool IsPaused() const;

    bool IsThreadStopedOrTerminated() const;
    bool IsFinalScripPerforming() const;
    const MyException* GetLastException() const;

    struct State  { enum{ BEG, END, ERR, START, STOP, PAUSED, CONTINUED}; };

	boost::shared_ptr<void> ConnectOnReportState( TVoid_IntStrMthd method );
    /**
    *       список трансферов
    */
    TaskList& GetTaskList();

    void SyncronizedSleep( unsigned tm );
    explicit TransferManagerT();
                            
    void ShowTopPanel(const AnsiString&, bool showProgress=false);
    void HideTopPanel();
    void SetTopPanelProgress( unsigned v, unsigned vMax=-1, unsigned vMin=-1 );
    void SetEnableTopPanelTimeProgress( bool eneble );
    void SetTopPanelCaption( const AnsiString& s );

    TFrmWait* GetTopPanel();
private:
	class Impl;
    boost::shared_ptr<Impl> impl_;
};

class TemporaryStopTransferManager : public boost::noncopyable
{
public:
    explicit TemporaryStopTransferManager();
    ~TemporaryStopTransferManager();
private:
    bool wasStarted_;
};

//------------------------------------------------------------------------------
struct TransferManagerTagT {};
typedef MyNoDestroySingletonHolder<TransferManagerT,TransferManagerTagT> TransferManager;
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
