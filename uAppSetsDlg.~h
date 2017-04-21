//---------------------------------------------------------------------------

#ifndef uAppSetsDlgH
#define uAppSetsDlgH
//---------------------------------------------------------------------------
#include "DrawGridCell.h"
#include "my_array_size.h"
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <CheckLst.hpp>
#include <Buttons.hpp>
#include <Series.hpp>
#include <Chart.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
//------------------------------------------------------------------------------
// std
#include <vector>
#include <map>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include "MyStrValRef.hpp"
#include "xmlUtils_.h"
//------------------------------------------------------------------------------
namespace AppSetsGrdItems
{
    namespace Row
    {
        enum
        {
        ttl,

    	COM,
        COM_heatcham,

    	Timeout,
    	WaitDelay,
        SilentTime,
    	RepeatCount,

        Timeout_heatcham,
    	WaitDelay_heatcham,
        SilentTime_heatcham,
    	RepeatCount_heatcham,

        ShowComPortLog,
        PneumoAddy,
        JurikStendAddy,


        SoftVer,
        Isp,
        Gas,
        Scale,

        Count
        };

    }; //namespace Row{ enum {

    const char * const ttls[] =
    {
    "Параметр",
    "COM-порт приборов",
    "COM-порт термокамеры",

    "таймаут ответа",
    "задержка отправки",
    "длительность ожидания символа приёма",
    "количество повторов запроса",

    "термокамера: таймаут ответа",
    "термокамера: задержка отправки",
    "термокамера: длительность ожидания символа приёма",
    "термокамера: количество повторов запроса",


    "Показывать посылки",
    "Адрес пневмоблока",
    "Адрес стенда",


    "Версия ПО",
    "Исполнение",    
    "Газ",
    "Шкала"
    };
};
//---------------------------------------------------------------------------


//------------------------------------------------------------------------------
class TFormAppSetsDlg : public TForm
{
__published:	// IDE-managed Components
    TStringGrid *grd1;
    TComboBox *cbPortName;
    TEdit *edGrd;
    TComboBox *ComboBox1;
    TPanel *Panel2;
    TRadioButton *RadioButton1;
    TRadioButton *RadioButton2;
    TComboBox *cbGas;
    TComboBox *cbScale;
    TRadioButton *RadioButton3;
    TPanel *Panel1;
    TButton *Button1;
    TButton *Button2;
    TCheckBox *CheckBox1;
    TLabel *Label1;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall CheckListBox1ClickCheck(TObject *Sender);
    void __fastcall grd1SelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
    void __fastcall grd1SetEditText(TObject *Sender, int ACol, int ARow,
          const AnsiString Value);
    void __fastcall cbChanalKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall RadioButton1Click(TObject *Sender);
public:
    enum
    {
        VALS_COUNT =  AppSetsGrdItems::Row::Count-1
    };
private:	// User declarations
    my::DrawGridCellManager drwGrd;
    PMyStrValRef refs_[VALS_COUNT];
    std::vector<bool> changed_;

    TiXmlElement *xmlPlcmnt_, *xml_, *xmlGrd_;

    void AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
		MyGridCellState drawState, TRect rect, int& txtX, int& txtY, bool& );
    void OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff);
    bool IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row );

public:		// User declarations
	__fastcall TFormAppSetsDlg(TComponent* Owner, PMyStrValRef* refs);
    virtual __fastcall ~TFormAppSetsDlg();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAppSetsDlg *FormAppSetsDlg;
//---------------------------------------------------------------------------
#endif
