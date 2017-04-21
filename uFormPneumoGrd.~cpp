//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uFormPneumoGrd.h"
#include "SGrdHlpr.h"
#include "AnsiStringUtils_.h"
#include "MyVclUtils_.h"
#include "AppXML.h"
#include "xmlUtils_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//------------------------------------------------------------------------------
TFormPneumoGrd *FormPneumoGrd;
//------------------------------------------------------------------------------
__fastcall TFormPneumoGrd::TFormPneumoGrd(TComponent* Owner, const Pneumo::Gases& gases,
    TNotifyPneumoGasEvent onUserSelectPneumoGas)
    : TForm(Owner), drwGrdPneumo( grdPneumo ), gases_(gases),
    onUserSelectPneumoGas_(onUserSelectPneumoGas),
    xml_( GetAppXML().pneumoBlock )
{
    const unsigned gasCount = gases_.size();

	grdPneumo->RowCount = gasCount + 1;
    grdPneumo->ColCount = 6;
    if(gasCount>0) grdPneumo->FixedRows = 1;

    grdPneumo->Cells[0][0] = "№";
    grdPneumo->Cells[1][0] = "Газ";
    grdPneumo->Cells[2][0] = "Концентрация";
    grdPneumo->Cells[3][0] = "Примечание";
    grdPneumo->Cells[4][0] = "Старт";
    grdPneumo->Cells[5][0] = "Длительность";
    for( unsigned i=0; i<gasCount; ++i )
    {
    	grdPneumo->Cells[0][i+1] = AnsiString().sprintf( "%d.", i+1);
        grdPneumo->Cells[1][i+1] = gases_[i].name;
        grdPneumo->Cells[2][i+1] = gases_[i].conc;
        grdPneumo->Cells[3][i+1] = gases_[i].caption;
    }
    drwGrdPneumo.SetAfterSetCanvasFoo( &AfterSetCanvasGrd );
    drwGrdPneumo.SetOnDeterminateColumnWidthFoo(&OnDeterminateColumnWidth);
    my::Grd::SetColWidthsAsStr( grdPneumo, "10 50 50 50 50 50" );    
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void TFormPneumoGrd::AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	MyGridCellState drawState, TRect rect, int& txtX, int& txtY, bool& )
{
	TCanvas &cnv = *grd->Canvas;

    if ( drawState & mygdHot )
    {
        cnv.Font->Color  = clNavy;
    }

    if ( drawState & mygdSelected )
    {
        cnv.Brush->Color = (TColor)0xB9FDF8;
        cnv.Font->Color  = clNavy;
    }

    if( IsLeftAligmentAndNoAutoColWidthCell(grd, col, row) ) txtX = rect.left + 3;
}
//------------------------------------------------------------------------------
void TFormPneumoGrd::OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff)
{
    ff = !IsLeftAligmentAndNoAutoColWidthCell(grd, col, row);
}
//------------------------------------------------------------------------------
bool TFormPneumoGrd::IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row )
{
	return grd==grdPneumo && col>0;
}
//---------------------------------------------------------------------------
void __fastcall TFormPneumoGrd::grdPneumoSelectCell(TObject *Sender,
      int ACol, int ARow, bool &CanSelect)
{
    TStringGrid* grd = static_cast<TStringGrid*>(Sender);
	my::Grd::SetEditing(grd, grd==grdPneumo && ARow>0 && (ACol==2 || ACol==6) );

}
//---------------------------------------------------------------------------


void __fastcall TFormPneumoGrd::grdPneumoMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    if( !Shift.Contains(ssLeft) ) return;
    TStringGrid *grd = static_cast<TStringGrid*>(Sender);
    TGridRect sel = grd->Selection;

    int col,row;
    TPoint pt;
    GetCursorPos(&pt);
    pt = grd->ScreenToClient(pt);
	grd->MouseToCell(pt.x, pt.y, col, row);
    if( col< sel.Left || row<sel.Top ) return;

    sel.Right=col;
    sel.Bottom=row;
    grd->Selection = sel;
}
//---------------------------------------------------------------------------

void __fastcall TFormPneumoGrd::grdPneumoKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if( Screen->ActiveControl!=grdPneumo || Key!=VK_ESCAPE ) return;
    onUserSelectPneumoGas_(grdPneumo->Row-1);
}
//---------------------------------------------------------------------------
