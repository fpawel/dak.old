//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uFrameDevicePasport.h"
#include "uFrameGrid.h"
#include "SGrdHlpr.h"
#include "vardef.hpp"
#include "uFrameTestConcData.h"

#include <math>
#include "xmlUtils_.h"
#include "htmlreport.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ThunderbirdTreeMain"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
void CreateThunderbirdTreeSection1(TThunderbirdTree* tree, TWinControl* ctrl, const AnsiString &name)
{
    TThunderbirdSection *sect = tree->AddSection(name);
    tree->AttachControl( sect, ctrl);
    //sect->Content->BevelInner = bvRaised;
    //sect->Content->BevelOuter = bvLowered;
}


//---------------------------------------------------------------------------
TFrameDevicePasport *FrameDevicePasport;
//---------------------------------------------------------------------------
__fastcall TFrameDevicePasport::TFrameDevicePasport(TComponent* Owner) : TFrame(Owner)
{
    Name = "FrameDevicePasport_"+IntToStr( (int) this );
    #define CCRTE__(nn,kk,ss) CreateFrameTestConcData(DAK::Test::##nn##, TFrameTestConcData::##kk##, ss)
    frmMainTest     = CCRTE__(main,         POSTANOVKA,    DAK_MAIN_TEST);
    frmTestStab     = CCRTE__(stability,    STABILITY,     "Проверка нестабильности показаний");
    frmTestPower11  = CCRTE__(power_11,     POSTANOVKA,    DAK_TEST_POWER", 11B");
    frmTestPower30  = CCRTE__(power_30,     SNJATIE,       DAK_TEST_POWER", 30B");
    frmTest20_1     = CCRTE__(tech_20_3,    POSTANOVKA,    DAK_TEST_TECHPROGON_20", постановка");
    frmTest20_2     = CCRTE__(tech_20_16,   SNJATIE,       DAK_TEST_TECHPROGON_20", снятие");
    frmTest78_1     = CCRTE__(tech_78_3,    POSTANOVKA,    DAK_TEST_TECHPROGON_78", постановка");
    frmTest78_2     = CCRTE__(tech_78_19,   SNJATIE,       DAK_TEST_TECHPROGON_78", снятие");

    frmTemperature  = CCRTE__(temperature,  TEMPERATURE,   "Компенсация влияния температуры");

    frmTest80_1     = CCRTE__(tech_80_3,    POSTANOVKA,    DAK_TEST_TECHPROGON_78" 2, постановка");
    frmTest80_2     = CCRTE__(tech_80_19,   SNJATIE,       DAK_TEST_TECHPROGON_78" 2, снятие");


    #undef CCRTE__
}
//---------------------------------------------------------------------------
void TFrameDevicePasport::Add2Report(TiXmlElement* body)
{
    using namespace HTML;
    for( unsigned n=0; n<DAK::Test::count; ++n ) {
        if( !frmTest[n]->HasData() ) continue;
        AddTag( body, "br");
        AddText( body, DAK::Test::name[n], TextAttr().Bold().Align("center") );
        
        frmTest[n]->Add2Report(body);
    }
}
//---------------------------------------------------------------------------
void TFrameDevicePasport::Save(TiXmlElement* xml)
{
    for( unsigned n=0; n<DAK::Test::count; ++n )
        if( frmTest[n]->HasData() )
            frmTest[n]->Save( TiXML::ChildElement(xml, DAK::Test::id[n]) );
}
//---------------------------------------------------------------------------
void TFrameDevicePasport::Load(TiXmlElement* xml)
{
    for( unsigned n=0; n<DAK::Test::count; ++n )

        if( TiXmlElement *elem = TiXmlHandle(xml).FirstChildElement(DAK::Test::id[n]).ToElement() )
            frmTest[n]->Load( elem );
        else
            frmTest[n]->Clear();
}
//---------------------------------------------------------------------------
TFrameTestConcData* TFrameDevicePasport::CreateFrameTestConcData(
    unsigned nTest,  TFrameTestConcData::KindT arg, const AnsiString &s)
{
    TFrameTestConcData* ret = new TFrameTestConcData(this,arg);
    ret->Align = alTop;
    ret->Parent = this;
    CreateThunderbirdTreeSection1(ThunderbirdTree1, ret, s);
    frmTest[nTest] = ret;
    return ret;
}
//---------------------------------------------------------------------------
void TFrameDevicePasport::UpdateMaxPogr( unsigned nTest,  unsigned nCol, unsigned nRow)
{
    using namespace DAK::Test::Col;
    using namespace DAK::Test::Row;
    const TFrameTestConcData::KindT knd = frmTest[nTest]->kind;
    const bool
        snajtKnd = knd==TFrameTestConcData::SNJATIE,
        postanKnd = knd==TFrameTestConcData::POSTANOVKA,
        notStabKind = snajtKnd || postanKnd,
        isLastPgs = nCol==pgs3_6+1,
        isFirstPgs = nCol==pgs1_1+1;
    if( notStabKind && isFirstPgs  ) {
        for(unsigned cl=pgs2_2+1; cl<=pgs1_7+1; ++cl)
            frmTest[nTest]->Set("", cl, nRow);
    }
    if( notStabKind && nCol==adjust0+1  )
            frmTest[nTest]->Set("", adjustS+1, nRow);

    if( notStabKind && isLastPgs && ( nRow==conc+1 || nRow==conc_i+1 || nRow==difference+1) ) {
        std::vector<double> pogrs;
        double d;
        for(unsigned n=0; n<=pgs3_6; ++n)
            if( DAK::Format::TryGetDelta(Get(nTest, n+1, nRow), d) )
                pogrs.push_back(d);
        AnsiString sPogr;
        if( !pogrs.empty() )
            sPogr = DAK::Format::RelPogr(*std::max_element(pogrs.begin(), pogrs.end()) );
        frmTest[nTest]->Set( sPogr, maxPogr+1, nRow );
    }
    if(snajtKnd && isLastPgs &&  nRow==conc+1) {
        for(unsigned n=0; n<=pgs3_6; ++n) {
            double v1, v2;
            if(     DAK::Format::TryGetValue1(Get(nTest, n+1, nRow), v1) &&
                    DAK::Format::TryGetValue1(Get(nTest-1, n+1, nRow), v2) )
                frmTest[nTest]->Set( DAK::Format::DeltaConc(std::fabs(v1-v2), n),
                    n+1, difference+1);
        }
    }
}
//------------------------------------------------------------------------------
void TFrameDevicePasport::Set(const AnsiString &s, unsigned nTest,  unsigned nCol, unsigned nRow)
{

    if(nTest<DAK::Test::count) {
        frmTest[nTest]->Set(s, nCol, nRow);
        using namespace DAK::Test::Col;
        using namespace DAK::Test::Row;
        if(nCol==variac2+1 && nRow==conc+1)
            frmTest[nTest]->Set( DAK::Format::Variac2(Get(nTest,
                variac1+1, nRow), s), nCol, variac+1 );
        UpdateMaxPogr( nTest,  nCol, nRow);
    }
}
//------------------------------------------------------------------------------
AnsiString TFrameDevicePasport::Get(unsigned nTest, unsigned nCol, unsigned nRow)
{
    if(nTest<DAK::Test::count)
        return frmTest[nTest]->Get(nCol, nRow);
    else
        return "";
}
//------------------------------------------------------------------------------
