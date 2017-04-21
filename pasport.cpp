//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "pasport.h"
//------------------------------------------------------------------------------
#include "boost\shared_ptr.hpp"
//------------------------------------------------------------------------------
#include <math>
#include <dateutils.hpp>
//------------------------------------------------------------------------------
#include "htmlreport.h"
#include "MyStrUtils.h"
#include "Devices.h"
#include "vardef.hpp"
#include "mymodfn.hpp"
#include "AnsiStringUtils_.h"
#include "MyFileCtrl.h"
#include "ctrlsys.h"
#include "uFrmWait.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
#define MAIN_KEF_VAL_(ss) Devs::GetKef(nDev, MAIN_KEF_N_(ss) ).c_str()
#define MAIN_KEF_KEY_(ss) MAIN_KEF_(ss).Val2Key( Devs::GetKef(nDev, MAIN_KEF_N_(ss) ) ).c_str()
//------------------------------------------------------------------------------
/*
void AddTestResult(TiXmlElement *body, unsigned addy, unsigned nTest)
{
    using namespace HTML;
    const unsigned nDev = Devs::IndexOfAddy(addy);
    if(nDev>=Devs::Count()) return;
    const DAK::Test::DataT& testRes = Devs::Test::GetTestResult(addy);
    const DAK::Test::DataT& dt = Devs::Test::GetTestResult(addy);
    TableT tbl;
    ElemsT rows;

    AddText( body, MYSPRINTF_("%d. %s", nTest+1, DAK::Test::name[nTest]).c_str(), TextAttr().Bold() );
    TiXmlElement*
    tblElem =  AddTable( body, 8, 4, tbl, &rows);
    tblElem->SetAttribute("class", "table1");
    AddText( tbl[0][1], "Ток, мА" );
    AddText( tbl[0][2], "Конц. ток" );
    AddText( tbl[0][3], "Конц." );
    const char *sPGS[7] = { "ПГС1", "ПГС2", "ПГС3", "ПГС2", "ПГС1", "ПГС3", "ПГС1" };
    for(unsigned nPGS=0; nPGS<7; ++nPGS) {
        AddText( tbl[nPGS+1][0], sPGS[nPGS] );
        const DAK::Test::IndicationT& itm = testRes.test[nTest].r[nPGS];
        TextAttr atr1 = TextAttr().Color( itm.conc_i.match ? "navy" : "red" );
        TextAttr atr2 = TextAttr().Color( itm.conc.match ? "navy" : "red" );
        AddText( tbl[nPGS+1][1], Float2Str(itm.conc_i.i),   atr1);
        AddText( tbl[nPGS+1][2], Float2Str(itm.conc_i.val), atr1 );
        AddText( tbl[nPGS+1][3], Float2Str(itm.conc.val),   atr2);
    }
    const double B = std::fabs( testRes.test[nTest].r[1].conc.val-testRes.test[nTest].r[3].conc.val );
    AddText( body, "Вариация показаний "+Float2Str(B) );

}
*/
//------------------------------------------------------------------------------
void CreatePasport(unsigned addy, bool openIt)
{
    const unsigned nDev = Devs::IndexOfAddy(addy);
    if(nDev>=Devs::Count()) return;

    const char cssTemplateFileName[] = "template.css";

    using namespace HTML;
    TiXmlElement *body, *head;
    boost::shared_ptr<TiXmlDocument> doc( HTML::CreateDocument("Паспорт ДАК-М", body, head ) );
    AddStyleSheet( head, cssTemplateFileName);

    TextAttr bold1 = TextAttr().Bold();
    AddText( body, "Паспорт ДАК-М", TextAttr().Class("header1") );   

    TableT tbl;
    ElemsT rows;
    TiXmlElement* tblElem = AddTable(body, 4, 4, tbl, &rows);
    tblElem->SetAttribute("class","table3");

    AddText( tbl[0][0], "Заводской номер" );
    AddText( tbl[0][1], "Исполнение");
    AddText( tbl[0][2], "Тип газа");
    AddText( tbl[0][3], "Шкала");

    const AnsiString sSerial = Devs::Serial(nDev);

    AddText( tbl[1][0], sSerial.c_str(),        bold1 );
    AddText( tbl[1][1], MAIN_KEF_VAL_(Isp),     bold1 );
    AddText( tbl[1][2], MAIN_KEF_KEY_(Gas),     bold1 );
    AddText( tbl[1][3], std::string( MAIN_KEF_KEY_(Scale))+" "+MAIN_KEF_KEY_(Units),   bold1 );

    AddText( tbl[2][0], "Адрес MODBUS" );
    std::string sAddy = Int2Str(addy,16)+std::string("h");
    AddText( tbl[3][0], sAddy,  bold1 );

    AddText( tbl[2][1], "Адрес MODBUS датчика" );
    AddText( tbl[3][1], Devs::GetSensorSlave(nDev).c_str(),  bold1 );

    Devs::GetFramePaspOfAddy(addy)->Add2Report( body );



    Word year, month, day, hour, minute, second, milliSecond;
    DecodeDateTime( Now(), year, month, day, hour, minute, second, milliSecond);
    const AnsiString scnReportDir = MYSPRINTF_(
        "%s\\Отчёты\\%d\\%s\\Прибор_%s\\", MyGetExePath().c_str(),
            year, MyFormatMonth(month), sSerial ),
        reportFileName = MYSPRINTF_( "%s_%d_%d_%d_%d_%d.html",
            scnReportDir, day, hour, minute, second, milliSecond );

    ForceDirectories(scnReportDir);
    doc->SaveFile( reportFileName.c_str() );
    MyExtractFileFromResource( scnReportDir+cssTemplateFileName, "DAK_REPORT_CSS_TEMPLATE_FILE", false );

    if(openIt)
        ::ShellExecute( Screen->ActiveForm->Handle, "open", reportFileName.c_str(), "",
            scnReportDir.c_str(), SW_SHOWMAXIMIZED );
}

void CreatePasports( bool showInfoPanel, bool openIt )
{
    boost::shared_ptr<TFrmWait> waitFrm;
    if(showInfoPanel) {
        waitFrm.reset( new TFrmWait(NULL) );
        waitFrm->Panel2->Caption = "Формирование паспортов";
        waitFrm->Panel2->Font->Size = 10;
        waitFrm->FixPosition();
        delete waitFrm->Panel1;
        waitFrm->Show();
        waitFrm->Repaint();
    }
    
    const Devs::Nums addy = Devs::GetSelectedAddys();
        for( unsigned i=0; i<addy.size(); ++i )
            CreatePasport(addy[i], openIt);
}

