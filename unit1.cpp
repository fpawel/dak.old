//------------------------------------------------------------------------------
//#include      <ComObj.hpp>
//#include      <utilcls.h>
//------------------------------------------------------------------------------
#include <vcl.h>
//------------------------------------------------------------------------------
#include <fstream>
#include <Algorithm>
#include <math>
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "Unit1.h"
//------------------------------------------------------------------------------
#include <math.hpp>
#include <DateUtils.hpp>
//------------------------------------------------------------------------------
#include "boost\any.hpp"
//------------------------------------------------------------------------------
#include "Myformat.hpp"
#include "TransferManage.h"
#include "FooCalledOnce.hpp"
#include "MyModFN.hpp"
#include "MyIostream.h"
#include "MyProcNfo.hpp"
#include "guicon.h"
#include "AnsiStringUtils_.h"
#include "iniUtils_.h"
#include "FreeAndNuLL.hpp"
#include "MyVclUtils_.h"
#include "bcd_.h"
#include "TaskList.h"
#include "CRC16.h"
#include "ctrlsys.h"
#include "IOProtocols.hpp"
#include "SGrdHlpr.h"
#include "uFrmWait.h"
#include "winrc.h"
#include "MyFileCtrl.h"
#include "task_.h"
#include "uAppSetsDlg.h"
#include "alchemy.h"
#include "MyVclUtils_.h"
#include "pneumo.hpp"
#include "listViewHelpr_.h"
#include "Scenaries.h"
#include "MySynchronize.h"
#include "uFormMdiChild.h"
#include "my_array_size.h"
#include "modbusvars.hpp"
#include "MyStrValRef.hpp"
#include "tinyxml.h"
#include "xmlUtils_.h"
#include "ModbusAdapter.h"
#include "WinErr.hpp"
#include "Devices.h"
#include "MyStrUtils.h"
#include "MyAbstructTransfer.hpp"
#include "AppXML.h"
#include "rtflib\rtflib.h"
#include "rtflib\globals.h"
#include "scriptTask.h"
#include "uFrameGrid.h"
#include "uFrameDevicePasport.h"
#include "pasport.h"
#include "HeatChamber.h"



//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ThunderbirdTreeMain"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------
#define SCENARY_FILE_NAME "scns.xml"
//------------------------------------------------------------------------------
void SetErrorStyleCell( TCanvas& cnv )
{
	cnv.Font->Color = clYellow;
    //cnv.Font->Style = cnv.Font->Style + (TFontStyles() << fsBold);
    cnv.Brush->Color = clBlack;
}
//------------------------------------------------------------------------------
bool IsCellContains( TStringGrid* grd, int col, int row, const AnsiString& s )
{
    return grd->Cells[col][row].LowerCase().Pos(s.LowerCase())!=0;
}

bool IsScenaryErrorRow( TStringGrid* grd, int row )
{
	for( int col=0; col<grd->ColCount; ++col )
    {
    	const AnsiString s = grd->Cells[col][row].LowerCase();
		if( s.Pos("ошибка")!=0 )
        	return true;
    }
    return false;
}
//------------------------------------------------------------------------------
TPoint GetCenterPosition( TRect rect, int w, int h )
{
    return
	    TPoint( rect.left+static_cast<int>( ( rect.Width() - w ) / 2),
			rect.Top+static_cast<int>( ( rect.Height() - h ) / 2) );
}
//------------------------------------------------------------------------------
void DrawFrameRect( TRect rect, TCanvas& cnv )
{
	rect = my::DecRect(rect,2);

    cnv.Pen->Color = clRed;
    cnv.Pen->Width = 1;
    TPenStyle penStl =cnv.Pen->Style;
    cnv.Pen->Style = psDot ;

    cnv.MoveTo(rect.Left,rect.Top);
    cnv.LineTo(rect.Right,rect.Top);
    cnv.LineTo(rect.Right,rect.Bottom);
    cnv.LineTo(rect.Left,rect.Bottom);
    cnv.LineTo(rect.Left,rect.Top);

    cnv.Pen->Style = penStl ;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AnsiString GetXMLEditorExeFileName()
{
    AnsiString fnExe;
    TiXMLAtr::Holder attr = TiXMLAtr::CreateHolderRef(
        TiXML::ChildElement( GetAppXML().config, "XML_редактор" ), "exe", fnExe );

    if( !MyFileExists(fnExe.c_str()) )
    {
        ForceCurrentDirectory = true;
        if( !PromptForFileName(fnExe, "XML-редактор|*.exe", "exe",
            "Выбрать XML-редактор", "", false) )
            return "";
    }
    return fnExe.c_str();
}
//------------------------------------------------------------------------------
TThunderbirdTree *CreateNewThunderbirdTree(TWinControl *parent)
{
    TThunderbirdTree *ret = new TThunderbirdTree(parent);
    ret->Parent = parent;
    ret->Align = alTop;
    ret->DoubleBuffered = true;
    ret->Height = 0;
    return ret;
}
//------------------------------------------------------------------------------
void CreateThunderbirdTreeSection(TThunderbirdTree* tree, TWinControl* ctrl, const AnsiString &name)
{
    TThunderbirdSection *sect = tree->AddSection(name);
    sect->Content->BevelInner = bvRaised;
    sect->Content->BevelOuter = bvLowered;
    tree->AttachControl( sect, ctrl);

}
//------------------------------------------------------------------------------
TForm1 *Form1;
//------------------------------------------------------------------------------

__fastcall TForm1::TForm1(TComponent* Owner)
	: lifeTime_(MakeMyLogQuote( "Form1_LIFE_TIME" ) ), TForm(Owner),
    tmngr_( TransferManager().Instance() ),
    tskLst_( tmngr_.GetTaskList() ),
    ctrlSys_( CtrlSys().Instance() ),
    drwGrdVars(grdVars), drwGrdScnList(grdScnList), drwGrdKfs(grdKfs),
    connectOnTransferManagerReport_( tmngr_.ConnectOnReportState(OnTransferManagerReport) ),
    form1Plcmnt_( TiXML::ChildElement( GetAppXML().windowsPlacements, "form1" ) ),
    consolePlcmnt_( TiXML::ChildElement( GetAppXML().windowsPlacements, "консоль" ) ),
    xmlTables_( TiXML::ChildElement( GetAppXML().form1, "таблицы" ) ),
    xmlReportFile_( TiXML::ChildElement( GetAppXML().config, "Протокол_настройки" ) )
{
	Caption = "Настройка ДАК. Версия 1.4.9";
    //::CoInitialize(NULL);
    // настройка панелей статуса
    pnStatus_[0] = Panel9;
    pnStatus_[1] = Panel10;
    pnStatus_[2] = Panel11;
    pnStatus_[3] = Panel12;
    
    drwGrdVars.SetAfterSetCanvasFoo( &AfterSetCanvasGrd );
    drwGrdVars.SetOnDeterminateColumnWidthFoo( &OnDeterminateColumnWidth );
    drwGrdVars.SetOnOnGetCellText( &OnGetIndicationCellText );

    drwGrdKfs.SetAfterSetCanvasFoo( &AfterSetCanvasGrd );
    drwGrdKfs.SetOnDeterminateColumnWidthFoo(&OnDeterminateColumnWidth);
    drwGrdKfs.SetOnOnGetCellText( &OnGetKefValCellText );

    drwGrdScnList.SetAfterSetCanvasFoo( &AfterSetCanvasGrd );
    drwGrdScnList.SetOnDeterminateColumnWidthFoo( &OnDeterminateColumnWidth );

    my::Grd::AddCopyItemToPopupMenu( grdVars );

    my::Grd::AddCopyItemToPopupMenu( grdKfs );
    my::Grd::AddPastItemToPopupMenu( grdKfs );
    my::Grd::AddSkipPastCol(grdKfs, 1);

    my::Grd::AddCopyItemToPopupMenu( grdScnList );


    scnLog->Lines->Clear();
    MakeLabelAsButton( btnPerformComand );

    CreateThunderbirdTreeSection(ThunderbirdTree2, PanelDevicesList, "Приборы");
    
    TStringGrid* grdPneumo = static_cast<TStringGrid*>(Pneumo::GetControl() );
    grdPneumo->Align = alNone;
    grdPneumo->Height = my::Grd::GetActualRowsTotalHeight(grdPneumo) + 50;
    CreateThunderbirdTreeSection(ThunderbirdTree2, grdPneumo, "Пневмоблок");

    CreateThunderbirdTreeSection(ThunderbirdTree2, panelHeatcham, "Термокамера");

    PopupMenuComand->Items->Clear();
    for(unsigned i=0; i<DAK::Cmd::Idx::count; ++i)
    {
        TMenuItem *menu = new TMenuItem(this);
        menu->Caption = DAK::Cmd::name[i];
        menu->OnClick = menuPerformComandClick;
        PopupMenuComand->Items->Add(menu);
    }


    MakeLabelsAsButton( btnHeatchamSetSetpoint, btnHeatchamSetSetpoint1 );
    MakeLabelAsButton( btnHeatchamStart );
    MakeLabelAsButton( btnHeatchamStop );
    MakeLabelAsButton( btnHeatchamFix );

    Devs::InitT initDevs;
    initDevs.grdVars = grdVars;
    initDevs.grdScnList = grdScnList;
    initDevs.lvDevs = lvDevs;
    initDevs.grdKfs = grdKfs;
    initDevs.pgCtRep = PageControlReport;
    initDevs.pgPasp = PageControlPasport;
    initDevs.pgChart = PageControlChart;

    Devs::Initialize(initDevs);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormActivate(TObject *Sender)
{
    OnActivate = NULL;
    ASSERT_FUNCTION_CALLED_ONCE__;
    ActiveControl = NULL;

	// перехват консоли на себя
    const HWND hCon = ::GetConsoleWindow();
    ::SetParent( hCon, Handle );
    ::ShowWindow( hCon, SW_HIDE );

    TiXML::LoadWindowPlacement( form1Plcmnt_, Handle );
	TiXML::LoadConsolePlacement( consolePlcmnt_ );

    Screen->MenuFont->Name = "Arial";
    Screen->MenuFont->Size = 11;

    Devs::LoadFile( ( MyGetExePath()+"devices.xml" ).c_str() );    

    for (int i = 0; i<ComponentCount; ++i)
  	{
    	TStringGrid *grd = dynamic_cast<TStringGrid*>(Components[i]);
        if(grd) TiXML::LoadGrd(xmlTables_, grd);
    }
    delete TabSheet2;

    cbCurentScenary->ItemIndex = TiXMLAtr::Get(GetAppXML().form1,
        "cbCurentScenary_ItemIndex", cbCurentScenary->ItemIndex );
    lvDevs->Column[1]->Width = TiXMLAtr::Get(GetAppXML().form1,
        "lvDevs_Column1", lvDevs->Column[1]->Width );

    const std::string scnFn =  MyGetExePath()+SCENARY_FILE_NAME;

    scenaries_ = MyFileExists(scnFn.c_str()) ?
        LoadScenaryFromXML( scnFn.c_str()  ) :
        CreateDefaultScenary();
    ShowCurrentScenary();

    cbCurentScenary->Width = cbCurentScenary->Parent->Width - 10 - cbCurentScenary->Left;

}
//------------------------------------------------------------------------------
__fastcall TForm1::~TForm1()
{
	ASSERT_FUNCTION_CALLED_ONCE__;
    LOG_FOO_LIFETIME_QUOTES;
    tmngr_.StopThread();

    Word year, month, day, hour, minute, second, milliSecond;
    DecodeDateTime( Now(), year, month, day, hour, minute, second, milliSecond);

    const AnsiString
        scnReportDir = MYSPRINTF_( "%s\\Отчёты\\%d\\%s\\", MyGetExePath().c_str(), year, MyFormatMonth(month) ),
            scnReportFileName = MYSPRINTF_( "%s%d_%d_%d_%d_%d.rtf",
            scnReportDir, day, hour, minute, second, milliSecond );
    SaveCurrentScenaryReport(scnReportFileName);

    SaveScenaryToXML( SCENARY_FILE_NAME, scenaries_ );
    Devs::SaveFile( ( MyGetExePath()+"devices.xml" ).c_str() );
    Pneumo::SaveConc();
    UnregisterAllLabelsAsButton(this);

    try
    {
        SaveIniData();
    }
    catch(...)
    {
        ::MessageBox(Handle, MYSPRINTF_("Ошибка записи в ini файл!\n%s",
            WindowsErrorCodeToStr( ::GetLastError() )).c_str(),
    		"form1.ini", MB_OK | MB_APPLMODAL | MB_ICONERROR );

        MyCout("Ошибка обращения к form1.ini!", MY_SET_CONSOLE_RED_TEXT);
    }
    Devs::Finalize();
}
//------------------------------------------------------------------------------
bool TForm1::MustSkipTask(const MyTransferTask* task)
{
    if( task != tskLst_.GetCurrentTask() ) return false;
    const int taskIdx = GetScenaryTaskIndex( scenary_, task );
    return taskIdx>-1 && taskIdx<grdScnList->RowCount-1 && !my::Grd::IsRowSelected(grdScnList, taskIdx+1);
}
//------------------------------------------------------------------------------
void TForm1::SaveIniData()
{
    TiXML::SaveWindowPlacement( form1Plcmnt_, Handle );
    TiXML::SaveConsolePlacement( consolePlcmnt_ );

    // ширина панелей статуса
    for (int i = 0; i<ComponentCount; ++i)
  	{
    	TStringGrid *grd = dynamic_cast<TStringGrid*>(Components[i]);
        if(grd) TiXML::SaveGrd(xmlTables_, grd);
    }

    TiXMLAtr::Set(GetAppXML().form1,
        "cbCurentScenary_ItemIndex", cbCurentScenary->ItemIndex );
    TiXMLAtr::Set(GetAppXML().form1,
        "lvDevs_Column1", lvDevs->Column[1]->Width );
    Devs::SaveDefaultKefsVals();
}
//------------------------------------------------------------------------------
void TForm1::ShowCurrentScenary()
{
    const int idx = cbCurentScenary->ItemIndex;
    cbCurentScenary->Items->Clear();
    const unsigned sz = scenaries_->Items().size();
    for( unsigned i=0; i<sz; ++i )
        if(i!=sz-1)
            cbCurentScenary->Items->Add( scenaries_->Items()[i]->What() );
    cbCurentScenary->ItemIndex = std::min( idx, cbCurentScenary->Items->Count-1 );
    cbCurentScenaryChange(cbCurentScenary);
}
//------------------------------------------------------------------------------
void TForm1::SaveCurrentScenaryReport(const AnsiString& scnReportFileName )
{
    ForceDirectories(ExtractFilePath(scnReportFileName));
    scnLog->Lines->SaveToFile(scnReportFileName);
}
//------------------------------------------------------------------------------
void TForm1::AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	MyGridCellState drawState, TRect rect, int& txtX, int& txtY, bool& wrapText )
{
	TCanvas &cnv = *grd->Canvas;
    const AnsiString cellText = grd->Cells[col][row];

    if( row==0 || col==0 )
    {
        cnv.Brush->Color = grd->FixedColor;
    }

    if(col>0 && row>0 && grd==grdScnList && !tmngr_.IsThreadStopedOrTerminated() &&
        tskLst_.GetCurrentTaskIndex()==row-1  )
            cnv.Brush->Color = clInfoBk;
    if( col==4 ) {
        if( cellText=="Прервано" ) cnv.Font->Color = clRed;
        if( cellText=="Выполнено" ) cnv.Font->Color = clBlue;
    }

    bool isLeftAligmentAndNoAutoColWidthCell = 0;
    OnDeterminateColumnWidth( grd, col, row, isLeftAligmentAndNoAutoColWidthCell);
    if( !isLeftAligmentAndNoAutoColWidthCell )
        txtX = rect.left + 3;

	int idxDev = -1;
    if( grd==grdVars ) idxDev = col - 1;
    if( grd==grdScnList ) idxDev = col - 5;
    if( idxDev>-1 && (unsigned)idxDev<Devs::GetAddys().size() ) {
        const unsigned addy = Devs::GetAddys()[idxDev];
        if( row==0 && IsNotAnsweredAddy(addy) )
            SetErrorStyleCell( cnv );
        if( cellText.Pos("[-]") )
        {
            cnv.Font->Color = clRed;
        }   else if( cellText.Pos("[+]") ) {
            cnv.Font->Color = clBlue;
        }
        if( grd==grdScnList && IsCellContains( grd, col, row, "не отвечает") )
            SetErrorStyleCell( cnv );
    }
    if( IsScenaryErrorRow(grd, row) )
    SetErrorStyleCell( cnv );
    if( row>0 && drawState & mygdSelected ) {
        cnv.Brush->Color = clMenuHighlight;
        cnv.Font->Color = clWhite;
    }
    if(grd==grdScnList && row>0 && col>2 ) wrapText = true;
}
//------------------------------------------------------------------------------
void TForm1::OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff)
{
    ff = !IsLeftAligmentAndNoAutoColWidthCell(grd, col, row);
}
//------------------------------------------------------------------------------
bool TForm1::IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row )
{
	return
    	( grd==grdVars && row>0 ) ||
        ( grd==grdScnList && col>0 && !(row==0 && col>4) ) ||
        ( grd==grdKfs && (col==1 || col==2) )
    ;
}
//------------------------------------------------------------------------------
void TForm1::OnGetKefValCellText( TStringGrid* grd, int col, int row, AnsiString& val )
{
    if(col<2) return;
    const unsigned nKef = row-1;
    const DAK::Kefs& kfs = DAK::GetKefs();
    const unsigned kfsCount = kfs.size();
    if(nKef>=kfsCount) return;
    val = kfs[nKef].Val2Key(val);
}
void TForm1::OnGetIndicationCellText( TStringGrid* grd, int col, int row, AnsiString& val )
{
    if(col==0 || row==0) return;
    val = DAK::Format::ConvertLongConc2Short(val);
}
//------------------------------------------------------------------------------
void TForm1::SetStatus( const AnsiString s, unsigned idx )
{
    assert( idx<STATUS_PNLS_COUNT );
    pnStatus_[idx]->Caption = s;
    pnStatus_[idx]->Hint = pnStatus_[idx]->Caption;
}
//------------------------------------------------------------------------------
void TForm1::AddLogAll( const AnsiString& s, const RichTextAttr &atrs )
{
    AddLog( s, atrs );
    for(unsigned nDev=0; nDev<Devs::Count(); ++nDev)
        if( Devs::IsSelectedDevice(nDev) )
            AddRicheditText( Devs::GetLog(nDev), s, atrs);
}
//------------------------------------------------------------------------------
void TForm1::AddLog( const AnsiString& s, const RichTextAttr &atrs )
{
    MyCout(s+"\n");
    RichTextAttr atrs1 = atrs;
    if(s.Pos("[+]"))
        atrs.TextColor(clBlue);
    else if(s.Pos("[-]"))
        atrs.TextColor(clRed);

    AddRicheditText( Form1->scnLog, s, atrs);
}
//------------------------------------------------------------------------------
void TForm1::AddAddyLog( unsigned addy, const AnsiString& s, const RichTextAttr &atrs )
{

    AddLog( MYSPRINTF_("%d: %s ", addy, s), atrs );
    if( Devs::IsSelectedAddy(addy) )
    if( TRichEdit *re = Devs::GetLogOfAddy(addy) )
        AddRicheditText( re, s, atrs);
}
//------------------------------------------------------------------------------
void TForm1::LogError( const AnsiString& msg )
{
    const ConsoleScreenAtrributesHolder cah(MY_SET_CONSOLE_RED_TEXT);
    const unsigned addy = ctrlSys_.GetIOSets().modbus->Addy();
    AddAddyLog( addy, msg, RichTextAttr().TextColor(clRed).
        FontStyles(TFontStyles()<< fsBold) ) ;
    FixScripResultForAddy( addy, msg, true);
}
//------------------------------------------------------------------------------
void TForm1::LogAddyError( unsigned addy, const AnsiString& msg )
{
    const ConsoleScreenAtrributesHolder cah(MY_SET_CONSOLE_RED_TEXT);
    AddAddyLog( addy, msg, RichTextAttr().TextColor(clRed).
        FontStyles(TFontStyles()<< fsBold) ) ;
    FixScripResultForAddy( addy, msg, true);
}
//------------------------------------------------------------------------------
void TForm1::LogPerformedAddy(  )
{
    const unsigned addy = ctrlSys_.GetIOSets().modbus->Addy();
    FixScripResultForAddy( addy, "Выполнено.", false);
    AddAddyLog( addy, "Выполнено", RichTextAttr().TextColor(clBlue) );
}
//------------------------------------------------------------------------------
void TForm1::SetMeasured(unsigned addy, unsigned nVar, const AnsiString& s)
{
    const int idx = Devs::IndexOfAddy(addy);
    if(idx==-1) return;
    if(nVar>=DAK::Vars::count) return;
    grdVars->Cells[1+idx][nVar+1] = s;
}
AnsiString TForm1::GetMeasured(unsigned addy, unsigned nVar)
{
    const int idx = Devs::IndexOfAddy(addy);
    if(idx==-1) return "";
    if(nVar>=DAK::Vars::count) return "";
    return grdVars->Cells[1+idx][nVar+1];
}

//------------------------------------------------------------------------------
void TForm1::ResetScenaryComboBox()
{
    TNotifyEvent onCh = cbCurentScenary->OnChange;
    cbCurentScenary->OnChange = NULL;
    //cbCurentScenary->ItemIndex = cbCurentScenary->Items->Count-1;
    cbCurentScenary->ItemIndex = -1;
    cbCurentScenary->OnChange = onCh;
}
//------------------------------------------------------------------------------


void __fastcall TForm1::menuAddDeviceClick(TObject *Sender)
{
    Devs::PromptUserInputDevice();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::menuDelDeviceClick(TObject *Sender)
{
    if ( ::MessageBox( Handle, "Удалить из списка выделенные элементы?",
    	"Список устройств.", MB_YESNO |
        MB_APPLMODAL | MB_ICONQUESTION )!=IDYES	)
    	return;

    lx:
	const unsigned sz = Devs::Count();
    for( unsigned i=0; i<sz; ++i )
    	if( lvDevs->Items->Item[i]->Selected  )
        {
        	Devs::DelDevice(i);
            goto lx;
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::menuChangeAddrClick(TObject *Sender)
{
    const int idx = Lv::ItemIndex( lvDevs );
    if( !( idx>-1 && idx<(int)Devs::Count() ) ) return;
    const AnsiString ss = MYSPRINTF_( "Изменить адрес прибора %d",
    	Devs::Serial(idx) );
    if ( ::MessageBox( Handle, (ss+"?").c_str(), "Список устройств.", MB_YESNO |
        MB_APPLMODAL | MB_ICONQUESTION )!=IDYES	)
    	return;
   	Devs::PromptUserChangeDevice(idx);
}
//---------------------------------------------------------------------------






void __fastcall TForm1::TimerTransferManagerProcessTimer(TObject *Sender)
{
    const MyTransferTask* task = tskLst_.GetCurrentTask();
    const int taskIdx = GetScenaryTaskIndex( scenary_, task );
    if( !tmngr_.IsThreadStopedOrTerminated() && task!=NULL && taskIdx>-1 )
    {
    	TDateTime oldTime;
    	if( TryStrToTime(grdScnList->Cells[2][taskIdx+1], oldTime) )
    		grdScnList->Cells[2][taskIdx+1] = TimeToStr( IncMilliSecond( oldTime,
            	TimerTransferManagerProcess->Interval ) );
    }
}
//------------------------------------------------------------------------------
void __fastcall TForm1::scnLogChange(TObject *Sender)
{
    ::SendMessage( scnLog->Handle, EM_SCROLL, SB_LINEDOWN, 0);
}
//------------------------------------------------------------------------------
void __fastcall TForm1::cbCurentScenaryChange(TObject *Sender)
{
    const int idx = cbCurentScenary->ItemIndex;
    const unsigned count = scenaries_->Items().size();
    if( !(idx>-1 && (unsigned)idx<count ) ) return;
    scenary_ = scenaries_->Items()[idx]->Items();

    ShowScenaryList(scenary_, grdScnList);
    
    tmngr_.StopThread();
    tskLst_.SetList(scenary_);
    //ActiveControl = NULL;
    //scnLog->SetFocus();
}
//---------------------------------------------------------------------------



void __fastcall TForm1::btnRunClick(TObject *Sender)
{
    const bool statrted = !tmngr_.IsThreadStopedOrTerminated();
    if( statrted ) {
        const bool isPause = !tmngr_.IsPaused();
        tmngr_.SetPause( isPause );
        imgPause->Visible = !isPause;
        imgCicleRun->Visible = isPause;
        return;
    }
    const int idx = cbCurentScenary->ItemIndex;
    //if(idx==-1) return;

    //cbCurentScenaryChange(NULL);
    tskLst_.SetList(scenary_);

    Scenary repeatScript;
    if(idx==0)
        repeatScript = scenaries_->Items()[0]->Items();
    else
    {
        tmngr_.SetFinalTask(PTransfer( new FinalizeTask ) );
        tmngr_.SetBeginTask(PTransfer( new InitializeTask ) );
    }


    tmngr_.SetRepeatScript( repeatScript );
    tmngr_.StartThread();
    OnTransferManagerReport(TransferManagerT::State::START, "");
}
//---------------------------------------------------------------------------



void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
    CtrlSys::Instance().SetupDialog();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::N11Click(TObject *Sender)
{
    HWND hCon = ::GetConsoleWindow();
    WINDOWPLACEMENT conPl = {0};
    ::GetWindowPlacement(hCon, &conPl);
    long
        &l = conPl.rcNormalPosition.left,
        &t = conPl.rcNormalPosition.top,
        &r = conPl.rcNormalPosition.right,
        &b = conPl.rcNormalPosition.bottom;
    const bool
        conWndNotVis = ::IsWindowVisible(hCon)==FALSE,
        notVis = conWndNotVis || ( !conWndNotVis && (conPl.showCmd==2) );
    ::ShowWindow(hCon, notVis ? SW_SHOWNORMAL : SW_HIDE );
    if(t>Width-100 || l>Height-100 )
    {
        conPl.showCmd = SW_SHOWNORMAL;
        const long
            conWidth = r - l,
            conHeight = b - t;
        l = 10;
        t = 10;
        r = l + conWidth;
        b = t + conHeight;
        ::SetWindowPlacement(hCon, &conPl);
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::menuUnselectDeviceClick(TObject *Sender)
{
    const bool v = static_cast<TMenuItem*>(Sender)->Tag;
    for( int i=0; i<lvDevs->SelCount; ++i )
        Devs::SelectDevice(i, v);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::menuUnselectClick(TObject *Sender)
{
    //
    const unsigned idx = grdScnList->Row-1;
    if( idx<scenary_.size() )
        if( ScenaryAction *task = dynamic_cast<ScenaryAction*>( scenary_[idx].get() ) )
        {
            AnsiString id = task->ClassId();
            task->SetupParams();
            grdScnList->Cells[3][idx+1] = task->What();
        }

}
//---------------------------------------------------------------------------


void __fastcall TForm1::btnSkipClick(TObject *Sender)
{
    const MyTransferTask* task = tskLst_.GetCurrentTask();
    const int taskIdx = GetScenaryTaskIndex( scenary_, task );
    if ( task!=NULL && taskIdx>-1 && grdScnList->Cells[4][taskIdx+1]=="Выполняется")
    {
        grdScnList->Cells[4][taskIdx+1] = "Прервано";

        AddLog("");
        AddLog("Прервано!", RichTextAttr().TextColor(clRed).
            FontStyles( TFontStyles() << fsBold << fsUnderline).TextSize(12).ContinueLine().Time() );
    }
    MyTransferTask::SkipCurrentOperation();
}
//---------------------------------------------------------------------------



void __fastcall TForm1::N27Click(TObject *Sender)
{
    tmngr_.StopThread();
    tmngr_.ResetScripts();
    Pneumo::AddSwitchGasTask(-1);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::menuPerformComandClick(TObject *Sender)
{
    AnsiString sArg = "0";
    double arg = 0;
    if( !InputQuery("Введите значение аргумента", "Введите значение аргумента",
        sArg) || !TryStrToFloat(sArg, arg) )
        return;


    TMenuItem* menu = static_cast<TMenuItem*>(Sender);
    const unsigned idx = menu->MenuIndex;
    if(idx==-1) return;
    const AnsiString s = menu->Caption;

    const bool broadCast = ::MessageBox( Handle, MYSPRINTF_(
        "Отправить команду\n%s\nшироковещательно?",s).c_str(),
            "Отправить широковещательно?",MB_YESNO)==IDYES;

    TransferManagerT &tmngr = TransferManager::Instance();
    MyTransferTask *task;

    const unsigned code = DAK::Cmd::Code::all[idx];


    if(idx==DAK::Cmd::Idx::addy)
        task = new SetAddyTask( arg );
    else {
        if(broadCast)
            task = new WriteModbusBroadcastTask( code, arg );
        else
            task = new WriteModbusTask( code, arg );
    }

    tmngr.StopThread();
    tmngr.ResetScripts();
    tmngr.AddTask( task );
    ResetScenaryComboBox();
    tmngr.StartThread();
}
//--------------------------------------------------------------------------- 
void __fastcall TForm1::N16Click(TObject *Sender)
{
    const bool v = static_cast<TMenuItem*>(Sender)->Tag;
    TListItem *itm0 = lvDevs->Selected;
    if( !itm0 ) return;
    for( int i=0; i<lvDevs->SelCount; ++i )
        Devs::SelectDevice(itm0->Index+i, v);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::N18Click(TObject *Sender)
{
    AnsiString fn;
    TiXMLAtr::Holder attr = TiXMLAtr::CreateHolderRef( xmlReportFile_, "путь", fn );

    if( !PromptForFileName(fn, "Протокол настройки (*.rtf)|*.rtf", "rtf",
    	"Сохранить протокол настройки", fn, true) ) return;
    ForceDirectories(ExtractFilePath(fn));
    scnLog->Lines->SaveToFile(fn);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N19Click(TObject *Sender)
{
    AnsiString fn;
    if( !PromptForFileName(fn, "Cценарий настройки (*.xml)|*.xml", "xml",
        "Сохранить сценарий настройки ", "", true) ) return;
    SaveScenaryToXML( fn, scenaries_ );

}
//---------------------------------------------------------------------------

void __fastcall TForm1::N12Click(TObject *Sender)
{
    AnsiString fn;
    if( !PromptForFileName(fn, "Cценарий настройки (*.xml)|*.xml", "xml",
        "Открыть сценарий настройки ", "", false) ) return;
    scenaries_ = LoadScenaryFromXML( fn );
    ShowCurrentScenary();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::N20Click(TObject *Sender)
{
    scenaries_ = CreateDefaultScenary();
    ShowCurrentScenary();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::popupMenuScriptPopup(TObject *Sender)
{
    menuEditTask->Visible = 0;
    menuEditScript->Visible = 0;
    const unsigned nScript = cbCurentScenary->ItemIndex;
    if( nScript>=scenaries_->Items().size() ) return;
    menuEditScript->Caption = MYSPRINTF_("Редактировать сценарий \"%s\"",
        MyTurncStr1( scenaries_->Items().at(nScript)->What(), 25 ) );
    menuEditScript->Visible=1;
    PTransfer selectedOperation = scenaries_->Items().at(nScript);
    const unsigned nTask = grdScnList->Row-1;
    if( nTask>=selectedOperation->Items().size() ) return;
    PTransfer task = selectedOperation->Items().at(nTask);
    menuEditTask->Caption = MYSPRINTF_("Редактировать операцию \"%s\"",
        MyTurncStr1( task->What(), 25)  );
    menuEditTask->Visible=1;
}
//------------------------------------------------------------------------------

void __fastcall TForm1::menuEditTaskClick(TObject *Sender)
{
    const unsigned nScript = cbCurentScenary->ItemIndex;
    if( nScript>=scenaries_->Items().size() ) return;
    PTransfer scn = scenaries_->Items().at(nScript);
    const unsigned nTask = grdScnList->Row-1;
    if( nTask>=scn->Items().size() ) return;
    PTransfer task = scn->Items().at(nTask);

    if( ReferenceTask *pRef = dynamic_cast<ReferenceTask*>( task.get() ) )
        task = pRef->GetReferencedTask();

    const AnsiString
        fn = (MyGetExePath()+"tmp.xml").c_str(),
        fn1 = (MyGetExePath()+"tmp1.xml").c_str()
        ;
    boost::shared_ptr<TiXmlDocument> doc (  TiXML::CreateDocument( fn.c_str() ) );
    TiXmlElement *elem = doc->LinkEndChild( new TiXmlElement( "Сценарий" ) )->ToElement();
    task->SaveXML( elem );

    doc->SaveFile(fn1.c_str());
    doc->SaveFile(fn.c_str());
    
	const AnsiString fnExe = GetXMLEditorExeFileName();
    if(!MyFileExists(fnExe) ) return;
    Application->Minimize();
    OpenFileOtherApplication(fn, fnExe, "");
    doc->LoadFile();
    ::DeleteFile(fn.c_str());


    try
    {
        task->LoadXML( doc->FirstChildElement() );
        scenaries_->RootAceptReference();
    }
    catch(...)
    {
        ::MessageBox( Screen->ActiveForm->Handle,
            MY_RETHROW_->Message().c_str(),
            "", MB_OK | MB_APPLMODAL | MB_ICONERROR  );
        doc->LoadFile( fn1.c_str() );
        task->LoadXML( doc->FirstChildElement() );
        scenaries_->RootAceptReference();
    }


    ::DeleteFile( fn.c_str() );
    ::DeleteFile( fn1.c_str() );
    grdScnList->Cells[3][grdScnList->Row] = task->What();
    Application->Restore();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::menuEditScriptClick(TObject *Sender)
{
    const unsigned nScript = cbCurentScenary->ItemIndex;
    if( nScript>=scenaries_->Items().size() ) return;
    PTransfer scn = scenaries_->Items().at(nScript);


    const AnsiString
        fn = (MyGetExePath()+"tmp.xml").c_str(),
        fn1 = (MyGetExePath()+"tmp1.xml").c_str();
    boost::shared_ptr<TiXmlDocument> doc (  TiXML::CreateDocument( fn.c_str() ) );

    TiXmlElement* elem = doc->LinkEndChild( new TiXmlElement( scn->ClassId().c_str() ) )->ToElement();
    scn->SaveXML(elem);

    doc->SaveFile(fn1.c_str());
    doc->SaveFile(fn.c_str());

	const AnsiString fnExe = GetXMLEditorExeFileName();
    if(!MyFileExists(fnExe) ) return;
    Application->Minimize();
    OpenFileOtherApplication(fn, fnExe, "");


    try
    {
        doc->LoadFile();
        elem = doc->FirstChildElement();
        scn->LoadXML(elem);
        scenaries_->RootAceptReference();

    }
    catch(...)
    {
        ::MessageBox( Handle,
            MY_RETHROW_->Message().c_str(),
            "", MB_OK | MB_APPLMODAL | MB_ICONERROR  );
        doc->LoadFile( fn1.c_str() );
        scn->LoadXML(elem);
        scenaries_->RootAceptReference();
    }

    ::DeleteFile( fn.c_str() );
    ::DeleteFile( fn1.c_str() );
    ShowScenaryList(scn->Items(), grdScnList);
    Application->Restore();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnHeatchamSetSetpoint1Click(TObject *Sender)
{
    //HeatCham::SetSetpoint(-12);
    int setPoint = 0;
    SendHeatChamControlCommandTask::Cmd cmd;

    if(Sender==btnHeatchamSetSetpoint || Sender==btnHeatchamSetSetpoint1)
    {
        AnsiString sS = btnHeatchamSetpoint->Caption;
	    if( !InputQuery( "Запрос значения",
            "Введите уставку термокамеры в градусах Цельсия", sS) ) return;

        setPoint = Ceil( ( MyStrToFloatDef(sS, 20)*10 ) );
        cmd = SendHeatChamControlCommandTask::setpoint;
    }
    else if(Sender==btnHeatchamStart)
        cmd =SendHeatChamControlCommandTask::start;
    else if(Sender==btnHeatchamStop)
        cmd =SendHeatChamControlCommandTask::stop;
    else if(Sender==btnHeatchamFix)
        cmd =SendHeatChamControlCommandTask::fix;
    else assert(false);


    tmngr_.StopThread();
    tmngr_.ResetScripts();

    tmngr_.AddTask( new SendHeatChamControlCommandTask(cmd, setPoint ) );

    ResetScenaryComboBox();
    tmngr_.StartThread();
}
//---------------------------------------------------------------------------







void __fastcall TForm1::grdScnListKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    TStringGrid* grd = static_cast<TStringGrid*>(Sender);
    //const int aRow = grd->Row;
    if( Key == VK_ESCAPE )
    {
        my::Grd::InvertRowSelectionOfSelection( grd );
    } else
    if( Key == 65 && Shift.Contains(ssCtrl)  )
    {
    	TGridRect r, sel = grd->Selection;
        r.Left = 1;
  		r.Top = 1;
  		r.Right = grd->ColCount-1;
  		r.Bottom = grd->RowCount-1;
    	grd->Selection = r;
        my::Grd::InvertRowSelectionOfSelection( grd );
        grd->Selection = sel;
    }
    if(grd!=grdScnList) return;
    if(  Key!=VK_DELETE || scenary_.empty() ||
        !tmngr_.IsThreadStopedOrTerminated() ||
        tmngr_.IsFinalScripPerforming() ||
        ::MessageBox( Handle, "Исключить выделенные операции из сценария настройки?",
            "", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION )!=IDYES)
        return;
    PTransfer
        *p = scenary_.begin(), *pEnd = scenary_.end(),
        *p1 = p + grdScnList->Selection.Top-1,
        *p2 = p + grdScnList->Selection.Bottom;
    if( !( p1>=p && p1<pEnd && p2>=p && p2<=pEnd && p2>p1) )
        return;
    scenary_.erase( p1, p2 );
    tskLst_.SetList(scenary_);
    ShowScenaryList(scenary_, grdScnList);
    ActiveControl =  grdScnList;
    //ResetScenaryComboBox();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::grdKfsSelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
    TStringGrid* grd = static_cast<TStringGrid*>(Sender);
	my::Grd::SetEditing(grd,
    	(grd==grdKfs && ACol>1 && ARow>0)
    );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N2Click(TObject *Sender)
{
    AnsiString ss;
    ss.sprintf("%s значения выбранных коэффициентов\nвыбранных приборов?",
        Sender!=menuWriteKefs ? "Считать" : "Записать");

   	if( ::MessageBox( Handle, ss.c_str(), "Подтверждение",
    	MB_OKCANCEL | MB_APPLMODAL | MB_ICONQUESTION )!=IDOK )
        return;
	// настроить поток опроса
    tmngr_.StopThread();

    const Devs::Nums addys = Devs::GetSelectedKefsNums();
    MyTransferTask *trnsfr;

    if( Sender!=menuWriteKefs )
    	trnsfr =  new ReadKoefsTask(addys);
    else
    	trnsfr =  new WriteKoefsTask(addys);
    tmngr_.ResetScripts();
    tmngr_.AddTask( trnsfr );
    ResetScenaryComboBox();
    tmngr_.StartThread();
}
//--------------------------------------------------------------------------- 
void __fastcall TForm1::N13Click(TObject *Sender)
{
    Devs::SetDefaultSelectedKefsItemsVals();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::menuKefsPopup(TObject *Sender)
{
    const unsigned nKef = grdKfs->Row-1;
    const DAK::Kefs& kfs = DAK::GetKefs();
    const unsigned kfsCount = kfs.size();
    const TGridRect sel = grdKfs->Selection;
    if(grdKfs->Col<2 || sel.Top!=sel.Bottom || nKef>=kfsCount || kfs[nKef].props.empty() ) {
        menuKef->Visible = 0;
        menuKefBreak->Visible = 0;
        return;
    }
    const DAK::Kef& kef = kfs[nKef];
    const DAK::Kef::Props& props = kef.props;
    menuKef->Visible = 1;
    menuKefBreak->Visible = 1;
    menuKef->Caption = MYSPRINTF_("%d. %s", kef.n, kef.caption);
    menuKef->Clear();

    for( unsigned i=0; i<props.size(); ++i) {
        TMenuItem *menu = new TMenuItem(menuKef);
        menu->Caption = props[i].key;
        menu->Tag = nKef;
        menu->OnClick = menuKefPropClick;
        menuKef->Add(menu);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::menuKefPropClick(TObject *Sender)
{
    const TGridRect sel = grdKfs->Selection;
    if(grdKfs->Col<2 || sel.Top!=sel.Bottom) return;

    const unsigned nKef = grdKfs->Row-1;
    const DAK::Kefs& kfs = DAK::GetKefs();
    const unsigned kfsCount = kfs.size();
    if(nKef>=kfsCount) return;

    const DAK::Kef::Props& props = kfs[nKef].props;

    TMenuItem *menu = static_cast<TMenuItem *>( Sender );
    const unsigned itmIdx = menu->MenuIndex;
    if(itmIdx>=props.size() ) return;

    for(int cl=sel.Left; cl<=sel.Right; ++cl)
        grdKfs->Cells[cl][grdKfs->Row] = props[itmIdx].val;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnPerformComandClick(TObject *Sender)
{
    ActivatePopupMenu(this->PopupMenuComand);    
}
//---------------------------------------------------------------------------


void __fastcall TForm1::PopupMenu1Popup(TObject *Sender)
{
    menuTblView->Checked = lvDevs->ViewStyle==vsReport;
    menuListView->Checked = !menuTblView->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::menuTblViewClick(TObject *Sender)
{
    lvDevs->ViewStyle = vsReport;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::menuListViewClick(TObject *Sender)
{
    lvDevs->ViewStyle = vsList;    
}
//---------------------------------------------------------------------------



void __fastcall TForm1::btnStopClick(TObject *Sender)
{
    tmngr_.StopThread();    
}
//---------------------------------------------------------------------------






void __fastcall TForm1::N5Click(TObject *Sender)
{
    CreatePasports( true, true );    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N7Click(TObject *Sender)
{
    AnsiString fn;
    if( !PromptForFileName(fn, "Данные ДАК-М (*.xml)|*.xml", "xml",
        "Открыть файл данных ДАК-М", "", false) ) return;
    Devs::LoadFile( fn );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N8Click(TObject *Sender)
{
    AnsiString fn;
    if( !PromptForFileName(fn, "Данные ДАК-М (*.xml)|*.xml", "xml",
        "Сохранить файл данных ДАК-М", "", true) ) return;
    Devs::SaveFile( fn );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N9Click(TObject *Sender)
{
    Devs::ClearDevicesData();     
}
//---------------------------------------------------------------------------


void __fastcall TForm1::N10Click(TObject *Sender)
{
    AnsiString fn;
    if( !PromptForFileName(fn, "Данные ДАК-М (*.dev)|*.dev", "dev",
        "Сохранить файл данных ДАК-М", "", true) ) return;
    //Devs::SaveFile( fn );
    Devs::SaveForTermoFile(fn);
}
//---------------------------------------------------------------------------

