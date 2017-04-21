//---------------------------------------------------------------------------


#ifndef uFrameDevicePasportH
#define uFrameDevicePasportH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ThunderbirdTreeMain.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
#include "uFrameTestConcData.h"
#include "vardef.hpp"
#include "tinyxml.h"
class TFrameDevicePasport : public TFrame
{
__published:	// IDE-managed Components
    TThunderbirdTree *ThunderbirdTree1;
private:	// User declarations
    TFrameTestConcData* CreateFrameTestConcData(unsigned nTest, TFrameTestConcData::KindT, const AnsiString &s);
    TFrameTestConcData
        *frmMainTest, *frmTestPower11,  *frmTestPower30,
        *frmTest20_1, *frmTest20_2,     *frmTest78_1,
        *frmTest78_2,

        *frmTest80_1,
        *frmTest80_2,

        *frmTestStab, *frmTemperature,
        *frmTest[DAK::Test::count]
        ;
    void UpdateMaxPogr( unsigned nTest,  unsigned nCol, unsigned nRow);
public:		// User declarations
    __fastcall TFrameDevicePasport(TComponent* Owner);
    void Set(const AnsiString &s, unsigned nTest,  unsigned nCol, unsigned nRow);
    AnsiString Get(unsigned nTest, unsigned nCol, unsigned nRow);

    void Save(TiXmlElement* xml);
    void Load(TiXmlElement* xml);

    void Add2Report(TiXmlElement* report);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameDevicePasport *FrameDevicePasport;
//---------------------------------------------------------------------------
#endif
