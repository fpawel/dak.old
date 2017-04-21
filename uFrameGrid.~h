//---------------------------------------------------------------------------
#ifndef uFrameGridH
#define uFrameGridH
//---------------------------------------------------------------------------
#include "DrawGridCell.h"
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <set>
class TFrameGrid : public TFrame
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TLabel *lblGrdCell;
    TLabel *lblTtlCell;
    TStringGrid *grd1;
    void __fastcall grd1SelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
private:	// User declarations
    void OnMouseOcurence(TWinControl* ctrl, bool isEntered);

    void AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
		MyGridCellState drawState, TRect rect, int& txtX, int& txtY, bool& wrapText );
    void OnDeterminateColumnWidth( TStringGrid* grd, int col, int row, bool& ff);
    bool IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row );
    void OnGetCellText( TStringGrid* grd, int col, int row, AnsiString& val );

public:		// User declarations
	__fastcall TFrameGrid(TComponent* Owner);
    my::DrawGridCellManager drwGrd;
    std::set<int> LeftAlignCols;
    unsigned GridHeight();
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameGrid *FrameGrid;
//---------------------------------------------------------------------------
#endif
