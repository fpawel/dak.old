//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "uAppSetsDlg.h"
//------------------------------------------------------------------------------
#include "boost/assign/list_of.hpp"
//------------------------------------------------------------------------------
#include <Math.hpp>
#include <FileCtrl.hpp>
#include <dialogs.hpp>
//------------------------------------------------------------------------------
#include <set>
//------------------------------------------------------------------------------
#include "CommSets.h"
#include "DrawGridCell.h"
#include "SGrdHlpr.h"
#include "AnsiStringUtils_.h"
#include "bcd_.h"
#include "my_array_size.h"
#include "MyVclUtils_.h"
#include "iniUtils_.h"
#include "AppXML.h"
#include "vardef.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormAppSetsDlg *FormAppSetsDlg;



namespace AppSetsGrdItems
{
    namespace Row
    {
        void SetGrp(TStringGrid *grd, unsigned n)
        {
            typedef std::set<unsigned> Rows;
            #define INIT_VALS_ Rows( boost::assign::list_of<int>
            const Rows  sets[] = {
                INIT_VALS_ (COM)(COM_heatcham)(JurikStendAddy)(PneumoAddy) ),
                INIT_VALS_ (Gas)(Scale)(SoftVer)(Isp) ),
                INIT_VALS_ (Timeout)(WaitDelay)(SilentTime)(RepeatCount)(Timeout_heatcham)
                    (WaitDelay_heatcham)(SilentTime_heatcham)(RepeatCount_heatcham)(ShowComPortLog) ) };
            #undef INIT_VALS_
            for(int ro=1; ro<grd->RowCount; ++ro)
                my::Grd::SetRowVis(grd, ro, sets[n].find(ro)!=sets[n].end() );
        }
    };
};
//---------------------------------------------------------------------------
#define INI_SEKT "Диалог_настроек"


//---------------------------------------------------------------------------
__fastcall TFormAppSetsDlg::TFormAppSetsDlg(TComponent* Owner, PMyStrValRef* refs)
	: TForm(Owner), drwGrd(grd1), changed_(VALS_COUNT, false),
    xmlPlcmnt_( TiXML::ChildElement( GetAppXML().windowsPlacements, "диалог_настройки_приложения" ) ),
    xml_( TiXML::ChildElement( GetAppXML().form1, "диалог_настройки_приложения" ) ),
    xmlGrd_( TiXML::ChildElement( xml_, "таблица" ) )
{
    Caption = "Параметры приложения.";

    grd1->OnSetEditText = NULL;

    std::copy( refs, refs+VALS_COUNT, refs_ );
    for( unsigned i=0; i<VALS_COUNT; ++i )
        if(refs_[i])
            grd1->Cells[2][i+1] = refs_[i]->Get();


	// имена СОМ-портов системы
    const std::vector<AnsiString>& ports = my::RS232::SystemPorts();


    for( unsigned i = 0; i<ports.size(); ++i )
    	cbPortName->Items->Add( ports[i] );


    drwGrd.SetAfterSetCanvasFoo( &AfterSetCanvasGrd );
    drwGrd.SetOnDeterminateColumnWidthFoo( &OnDeterminateColumnWidth );


    using namespace AppSetsGrdItems;

    grd1->RowCount = AppSetsGrdItems::Row::Count;
    grd1->Cells[1][0] = "Примечание";
    grd1->Cells[2][0] = "Значение";
    my::Grd::SetColumn( grd1, 0, ttls, ttls+AppSetsGrdItems::Row::Count);

    Height = AppSetsGrdItems::Row::Count*grd1->DefaultRowHeight + 100;

    using namespace Row;
    StringgridControl::RegisterRowControl( cbPortName, grd1, COM );
    StringgridControl::RegisterRowControl( cbPortName, grd1, COM_heatcham );
    StringgridControl::RegisterOneEditControl( edGrd, grd1  );
    StringgridControl::RegisterRowControl( ComboBox1, grd1, ShowComPortLog );
    StringgridControl::RegisterRowControl( cbGas, grd1, Gas );
    StringgridControl::RegisterRowControl( cbScale, grd1, Scale );

    grd1->Cells[1][Timeout] = "мс";
    grd1->Cells[1][SilentTime] = "мс";
    grd1->Cells[1][WaitDelay] = "мс";

    SetGrp(grd1, 0);

    grd1->OnSetEditText = grd1SetEditText;


    cbGas->Items->Clear();
    cbScale->Items->Clear();

    using namespace DAK;
    MAIN_KEF_(Scale).SetStringsKeys( cbScale->Items );
    MAIN_KEF_(Gas).SetStringsKeys( cbGas->Items );
}
//---------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::FormActivate(TObject *Sender)
{
    OnActivate = NULL;
    TiXML::SaveWindowPlacement(xmlPlcmnt_, Handle);
    TiXML::SaveGrd(xml_, grd1);
}
//------------------------------------------------------------------------------
__fastcall TFormAppSetsDlg::~TFormAppSetsDlg()
{
    StringgridControl::UnregisterAll( this );
    TiXML::LoadWindowPlacement(xmlPlcmnt_, Handle);
    TiXML::LoadGrd(xml_, grd1);

    if(ModalResult==mrOk)
        for( unsigned i=0; i<VALS_COUNT; ++i )
            if( changed_[i] && refs_[i] .get()!=0  )
                refs_[i]->Set( grd1->Cells[2][i+1] );
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void TFormAppSetsDlg::AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	MyGridCellState drawState, TRect rect, int& txtX, int& txtY, bool& )
{
    if( IsLeftAligmentAndNoAutoColWidthCell(grd, col, row) ) txtX = rect.left + 3;

	TCanvas &cnv = *grd->Canvas;
    if ( col<2 )
        cnv.Brush->Color  = grd->FixedColor;

    if ( row==0 )
    {
        cnv.Brush->Color  = (TColor)0x00DDDDDD;
        cnv.Font->Size  = grd->Font->Size+1;
    }

    if ( drawState & mygdHot )
        cnv.Font->Color  = clNavy;

    if ( col>1 && drawState & mygdSelected )
    {
        cnv.Font->Color  = clWhite;
        cnv.Brush->Color = clMenuHighlight;
    }
}
//------------------------------------------------------------------------------
void TFormAppSetsDlg::OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff)
{
    ff = !IsLeftAligmentAndNoAutoColWidthCell( grd, col, row );
}
//------------------------------------------------------------------------------
bool TFormAppSetsDlg::IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row )
{
    return true;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::Button1Click(TObject *Sender)
{
    using namespace AppSetsGrdItems::Row;
    const AnsiString sScale = grd1->Cells[2][Scale], sGas = grd1->Cells[2][Gas];

    const bool
        isCH = sGas=="CH4" || sGas=="C3H8",
        is4 = sScale=="0-4",
        is10 = sScale=="0-10",
        is20 = sScale=="0-20",
        is100 = sScale=="0-100",
        isOk = isCH && is100 || !isCH && ( is4 || is10 || is20 )
    ;
    if(!isOk) ::MessageBox(Handle, MYSPRINTF_(
        "Не допустимое сочетание для данного прибора: %s, шкала 0-%s",
            sGas, sScale).c_str(), "Некорректный ввод!", MB_OK | MB_APPLMODAL | MB_ICONERROR );
    else
	    ModalResult = mrOk;
}
//------------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::CheckListBox1ClickCheck(TObject *Sender)
{
	TCheckListBox& clb = *static_cast<TCheckListBox*>(Sender);
    if( clb.Checked[ clb.ItemIndex ] ) return;
	for( int i=0; i<clb.Items->Count; ++i )
    	if( clb.Checked[i] ) return;
    clb.OnClickCheck = NULL;
    clb.Checked[ clb.ItemIndex ] = true;
    clb.OnClickCheck = CheckListBox1ClickCheck;
}
//------------------------------------------------------------------------------

void __fastcall TFormAppSetsDlg::grd1SelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
    my::Grd::SetEditing(grd1, ARow>0 && ACol>1);
}
//---------------------------------------------------------------------------

void __fastcall TFormAppSetsDlg::grd1SetEditText(TObject *Sender, int ACol,
      int ARow, const AnsiString Value)
{
    changed_[ARow-1] = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::cbChanalKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    TWinControl *ctrl = dynamic_cast<TWinControl*>(Sender);
    TStringGrid* grd = StringgridControl::GetActiveGrid();
    const bool
        escapeKey = Key==VK_ESCAPE,
        enterKey  = Key==VK_RETURN
    ;

    if( escapeKey )
    {
        const TNotifyEvent onExit = GetCtrlOnExit( ctrl );
        SetCtrlOnExit( ctrl, NULL );
        ctrl->Visible = 0;
        SetCtrlOnExit( ctrl, onExit );
        grd->Cells[grd->Col][grd->Row] = StringgridControl::GetStoredValue(ctrl);

    }
    if( escapeKey || enterKey )
        ActiveControl = grd;
}
//---------------------------------------------------------------------------
void __fastcall TFormAppSetsDlg::RadioButton1Click(TObject *Sender)
{
    const int i = static_cast<TComponent*>(Sender)->Tag;
    AppSetsGrdItems::Row::SetGrp(grd1, i);
}
//---------------------------------------------------------------------------
