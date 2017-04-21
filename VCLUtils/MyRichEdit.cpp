//------------------------------------------------------------------------------
#pragma hdrstop
#include "MyRichEdit.h"
#include "MySynchronize.h"
#include "MyFileCtrl.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
void Move2RichEditPreviousLine( TRichEdit* re )
{
    re->CaretPos = TPoint( 0, re->CaretPos.y-1);
    re->Perform( WM_KEYDOWN, VK_BACK, 0 );
    re->Perform( WM_KEYUP, VK_BACK, 0 );
    re->SelStart = 0xFFFFFFFF;
}
//------------------------------------------------------------------------------
LRESULT AddRicheditTextAddLog1( TRichEdit* re, const AnsiString& s, const RichTextAttr& atrs)
{
    TTextAttributes &textAtrs = *re->SelAttributes;

    if(atrs.h.time) {
        textAtrs.Color = clGreen;
        textAtrs.Size = 8;
        re->Lines->Add("<"+FormatDateTime("hh:nn:ss", Time() )+">  ");
    }
    
    if(atrs.h.textColor)
        textAtrs.Color = atrs.textColor;
    if(atrs.h.fontStyles)
        textAtrs.Style = atrs.fontStyles;
    if(atrs.h.textSize)
        textAtrs.Size = atrs.textSize;
    re->Lines->Add(s);
    if(atrs.h.continueLine || atrs.h.time)
        Move2RichEditPreviousLine( re );
    return 0;
}
//------------------------------------------------------------------------------
void AddRicheditText( TRichEdit* re, const AnsiString& s, const RichTextAttr& atrs)
{
    my::Synchronize3( &AddRicheditTextAddLog1, re, s, atrs, __FILE_LINE__ );
}
//------------------------------------------------------------------------------

