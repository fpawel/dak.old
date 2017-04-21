//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uFrameGrid.h"
#include "MouseEvt.h"
#include "SGrdHlpr.h"
#include "vardef.hpp"
//---------------------------------------------------------------------------
#include "boost/regex.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"



TColor ComputeFontColor1( const AnsiString& s)
{
    if( s.Pos("[-]") )
        return clRed;
    else if( s.Pos("[+]") ) {
        return clBlue;
    } else
        return clBlack;
}
//------------------------------------------------------------------------------
//TFrameDropDownPanel *FrameDropDownPanel;
//------------------------------------------------------------------------------
__fastcall TFrameGrid::TFrameGrid(TComponent* Owner	)
	: TFrame(Owner), drwGrd(grd1)
{
    Name = "GridPanel_"+IntToStr( (int) this ) + "_" + IntToStr( (int) grd1 );
    drwGrd.SetAfterSetCanvasFoo( &AfterSetCanvasGrd );
    drwGrd.SetOnDeterminateColumnWidthFoo( &OnDeterminateColumnWidth );
    drwGrd.SetOnOnGetCellText( &OnGetCellText );
    grd1->Cells[0][0] = "Параметр";

    my::Grd::AddCopyItemToPopupMenu(grd1);
    my::Grd::AddPastItemToPopupMenu(grd1);
}
//------------------------------------------------------------------------------
unsigned TFrameGrid::GridHeight()
{
    return my::Grd::GetActualRowsTotalHeight(grd1) + 10;
}
//------------------------------------------------------------------------------
void TFrameGrid::AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
	MyGridCellState drawState, TRect rect, int& txtX, int& txtY, bool& wrapText )
{
    if( IsLeftAligmentAndNoAutoColWidthCell(grd, col, row) ) txtX = rect.left + 3;
	TCanvas &cnv = *grd->Canvas;
    const AnsiString cellText = grd->Cells[col][row];

    cnv.Font->Color = ComputeFontColor1( cellText );

    if ( col==0 || row==0 )
        cnv.Brush->Color  = grd->FixedColor;
    if ( drawState & mygdSelected ) {
        cnv.Font->Color  = clWhite;
        cnv.Brush->Color = clMenuHighlight;
    }
}
//------------------------------------------------------------------------------
void TFrameGrid::OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff)
{
    ff = !IsLeftAligmentAndNoAutoColWidthCell( grd, col, row );
}
//------------------------------------------------------------------------------
bool TFrameGrid::IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row )
{
    return true;
}
//------------------------------------------------------------------------------
void TFrameGrid::OnGetCellText( TStringGrid* grd, int col, int row, AnsiString& val )
{
    if(col==0 || row==0) return;
    val = DAK::Format::ConvertLongConc2Short(val);
}
//------------------------------------------------------------------------------
void __fastcall TFrameGrid::grd1SelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
    lblTtlCell->Caption = grd1->Cells[0][ARow]+", "+grd1->Cells[ACol][0]+" = ";
    lblGrdCell->Caption = grd1->Cells[ACol][ARow];
    lblGrdCell->Font->Color = ComputeFontColor1( grd1->Cells[ACol][ARow] );
}
//---------------------------------------------------------------------------

