//---------------------------------------------------------------------------

#ifndef uFormPneumoGrdH
#define uFormPneumoGrdH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#include "DrawGridCell.h"
#include "pneumo_gas.hpp"
#include "xmlUtils_.h"

//------------------------------------------------------------------------------
typedef void (__closure *TNotifyPneumoGasEvent)(unsigned nGas);
//------------------------------------------------------------------------------
class TFormPneumoGrd : public TForm
{
__published:	// IDE-managed Components
    TTimer *Timer1;
    TStringGrid *grdPneumo;
    void __fastcall grdPneumoSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
    void __fastcall grdPneumoMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall grdPneumoKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
    my::DrawGridCellManager drwGrdPneumo;

    void AfterSetCanvasGrd( TStringGrid* grd, int col, int row,
		MyGridCellState drawState, TRect rect, int& txtX, int& txtY, bool& );
    void OnDeterminateColumnWidth( TStringGrid* grd, int col, int, bool& ff);
    bool IsLeftAligmentAndNoAutoColWidthCell( TStringGrid* grd, int col, int row );

    const Pneumo::Gases& gases_;
    const TNotifyPneumoGasEvent onUserSelectPneumoGas_;
    TiXmlElement *xml_;

public:		// User declarations
    __fastcall TFormPneumoGrd(TComponent* Owner, const Pneumo::Gases& gases,
        TNotifyPneumoGasEvent onUserSelectPneumoGas);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPneumoGrd *FormPneumoGrd;
//---------------------------------------------------------------------------
#endif
