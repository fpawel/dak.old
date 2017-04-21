//---------------------------------------------------------------------------
#ifndef uFrameTestConcDataH
#define uFrameTestConcDataH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include "tinyxml.h"
//------------------------------------------------------------------------------
class TFrameTestConcData : public TFrame
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TLabel *Label1;
    TLabel *lblDateTime;
private:	// User declarations
    LRESULT Set1(const AnsiString &s, unsigned nCol, unsigned nRow);
    TStringGrid *grd;
public:		// User declarations
    typedef enum
    {
        POSTANOVKA,
        SNJATIE,
        STABILITY,
        TEMPERATURE
    } KindT;
    __fastcall TFrameTestConcData(TComponent* Owner, KindT kind);

    const KindT kind;
    void Save(TiXmlElement* xml);
    void Load(TiXmlElement* xml);
    void Clear();

    void Set(const AnsiString &s, unsigned nCol, unsigned nRow);
    AnsiString Get(unsigned nCol, unsigned nRow);
    bool HasData();
    bool Result();

    void Add2Report(TiXmlElement* report);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameTestConcData *FrameTestConcData;
//---------------------------------------------------------------------------
#endif
