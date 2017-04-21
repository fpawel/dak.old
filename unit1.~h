//---------------------------------------------------------------------------
#ifndef Unit1H
#define Unit1H
#include <ActnList.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
#include <Chart.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
#include <Graphics.hpp>
#include <StdActns.hpp>
#include <CheckLst.hpp>
#include "ThunderbirdTreeMain.hpp"

//------------------------------------------------------------------------------
//std
#include <map>
#include <set>


#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"

#include "..\my_include\free_types_.h"
#include "DrawGridCell.h"
#include "MyAbstructTransfer.hpp"
#include "vardef.hpp"
#include "MyIostream.h"
#include "xmlUtils_.h"
#include "MyRichEdit.h"
//------------------------------------------------------------------------------

class TransferManagerT;
class TaskList;
class CtrlSysImpl;
class TFrameDropDownPanel;
class TFrameGraph;
class TFormMdiChild;

class TForm1 : public TForm
{
private:
    boost::shared_ptr<void> lifeTime_;

__published:	// IDE-managed Components
	TPanel *panelStatus;
	TSplitter *Splitter2;
	TSplitter *Splitter3;
	TSplitter *Splitter4;
	TPanel *Panel9;
	TPanel *Panel10;
	TPanel *Panel11;
	TPanel *Panel12;
    TPopupMenu *PopupMenu1;
    TMenuItem *menuAddDevice;
    TMenuItem *menuDelDevice;
    TMenuItem *menuChangeAddr;
    TTimer *TimerTransferManagerProcess;
    TPanel *Panel2;
    TPanel *panelConnect;
    TPanel *Panel5;
    TImage *imgStopEnabled;
    TImage *imgCicleRun;
    TSpeedButton *btnRun;
    TImage *Image1;
    TSpeedButton *SpeedButton1;
    TComboBox *cbCurentScenary;
    TMainMenu *MainMenu1;
    TMenuItem *Afqk1;
    TMenuItem *N3;
    TMenuItem *N11;
    TMenuItem *N12;
    TImage *imgSkipEnabled;
    TSpeedButton *btnSkip;
    TMenuItem *N14;
    TMenuItem *N15;
    TMenuItem *menuSelect;
    TMenuItem *N16;
    TMenuItem *N18;
    TMenuItem *N19;
    TMenuItem *N20;
    TMenuItem *N21;
    TPopupMenu *popupMenuScript;
    TMenuItem *menuEditTask;
    TMenuItem *menuEditScript;
    TPopupMenu *menuKefs;
    TMenuItem *menuWriteKefs;
    TMenuItem *N2;
    TMenuItem *N13;
    TMenuItem *N1;
    TMenuItem *N17;
    TMenuItem *menuKef;
    TMenuItem *menuKefBreak;
    TMenuItem *menuKefProp;
    TScrollBox *ScrollBox2;
    TSplitter *Splitter1;
    TPanel *Panel1;
    TPopupMenu *PopupMenuComand;
    TPageControl *PageControlDevicesData;
    TTabSheet *TabSheet6;
    TStringGrid *grdVars;
    TTabSheet *TabSheet7;
    TStringGrid *grdScnList;
    TTabSheet *TabSheet8;
    TStringGrid *grdKfs;
    TTabSheet *TabSheet2;
    TPanel *Panel6;
    TPanel *PanelDevicesList;
    TPanel *Panel8;
    TListView *lvDevs;
    TPanel *panelHeatcham;
    TLabel *Label3;
    TLabel *btnHeatchamSetSetpoint;
    TLabel *btnHeatchamSetpoint;
    TLabel *btnHeatchamT;
    TLabel *btnHeatchamStart;
    TLabel *btnHeatchamStop;
    TLabel *btnHeatchamFix;
    TLabel *btnHeatchamSetSetpoint1;
    TTabSheet *TabSheet1;
    TThunderbirdTree *ThunderbirdTree2;
    TPageControl *PageControlPasport;
    TLabel *btnPerformComand;
    TMenuItem *N4;
    TMenuItem *menuTblView;
    TMenuItem *menuListView;
    TSpeedButton *btnStop;
    TImage *imgPause;
    TImage *imgStopDisabled;
    TImage *imgSkipDisabled;
    TMenuItem *nn1;
    TMenuItem *N5;
    TPageControl *PageControlReport;
    TTabSheet *TabSheetMainReport;
    TRichEdit *scnLog;
    TSplitter *Splitter5;
    TMenuItem *N6;
    TMenuItem *N7;
    TMenuItem *N8;
    TMenuItem *N9;
    TMenuItem *N10;
    TTabSheet *TabsheetPageControlChart;
    TPageControl *PageControlChart;
	void __fastcall FormActivate(TObject *Sender);
    void __fastcall menuAddDeviceClick(TObject *Sender);
    void __fastcall menuDelDeviceClick(TObject *Sender);
    void __fastcall menuChangeAddrClick(TObject *Sender);
    void __fastcall TimerTransferManagerProcessTimer(TObject *Sender);
    void __fastcall scnLogChange(TObject *Sender);
    void __fastcall cbCurentScenaryChange(TObject *Sender);
    void __fastcall btnRunClick(TObject *Sender);
    void __fastcall SpeedButton1Click(TObject *Sender);
    void __fastcall N11Click(TObject *Sender);
    void __fastcall menuUnselectDeviceClick(TObject *Sender);
    void __fastcall menuUnselectClick(TObject *Sender);
    void __fastcall btnSkipClick(TObject *Sender);
    void __fastcall N27Click(TObject *Sender);
    void __fastcall menuPerformComandClick(TObject *Sender);
    void __fastcall N16Click(TObject *Sender);
    void __fastcall N18Click(TObject *Sender);
    void __fastcall N19Click(TObject *Sender);
    void __fastcall N12Click(TObject *Sender);
    void __fastcall N20Click(TObject *Sender);
    void __fastcall popupMenuScriptPopup(TObject *Sender);
    void __fastcall menuEditTaskClick(TObject *Sender);
    void __fastcall menuEditScriptClick(TObject *Sender);
    void __fastcall btnHeatchamSetSetpoint1Click(TObject *Sender);
    void __fastcall grdScnListKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall grdKfsSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
    void __fastcall N2Click(TObject *Sender);
    void __fastcall N13Click(TObject *Sender);
    void __fastcall menuKefsPopup(TObject *Sender);
    void __fastcall menuKefPropClick(TObject *Sender);
    void __fastcall btnPerformComandClick(TObject *Sender);
    void __fastcall PopupMenu1Popup(TObject *Sender);
    void __fastcall menuTblViewClick(TObject *Sender);
    void __fastcall menuListViewClick(TObject *Sender);
    void __fastcall btnStopClick(TObject *Sender);
    void __fastcall N5Click(TObject *Sender);
    void __fastcall N7Click(TObject *Sender);
    void __fastcall N8Click(TObject *Sender);
    void __fastcall N9Click(TObject *Sender);
    void __fastcall N10Click(TObject *Sender);
private:	// User declarations
    TransferManagerT& tmngr_;
    TaskList& tskLst_;
    CtrlSysImpl& ctrlSys_;

	my::DrawGridCellManager drwGrdVars, drwGrdScnList, drwGrdKfs;

	// колличество панелей статуса
    enum{ STATUS_PNLS_COUNT = 4 };

    // правые панели
    TPanel* pnStatus_[STATUS_PNLS_COUNT];
    PVoid connectOnTransferManagerReport_;

    PTransfer scenaries_;
    Scenary scenary_;
    bool scenaryErrorFixed_;
  

    std::vector<TiXMLAtr::Holder> xmlAtrs_;
    TiXmlElement *form1Plcmnt_, *consolePlcmnt_, *xmlTables_, *xmlReportFile_;
    AnsiString selectedKefs_, selectedVars_;
    template<typename T>
    void AddAtr( TiXmlElement* elem, const char *name, T& ref )
    {
        xmlAtrs_.push_back( TiXMLAtr::CreateHolderRef( elem, name, ref ) );
    }


    void SetStatus( const AnsiString s, unsigned idx );

    //void OnAfterReadModbusRegister( unsigned regAdr, double val );
    void AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
		MyGridCellState drawState, TRect rect, int& txtX, int& txtY, bool& wrapText );
    void OnDeterminateColumnWidth( TStringGrid* grd, int col, int, bool& ff);
    bool IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row );

    // HandleTransfer.cpp
    void HandleException();
    void OnTransferManagerReport(unsigned context, const AnsiString& state);
    void HandleBeginOrEndTransfer(const AnsiString& state, bool isBeginTransfer);
    void HandleStopOrStartThread(const AnsiString& state, bool isStartThreadMsg);

    
    void FixScripResultForAddy(unsigned addy, const AnsiString& s, bool failed);
    void ResetScenaryComboBox();

    void ShowCurrentScenary();
    void SaveIniData();
    void SaveCurrentScenaryReport(const AnsiString& scnReportFileName );
    void __fastcall OnClickKefPropItem(TObject *Sender);

    void OnGetKefValCellText( TStringGrid* grd, int col, int row, AnsiString& val );
    void OnGetIndicationCellText( TStringGrid* grd, int col, int row, AnsiString& val );

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
    __fastcall ~TForm1();

    void AddLog( const AnsiString& s, const RichTextAttr &atrs = RichTextAttr() );
    void AddAddyLog( unsigned addy, const AnsiString& s, const RichTextAttr &atrs = RichTextAttr() );
    void AddLogAll( const AnsiString& s, const RichTextAttr &atrs = RichTextAttr() );
    void LogError( const AnsiString& s);

    void LogAddyError( unsigned addy, const AnsiString& msg );

    void LogPerformedAddy();

    void SetMeasured(unsigned addy, unsigned nVar, const AnsiString& s);
    AnsiString GetMeasured(unsigned addy, unsigned nVar);

    bool ScenaryErrorFixed() const { return scenaryErrorFixed_; }
    bool MustSkipTask(const MyTransferTask*);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
bool IsScenaryErrorRow( TStringGrid* grd, int row );
void ShowScenaryList(const Scenary& lst, TStringGrid* grd);
#endif
