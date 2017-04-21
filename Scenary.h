//------------------------------------------------------------------------------
#ifndef ScenaryH
#define ScenaryH
//------------------------------------------------------------------------------
#include <grids.hpp>
#include <ComCtrls.hpp>
//------------------------------------------------------------------------------
namespace Scenary
{
    struct InitT
    {
        TStringGrid *grdScnList;
        TRichEdit* scnLog;
    };
    void Initialize(InitT);

    void FormatLog( TColor textColor, TFontStyles fontStyles = TFontStyles(), unsigned textSize=10 );
    void AddLog( const AnsiString& s, bool isNewLine=true );
    void LogScenaryError( const AnsiString& s );
    void LogPerformedAddy();
    void LogTime();

};
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
