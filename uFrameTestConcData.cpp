//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uFrameTestConcData.h"
#include "uFrameGrid.h"
#include "SGrdHlpr.h"
#include "xmlUtils_.h"
#include "MySynchronize.h"
#include "vardef.hpp"
#include "htmlreport.h"
#include "AnsiStringUtils_.h"
#include "MyStrUtils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
void MakeGrdConcTitles1_9cols_8rows(TStringGrid *grd)
{
    using namespace DAK::Test::Row;
    // pgs, conc, conc_i, i, variac, porog1, porog2, difference
    grd->Cells[0][0]    = "Параметр";
    grd->Cells[0][1]    = name[pgs];
    grd->Cells[0][2]    = name[conc];
    grd->Cells[0][3]    = name[conc_i];
    grd->Cells[0][4]    = name[i];
    grd->Cells[0][5]    = name[variac];
    grd->Cells[0][6]    = name[porog1];
    grd->Cells[0][7]    = name[porog2];

    grd->Cells[1][0]    = "ПГС1";
    grd->Cells[2][0]    = "ПГС3";
    grd->Cells[3][0]    = "ПГС4";
    grd->Cells[4][0]    = "ПГС3";
    grd->Cells[5][0]    = "ПГС1";
    grd->Cells[6][0]    = "ПГС4";
    grd->Cells[7][0]    = "ПГС1";
    grd->Cells[8][0]    = "Макс. погр.";
}
//---------------------------------------------------------------------------
void MakeGrdConc_Postanovka(TStringGrid *grd)
{
    grd->ColCount = 11;
    grd->RowCount = 8;
    MakeGrdConcTitles1_9cols_8rows(grd);
    grd->Cells[9][0]    = "Кор.0";
    grd->Cells[10][0]   = "Кор.К";
}
void MakeGrdConc_Snjatie(TStringGrid *grd)
{
    grd->ColCount = 9;
    grd->RowCount = 9;
    MakeGrdConcTitles1_9cols_8rows(grd);
    grd->Cells[0][8]    = DAK::Test::Row::name[DAK::Test::Row::difference];
}

void MakeGrdTemperature(TStringGrid *grd)
{
    grd->ColCount = 21;
    grd->RowCount = 4;

    grd->Cells[0][0]    = "";
    grd->Cells[0][1]    = "\"0\"";
    grd->Cells[0][2]    = "середина";
    grd->Cells[0][3]    = "\"К\"";


    grd->Cells[1][0]    = "T'1(-)";
    grd->Cells[2][0]    = "Var1'1(-)";
    grd->Cells[3][0]    = "Конц'1(-)";
    grd->Cells[4][0]    = "Ilamp'1(-)";

    grd->Cells[5][0]    = "T'2(нку)";
    grd->Cells[6][0]    = "Var1'2(нку)";
    grd->Cells[7][0]    = "Конц'2(нку)";
    grd->Cells[8][0]    = "Ilamp'2(нку)";

    grd->Cells[9][0]    = "T'3(+)";
    grd->Cells[10][0]    = "Var1'3(+)";
    grd->Cells[11][0]    = "Конц'3(+)";
    grd->Cells[12][0]    = "Ilamp'3(+)";

    grd->Cells[13][0]    = "T'4(нку)";
    grd->Cells[14][0]    = "Var1'4(нку)";
    grd->Cells[15][0]    = "Конц'4(нку)";
    grd->Cells[16][0]    = "Ilamp'4(нку)";
	
	grd->Cells[17][0]    = "T'4(+)";
    grd->Cells[18][0]    = "Var1'4(+)";
    grd->Cells[19][0]    = "Конц'4(+)";
    grd->Cells[20][0]    = "Ilamp'4(+)";
}
//---------------------------------------------------------------------------
bool IsGrdRowNotEmpty(TStringGrid* grd, int ro)
{
    for( int cl=1; cl<grd->ColCount; ++cl )
        if(!grd->Cells[cl][ro].IsEmpty())
            return true;
    return false;

}
bool IsGrdEmpty(TStringGrid* grd)
{
    for( int ro=1; ro<grd->RowCount; ++ro )
        if( IsGrdRowNotEmpty(grd, ro) )
            return false;
    return true;
}
//---------------------------------------------------------------------------
TFrameTestConcData *FrameTestConcData;
//---------------------------------------------------------------------------
__fastcall TFrameTestConcData::TFrameTestConcData(TComponent* Owner, KindT kindArg)
    : TFrame(Owner), kind( kindArg )
{
    Name = "TFrameTestConcData"+IntToStr( (int) this );
    TFrameGrid *frmGrd = new TFrameGrid(this);
    grd = frmGrd->grd1;
    frmGrd->Align = alClient;
    frmGrd->Parent = this;

    if( kind==TEMPERATURE ) {
        MakeGrdTemperature(grd);
    }else if(kind==POSTANOVKA) {
        MakeGrdConc_Postanovka(grd);
    } else if(kind==SNJATIE)    {
        MakeGrdConc_Snjatie(grd);
    } else if(kind==STABILITY)    {
        grd->ColCount = 4;
        grd->RowCount = 5;
        grd->Cells[1][0] = "Макс.";
        grd->Cells[2][0] = "Мин.";
        grd->Cells[3][0] = "Разность";

        grd->Cells[0][0]    = "Параметр";
        grd->Cells[0][1]    = "Конц.ПГС";
        grd->Cells[0][2]    = "Конц.";
        grd->Cells[0][3]    = "Конц.ток";
        grd->Cells[0][4]    = "Ток,мА";
    }
    Height = my::Grd::GetActualRowsTotalHeight(grd)+Panel1->Height*2;
    grd->ColWidths[0] = 80;
}
//------------------------------------------------------------------------------
void TFrameTestConcData::Add2Report(TiXmlElement* body)
{
    if( !HasData() ) return;
    using namespace HTML;

    AddText( body, " ");
    AddText( body, lblDateTime->Caption.c_str(), TextAttr().Bold() );
    AddTag( body, "br");
    AddTag( body, "br");

    TableT tbl; ElemsT rows;
    const int cc = grd->ColCount, rc = grd->RowCount;
    TiXmlElement *tblElem =  AddTable( body, cc, rc, tbl, &rows);

    tblElem->SetAttribute("class", "table1");
    bool testResult = true;
    for(unsigned cl=0; cl<cc; ++cl)
        for(unsigned ro=0; ro<rc; ++ro) {
            const AnsiString
                s1 = grd->Cells[cl][ro],
                s =  MyReplace1(s1, "\\s*\\[[+\\-]\\]\\s*", "" );
            TextAttr ta;
            if(s1.Pos("[-]")) {
                testResult = false;
                ta = TextAttr().Bold().Color("red");
            }
            if(s.Pos("[+]")) ta = TextAttr().Bold().Color("blue");
            AddText( tbl[cl][ro], s.c_str(), ta );
        }
    AddTag( body, "br");
    AddText( body, "Результат проверки: " );

    TextAttr ta;
    ta.Bold();
    if(testResult)
        AddText( body, "соответствует требованиям документации", ta.Color("blue") );
    else
        AddText( body, "не соответствует требованиям документации", ta.Color("red") );
    AddTag( body, "br");
}
//------------------------------------------------------------------------------
bool TFrameTestConcData::HasData()
{
    return !IsGrdEmpty(grd);
}
//------------------------------------------------------------------------------
void TFrameTestConcData::Save(TiXmlElement* xml)
{
    if( !HasData() ) return;
    TiXMLAtr::SetStr(xml, "дата", lblDateTime->Caption.c_str() );
    TiXmlElement* elem = TiXML::ChildElement(xml, "таблица");
    TiXMLAtr::Set(elem, "столбцов", grd->ColCount );
    TiXMLAtr::Set(elem, "строк", grd->RowCount );
    for( int ro=1; ro<grd->RowCount; ++ro ) {
        if( !IsGrdRowNotEmpty(grd, ro) )
            continue;
        TiXmlElement* rowElem = TiXML::CreateChild(elem, "строка");
        TiXMLAtr::Set(rowElem, "n", ro );
        for( int cl=1; cl<grd->ColCount; ++cl ){
            const AnsiString s = grd->Cells[cl][ro];
            if(s.IsEmpty())
                continue;
            TiXmlElement* cellElem = TiXML::CreateChild(rowElem, "ячейка");
            TiXMLAtr::Set(cellElem, "n", cl );
            TiXMLText::SetStr( cellElem, s.c_str() );
        }
    }
}
//---------------------------------------------------------------------------
void TFrameTestConcData::Clear()
{
    lblDateTime->Caption = "";
    my::Grd::ClearTStringGrid(grd, 1, 1);
}
//---------------------------------------------------------------------------
void TFrameTestConcData::Load(TiXmlElement* xml)
{
    if(xml==0) {
        Clear();
        return;
    }

    lblDateTime->Caption = TiXMLAtr::GetStr(xml, "дата" ).c_str();
    my::Grd::ClearTStringGrid(grd,1,1);

    TiXmlElement* elem = xml->FirstChildElement();
    if(!elem) return;
    
    for( TiXmlElement* rowElem = elem->FirstChildElement(); rowElem;
        rowElem = rowElem->NextSiblingElement() ){
        const int ro = TiXMLAtr::Get(rowElem, "n", -1);
        if(ro<0 || ro>grd->RowCount-1)
            continue;
        for( TiXmlElement* cellElem = rowElem->FirstChildElement(); cellElem;
            cellElem = cellElem->NextSiblingElement() ){
            const int cl = TiXMLAtr::Get(cellElem, "n", -1);
            if(cl<0 || cl>grd->ColCount-1)
                continue;
            const AnsiString s = TiXMLText::GetStr( cellElem ).c_str();
            grd->Cells[cl][ro] = s;

        }
    }
}
//---------------------------------------------------------------------------
LRESULT TFrameTestConcData::Set1(const AnsiString &s, unsigned nCol, unsigned nRow)
{
    if(nCol<grd->ColCount && nRow<grd->RowCount)    {
        grd->Cells[nCol][nRow] = s;
        lblDateTime->Caption = FormatDateTime("dd mmmm yyyy, hh:nn", Now() );
    }
}
void TFrameTestConcData::Set(const AnsiString &s, unsigned nCol, unsigned nRow)
{
    my::Synchronize3( &Set1, s, nCol, nRow, __FILE_LINE__);
}
//---------------------------------------------------------------------------
AnsiString TFrameTestConcData::Get(unsigned nCol, unsigned nRow)
{
    if(nCol<grd->ColCount && nRow<grd->RowCount)
        return grd->Cells[nCol][nRow];
    else
        return "";
}
//---------------------------------------------------------------------------

