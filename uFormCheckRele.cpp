//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#pragma hdrstop

#include "uFormCheckRele.h"

//my
#include "DrawGridCell.h"
#include "SGrdHlpr.h"
#include "AnsiStringUtils_.h"
#include "bcd_.h"
#include "my_array_size.h"

//#include "Unit1.h"

//vcl
#include <Math.hpp>
#include <FileCtrl.hpp>
#include <dialogs.hpp>
#include "MyVclUtils_.h"
#include "tools.h"

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <list>



//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormCheckRele *FormCheckRele;

//---------------------------------------------------------------------------
#define INI_SEKT "Диалог_настроек_проверки_пинов_реле"


//---------------------------------------------------------------------------
__fastcall TFormCheckRele::TFormCheckRele(TComponent* Owner)
	: TForm(Owner)
{
    using namespace DAK::RelePin;
    lv1->Items->Clear();
    for(int i=0; i<count; ++i)
    {
        TListItem *lItem = lv1->Items->Add();
        lItem->Caption = cpt[i];
        lItem->SubItems->Add("");
    }
}
//---------------------------------------------------------------------------
__fastcall TFormCheckRele::~TFormCheckRele()
{
}
//------------------------------------------------------------------------------
void __fastcall TFormCheckRele::HandleWMKeyDown(TWMKey& msg)
{
    if(msg.CharCode==VK_RETURN)
        ModalResult = mrOk;
    else if(msg.CharCode==VK_ESCAPE	)
        ModalResult = mrCancel;
}

//------------------------------------------------------------------------------
void __fastcall TFormCheckRele::Button1Click(TObject *Sender)
{
	ModalResult = mrOk;
}
//------------------------------------------------------------------------------
void __fastcall TFormCheckRele::Button2Click(TObject *Sender)
{
	//std::fill( prmToCtrl_, prmToCtrl_+HROMAT_NUMBER_PARAM_COUNT, (TWinControl*)NULL );
	ModalResult = mrCancel;
}
//------------------------------------------------------------------------------
void __fastcall TFormCheckRele::PopupMenu1Popup(TObject *Sender)
{
    // TLabel *lbl = static_cast<TLabel *>( PopupMenu1->PopupComponent );
    for( int i=0; i<PopupMenu1->Items->Count; ++i )
    {
        TMenuItem *itm = PopupMenu1->Items->Items[i];
        itm->Checked = lv1->Selected && lv1->Selected->Caption==itm->Caption;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormCheckRele::N3Click(TObject *Sender)
{
    TListItem *itm0 = lv1->Selected;
    if( !itm0 ) return;
    const AnsiString s = static_cast<TMenuItem*>(Sender)->Caption;

    for( int i=0; i<lv1->SelCount; ++i )
        lv1->Items->Item[itm0->Index+i]->SubItems->Strings[0] = s;
}
//---------------------------------------------------------------------------
void __fastcall TFormCheckRele::lv1DblClick(TObject *Sender)
{
    ActivatePopupMenu(PopupMenu1);
}
//---------------------------------------------------------------------------



