//------------------------------------------------------------------------------
#ifndef MyRichEditH
#define MyRichEditH
//------------------------------------------------------------------------------
#include <ComCtrls.hpp>
#include <vector>
//------------------------------------------------------------------------------
struct RichTextAttr
{
    RichTextAttr()
    {
        h.continueLine=h.textColor=h.fontStyles=h.textSize=h.time = false;
    }
    RichTextAttr(const RichTextAttr& lhs)
    {
        swap(lhs);
    }
    RichTextAttr operator=(const RichTextAttr& lhs)
    {
        swap(lhs);
        return *this;
    }

    void swap(const RichTextAttr& lhs)
    {
        textColor           = lhs.textColor;
        fontStyles          = lhs.fontStyles;
        textSize            = lhs.textSize;
        h.continueLine           = lhs.h.continueLine;
        h.textColor         = lhs.h.textColor;
        h.fontStyles        = lhs.h.fontStyles;
        h.textSize          = lhs.h.textSize;
        h.time              = lhs.h.time;
    }

    RichTextAttr TextColor(TColor arg)
    {
        textColor = arg;
        h.textColor = true;
        return *this;
    }
    RichTextAttr FontStyles(TFontStyles arg)
    {
        fontStyles = arg;
        h.fontStyles = true;
        return *this;
    }
    RichTextAttr ContinueLine()
    {
        h.continueLine = true;
        return *this;
    }
    RichTextAttr TextSize(unsigned arg)
    {
        textSize = arg;
        h.textSize = true;
        return *this;
    }
    RichTextAttr Time()
    {
        h.time = true;
        return *this;
    }
    TColor textColor;
    TFontStyles fontStyles;
    unsigned textSize;
    struct
    {
        bool continueLine, textColor, fontStyles, textSize, time;
    } h;
};
//------------------------------------------------------------------------------
void AddRicheditText( TRichEdit* re, const AnsiString& s, const RichTextAttr&);

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
