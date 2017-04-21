//---------------------------------------------------------------------------


#pragma hdrstop

#include <dateutils.hpp>

#include "HandleTransfer.h"
#include "Unit1.h"
#include "MyExcpt.hpp"
#include "TaskList.h"
#include "SGrdHlpr.h"
#include "listViewHelpr_.h"
#include "AnsiStringUtils_.h"
#include "guicon.h"
#include "TransferManage.h"
#include "MasterSlaveIO.h"
#include "ctrlsys.h"
#include "task_.h"
#include "ModbusAdapter.h"
#include "Devices.h"
#include "MyModFN.hpp"
#include "MyRichEdit.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//------------------------------------------------------------------------------
void ShowScenaryList(const Scenary& lst, TStringGrid* grd)
{
	my::Grd::DeleteRow(grd, 1, grd->RowCount-1);
    grd->RowCount = lst.size() + 1;
    if( !lst.empty() )
        grd->FixedCols = 1;
    grd->ColWidths[0] = 25;

    grd->FixedCols = 1;
    if( grd->RowCount>1 ) grd->FixedRows = 1;

    unsigned n=0;
    for( unsigned i=0; i<lst.size(); ++i )
	{
        const AnsiString s = lst[i]->What();
        const bool vis = !s.IsEmpty();
        my::Grd::SetRowVis( grd, i+1, vis);
        n += vis;
    	grd->Cells[0][i+1] = i+1;
        grd->Cells[3][i+1] = s;
    }
}
//------------------------------------------------------------------------------
void TForm1::HandleException()
{
    scenaryErrorFixed_ = 1;
    const MyException *excpt = tmngr_.GetLastException();
    const AnsiString errSrc = ctrlSys_.GetIOSets().lastErrorSource;
    const bool
        isDeviceNoAnswer = excpt->TypeOf<MyNoAnswerException>() && errSrc=="прибор",
        mustContinueThread = isDeviceNoAnswer;

    AnsiString msg = excpt->Message();
    if( !errSrc.IsEmpty() )
        msg = errSrc+": "+msg;

    //if( isDeviceNoAnswer ) SetNotAnswered( ctrlSys_.GetModbus().Addy() );

    const MyTransferTask* task = tskLst_.GetCurrentTask();
    const int taskIdx = GetScenaryTaskIndex( scenary_, task );
    pnStatus_[1]->Font->Color = clRed;
    pnStatus_[1]->Font->Style = TFontStyles() << fsBold;

    if(taskIdx>-1)
    {
    	grdScnList->Cells[4][taskIdx+1] = "Ошибка. "+msg;
        my::Grd::RedrawRow(grdScnList, taskIdx+1 );
    }

    if( errSrc.Pos("прибор") )
        LogError( msg ) ;
    else
        AddLogAll( msg, RichTextAttr().TextColor(clRed).Time() );

    if( !mustContinueThread )
        tmngr_.StopThread();
}
//------------------------------------------------------------------------------
void TForm1::HandleBeginOrEndTransfer(const AnsiString& state, bool isBeginTransfer)
{
    const MyTransferTask* task = tskLst_.GetCurrentTask();
    const int taskIdx = GetScenaryTaskIndex( scenary_, task );

	if ( task!=NULL && taskIdx>-1 )
    {
        my::Grd::RedrawRect( grdScnList, TRect(0,taskIdx+1,3,taskIdx+1) );
        my::Grd::RedrawRow( grdVars, 0 );
        lvDevs->Repaint();
        if(isBeginTransfer)
        {
        	grdScnList->Cells[1][taskIdx+1] = TimeToStr( Now() );
            grdScnList->Cells[2][taskIdx+1] = "00:00:00";
            grdScnList->Cells[4][taskIdx+1] = "Выполняется";
        }
        else
        {
            if( grdScnList->Cells[4][taskIdx+1]!="Прервано" )
            {
                grdScnList->Cells[4][taskIdx+1] = "Выполнено";
            }
        }
    }

    if( !isBeginTransfer && !IsScenaryErrorRow( grdScnList, taskIdx+1 ) )
    {
        pnStatus_[1]->Font->Color = clBlack;
    	pnStatus_[1]->Font->Style = TFontStyles();
    }
    my::Grd::RedrawRow(grdScnList,taskIdx+1 );
}
//------------------------------------------------------------------------------
void TForm1::HandleStopOrStartThread(const AnsiString& state, bool isStartThread)
{
    static bool wasStarted = 0;
	SetStatus(state,0);
    for( unsigned i=1; i<STATUS_PNLS_COUNT; ++i ) SetStatus("",i);

    btnRun->Hint = isStartThread ? "Разъединить" : "Соединится";
    //SetStatus("",1);
    //btnRun->Visible = !isStartThread;
    imgStopEnabled->Visible = isStartThread;
    imgStopDisabled->Visible = !isStartThread;
    btnStop->Enabled = isStartThread;
    imgPause->Visible = isStartThread;
    imgCicleRun->Visible = !isStartThread;
    imgSkipEnabled->Visible = isStartThread;
    imgSkipDisabled->Visible = !isStartThread;
    btnSkip->Enabled = isStartThread;

    cbCurentScenary->Enabled = !isStartThread;
    const AnsiString scnName = MyAnsiStringEraseNotValidFileNameSymbols(cbCurentScenary->Text);
    RichTextAttr reTxtAtrs = RichTextAttr().TextColor(clBlue). FontStyles( TFontStyles() << fsBold
        << fsUnderline ).TextSize(14).Time();
    if( !isStartThread )
    {
        if(wasStarted && !scnName.IsEmpty() )
        {
            AddLogAll( "Сценарий завершён ", reTxtAtrs );         

        }

        const MyTransferTask* task = tskLst_.GetCurrentTask();
        const int taskIdx = GetScenaryTaskIndex( scenary_, task );
        if ( task!=NULL && taskIdx>-1 && grdScnList->Cells[4][taskIdx+1]=="Выполняется")
        {
            grdScnList->Cells[4][taskIdx+1] = "Прервано";
            AddLogAll( "Прервано!", reTxtAtrs.TextColor(clRed) );
        }
        wasStarted = 0;

    } else {
        wasStarted = 1;
        scenaryErrorFixed_ = 0;
        if(!scnName.IsEmpty())
            MyCoutTime( scnName, MY_SET_CONSOLE_YELLOW_TEXT );

        scenary_ = tskLst_.GetList();
        ShowScenaryList(scenary_, grdScnList);
        AddLogAll( cbCurentScenary->Text, reTxtAtrs );
    }
}
//------------------------------------------------------------------------------
void TForm1::OnTransferManagerReport(unsigned context, const AnsiString& state)
{
    const bool
        isStartThread    	= context==TransferManagerT::State::START,
        isStopThread        = context==TransferManagerT::State::STOP,
    	isBeginTransfer 	= context==TransferManagerT::State::BEG,
        isEndTransfer 		= context==TransferManagerT::State::END,
        isError             = context==TransferManagerT::State::ERR
    ;

    if ( isBeginTransfer || isEndTransfer )
    	HandleBeginOrEndTransfer(state, isBeginTransfer);
    else if( isStartThread || isStopThread )
    {
        HandleStopOrStartThread(state, isStartThread);
        return;
    } else if( isError )
        HandleException();

    if( isEndTransfer || isError )
        SetStatus( state, 1 );
}
//------------------------------------------------------------------------------
void TForm1::FixScripResultForAddy(unsigned addy, const AnsiString& s, bool failed)
{
    const MyTransferTask* task = tskLst_.GetCurrentTask();
    const int taskIdx = GetScenaryTaskIndex( scenary_, task );
    if ( taskIdx==-1 ) return;
    const int addyIdx = Devs::IndexOfAddy(addy);
    if(addyIdx>-1)
    {
        const int col = 5+addyIdx, row = taskIdx+1;
        grdScnList->Cells[col][row] = s;
    }
}
//------------------------------------------------------------------------------
