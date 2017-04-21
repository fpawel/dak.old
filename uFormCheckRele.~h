//---------------------------------------------------------------------------

#ifndef uFormCheckReleH
#define uFormCheckReleH
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
#include <Buttons.hpp>
#include <CheckLst.hpp>
#include <Menus.hpp>
//------------------------------------------------------------------------------





//------------------------------------------------------------------------------
#include <vector>
class TFormCheckRele : public TForm
{
__published:	// IDE-managed Components
    TPopupMenu *PopupMenu1;
    TMenuItem *N1;
    TMenuItem *N2;
    TMenuItem *N3;
    TButton *Button1;
    TButton *Button2;
    TListView *lv1;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
    void __fastcall PopupMenu1Popup(TObject *Sender);
    void __fastcall N3Click(TObject *Sender);
    void __fastcall lv1DblClick(TObject *Sender);
private:	// User declarations
    void __fastcall HandleWMKeyDown(TWMKey& msg);
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(CM_DIALOGKEY,TWMKey,HandleWMKeyDown)
    END_MESSAGE_MAP(TForm);

public:		// User declarations
	__fastcall TFormCheckRele(TComponent* Owner);
    virtual __fastcall ~TFormCheckRele();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormCheckRele *FormCheckRele;
//---------------------------------------------------------------------------
#endif
