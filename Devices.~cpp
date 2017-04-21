//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include <cassert>
#include <math>
//------------------------------------------------------------------------------
#include <Dialogs.hpp>
#include <DateUtils.hpp>
//------------------------------------------------------------------------------
#include "Devices.h"
#include "vardef.hpp"
#include "FooCalledOnce.hpp"
#include "SGrdHlpr.h"
#include "xmlUtils_.h"
#include "listViewHelpr_.h"
#include "ctrlsys.h"
#include "TransferManage.h"
#include "vardef.hpp"
#include "MySynchronize.h"
#include "AnsiStringUtils_.h"
#include "MyStrUtils.h"
#include "TransferManage.h"
#include "scriptTask.h"
#include "pasport.h"
#include "Unit1.h"
#include "MyModFN.hpp"
#include "uFrameGraph.h"
#include "modbus_.h"

//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
namespace Devs
{
//------------------------------------------------------------------------------
TStringGrid *grdVars, *grdScnList, *grdKfs;
TListView *lvDevs;
TPageControl *pgCtRep, *pgPasp, *pgChart;
//------------------------------------------------------------------------------
typedef boost::shared_ptr<TTabSheet> PTabSheet;
typedef boost::shared_ptr<TRichEdit> PRichEdit;
std::vector<PTabSheet > repTab;
std::vector<PTabSheet > paspTab;
std::vector<PTabSheet > chartTab;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
typedef std::pair<TStringGrid*, int> GrdDevcol0;
enum    {    GRDS_COUNT = 3};
GrdDevcol0 grdDvs[GRDS_COUNT];
//------------------------------------------------------------------------------
void UpdateSelectDevice(unsigned n );
//------------------------------------------------------------------------------
void LoadKefsList()
{
	const DAK::Kefs& lst = DAK::GetKefs();
    const unsigned sz = lst.size();
    grdKfs->RowCount = sz + 1;
    for( unsigned i=0; i<sz; ++i )
    {
    	const DAK::Kef &var = lst[i];
    	const int ro = i+1;

        grdKfs->Cells[0][ro] = var.n;
        grdKfs->Cells[1][ro] = var.caption;
        grdKfs->Cells[2][ro] = var.def;
    }
}
//------------------------------------------------------------------------------
struct Helper
{
    //void OnTransferManagerReport(unsigned context, const AnsiString& state);
    // void HandleStopOrStartThread(const AnsiString& state, bool isStartThread);
    void __fastcall lvDevsClick(TObject *Sender);
    void __fastcall lvDevsCustomDrawItem(TCustomListView *Sender,
      TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
    void __fastcall lvDevsCustomDrawSubItem(TCustomListView *Sender,
      TListItem *Item, int SubItem, TCustomDrawState State,
      bool &DefaultDraw);
    void __fastcall lvDevsKeyPress(TObject *Sender, char &Key);
    //PVoid connectOnTransferManagerReport_;
};
//------------------------------------------------------------------------------
Helper helper;
//------------------------------------------------------------------------------
LRESULT AddModbusFloatValueToChart(unsigned addy, unsigned varnum, double value);

void onReadModbusFloatValue(unsigned addr, unsigned varnum, double value){
    my::Synchronize3( AddModbusFloatValueToChart, addr, varnum, value, __FILE_LINE__);

}

void Initialize(InitT init)
{
    ASSERT_FUNCTION_CALLED_ONCE__;

    SetOnReadModbusFloatValue(onReadModbusFloatValue);

    //helper.connectOnTransferManagerReport_ = TransferManager().Instance(). ConnectOnReportState(&helper.OnTransferManagerReport);
    grdVars = init.grdVars;
    grdScnList = init.grdScnList;
    grdKfs = init.grdKfs;
    lvDevs = init.lvDevs;
    pgCtRep = init.pgCtRep;
    pgPasp = init.pgPasp;
    pgChart = init.pgChart;
    lvDevs->OnClick = helper.lvDevsClick;
    lvDevs->OnCustomDrawItem = helper.lvDevsCustomDrawItem;
    lvDevs->OnCustomDrawSubItem = helper.lvDevsCustomDrawSubItem;
    lvDevs->OnKeyPress = helper.lvDevsKeyPress;

    grdVars->RowCount = DAK::Vars::count+1;
    grdVars->Cells[0][0] = "№";

    my::Grd::SetCol( grdVars, 0, DAK::Vars::cpt, DAK::Vars::cpt+DAK::Vars::count, 1);

    grdVars->FixedCols =1;
    grdVars->FixedRows =1;
    grdVars->ColCount = Devs::Count() + 1;

    grdScnList->Cells[0][0] = "№";
    grdScnList->Cells[1][0] = "Старт";
    grdScnList->Cells[2][0] = "Длительность";
    grdScnList->Cells[3][0] = "Операция";
    grdScnList->Cells[4][0] = "Состояние";
    grdScnList->ColWidths[0] = 5;
    grdScnList->ColWidths[1] = 70;
    grdScnList->ColWidths[2] = 70;
    grdScnList->ColWidths[3] = 300;
    grdScnList->ColWidths[4] = 100;

    grdKfs->RowCount = 1;
    grdKfs->ColCount = Devs::Count() + 3;
    grdKfs->Cells[0][0] = "№";
    grdKfs->Cells[1][0] = "Наименование";
    grdKfs->Cells[2][0] = "По умолчанию";

    LoadKefsList();
    if( grdKfs->RowCount>1 )grdKfs->FixedRows = 1;
    grdKfs->FixedCols = 1;
    grdKfs->ColWidths[1] = 100;
    grdKfs->ColWidths[2] = 50;

    grdDvs[0] = GrdDevcol0(grdVars, 1);
    grdDvs[1] = GrdDevcol0(grdScnList, 5);
    grdDvs[2] = GrdDevcol0(grdKfs, 3);

}
//------------------------------------------------------------------------------
void Finalize()
{
    repTab.clear();
    paspTab.clear();
    chartTab.clear();
    //helper.connectOnTransferManagerReport_.reset();
}
//------------------------------------------------------------------------------
TRichEdit* GetLog(unsigned nDev)
{
    if(!(nDev<Count())) return NULL;
    return static_cast<TRichEdit*>( repTab[nDev]->Controls[0] );
}
TRichEdit* GetLogOfAddy(unsigned addy)
{
    int nDev = IndexOfAddy(addy);
    if(nDev==-1) return NULL;
    return GetLog(nDev);
}

TFrameDevicePasport* GetFramePasp(unsigned nDev)
{
    if(!(nDev<Count())) return NULL;
    return static_cast<TFrameDevicePasport*>( paspTab[nDev]->Controls[0] );
}
TFrameDevicePasport* GetFramePaspOfAddy(unsigned addy)
{
    int nDev = IndexOfAddy(addy);
    if(nDev==-1) return NULL;
    return GetFramePasp(nDev);
}



TFrameGraph* GetFrameGraph(unsigned nDev)
{
    if(!(nDev<Count())) return NULL;
    return static_cast<TFrameGraph*>( chartTab[nDev]->Controls[0] );
}
TFrameGraph* GetFrameGraphOfAddy(unsigned addy)
{
    int nDev = IndexOfAddy(addy);
    if(nDev==-1) return NULL;
    return GetFrameGraph(nDev);
}

AnsiString FormatVarnum(unsigned varnum){
    switch(varnum){
    case 0:
        return "Конц.";
    case 2:
        return "T\"C";
    case 4:
        return "Iмил";
    case 12:
        return "Refk";
    case 14:
        return "Workk";
    case 16:
        return "Var1";
    }
    return "-";
}

LRESULT AddModbusFloatValueToChart(unsigned addy, unsigned varnum, double value){
    TFrameGraph* fg = GetFrameGraphOfAddy(addy);
    AnsiString seriesName = FormatVarnum(varnum);
    for (int i = 0; i< fg->chrt_->SeriesCount(); ++i){
        TFastLineSeries* ser = static_cast<TFastLineSeries*>(fg->chrt_->Series[i]);
        if (ser->Title == seriesName){
            ser->AddXY( Now(), value );
        }
    }
}
//------------------------------------------------------------------------------
void UpdateSelectDevice(unsigned n )
{
	const bool vis = IsSelectedDevice(n);
    for( unsigned i=0; i<GRDS_COUNT; ++i )
        my::Grd::SetColVis(grdDvs[i].first, grdDvs[i].second+n, vis);
    repTab[n]->TabVisible = vis;
    paspTab[n]->TabVisible = vis;

}
//------------------------------------------------------------------------------
void AddDevice(unsigned serial, unsigned addy)
{
    TListItem *lItem = lvDevs->Items->Add();
    lItem->Caption = serial;
    lItem->SubItems->Add(addy);
    lItem->SubItems->Add("");
    lItem->Checked = 1;
    const int devsCount = Count();

    for( unsigned i=0; i<GRDS_COUNT; ++i ) {
        grdDvs[i].first->ColCount = devsCount + grdDvs[i].second;
        grdDvs[i].first->Cells[grdDvs[i].second+devsCount-1][0] = serial;
    }

    TComponent* null = NULL;
    TTabSheet *tbs = new TTabSheet(null);
    tbs->PageControl = pgCtRep;
    TRichEdit *re = new TRichEdit(tbs);
    re->Parent = tbs;
    re->Align = alClient;
    re->Font->Name = "Tahoma";
    re->Font->Size = 9;
    re->ScrollBars = ssVertical;
    tbs->Caption = serial;

    repTab.push_back( PTabSheet(tbs) );
    

    tbs = new TTabSheet(null);
    tbs->PageControl = pgPasp;
    TFrameDevicePasport *frameDevicePasport = new TFrameDevicePasport(null);
    frameDevicePasport->Parent = tbs;
    tbs->Caption = serial;

    paspTab.push_back( PTabSheet(tbs) );

    tbs = new TTabSheet(null);
    tbs->PageControl = pgChart;
    TFrameGraph *frameGraph = new TFrameGraph(tbs, tbs);

    unsigned vars[] = {0, 2, 4, 12, 14, 16};
    for (int i = 0; i< 6; ++i) {
        TChartSeries *ser = new TFastLineSeries(tbs);
        ser->Title = FormatVarnum(vars[i]);
        frameGraph->AddSeries(ser);

    }

    tbs->Caption = serial;  
    chartTab.push_back( PTabSheet(tbs) );


}
//------------------------------------------------------------------------------
void DelDevice(unsigned idx)
{
	assert(  idx<Count() );

    for( unsigned i=0; i<GRDS_COUNT; ++i )
        my::Grd::DeleteCol( grdDvs[i].first, grdDvs[i].second+idx, 1);

    lvDevs->Items->Delete( idx );
    for( unsigned i= idx; i< Count(); ++i )
    	UpdateSelectDevice(i);

    repTab.erase( repTab.begin()+idx );
    paspTab.erase( paspTab.begin()+idx );
    chartTab.erase( chartTab.begin()+idx );

}
//------------------------------------------------------------------------------
void ChangeDeviceAddy(unsigned n, unsigned addy)
{
    lvDevs->Items->Item[n]->SubItems[1].Text = addy;
    /*
    for( unsigned i=0; i<GRDS_COUNT; ++i ) {
        grdDvs[i].first->Cells[grdDvs[i].second+n][0] = addy;
        repTab[n]->Caption = addy;
        paspTab[n]->Caption = addy;
    }
    */
}
//------------------------------------------------------------------------------
std::string FormatKNum(unsigned n)
{
    return MYSPRINTF_("K%d", n).c_str();
}
void SaveFile(const AnsiString& fn)
{
	DeleteFile(fn);
    boost::shared_ptr<TiXmlDocument> doc ( TiXML::CreateDocument(fn.c_str()) );
    TiXmlElement* mainElem = TiXML::ChildElement( doc.get(), "Main" );
    const DAK::Kefs& kfsLst = DAK::GetKefs();
    const unsigned kfsLstSize = kfsLst.size();
    const Nums addys = GetAddys(), serials = GetSerials();

    for( unsigned iDev=0; iDev<Count(); ++iDev )
    {
    	TiXmlElement
        	*eDev = TiXML::CreateChild( mainElem, "ДАК"),
            *eKoefs = TiXML::CreateChild(eDev, "кэфы")
        ;
        TiXMLAtr::Set( eDev, "адрес", addys[iDev] );
        TiXMLAtr::Set( eDev, "серийный", serials[iDev] );
        TiXMLAtr::Set( eDev, "Выбран", lvDevs->Items->Item[iDev]->Checked );
        for( unsigned iKf=0; iKf<kfsLstSize; ++iKf )
        {
            const AnsiString s = GetKef(iDev, iKf);
            if(s.IsEmpty()) continue;
            TiXmlElement *eKef = TiXML::CreateChild(eKoefs, "кэф");
            TiXMLAtr::Set( eKef, "n",  kfsLst[iKf].n );
            TiXMLText::SetStr( eKef, s.c_str() );
        }
        Devs::GetFramePasp(iDev)->Save( TiXML::CreateChild( eDev, "проверки") );
    }
    doc->SaveFile();
}

AnsiString ExT(AnsiString s)
{
    double val;
    if( DAK::Format::TryGetValue1(s, val) )
        return val;
    else
        return "";
}

void SaveForTermoFile(const AnsiString& fn)
{
	DeleteFile(fn);
    boost::shared_ptr<TiXmlDocument> doc ( TiXML::CreateDocument(fn.c_str()) );
    TiXmlElement* mainElem = TiXML::ChildElement( doc.get(), "Main" );

    const Nums addys = GetAddys(), serials = GetSerials();

    const unsigned nTest = DAK::Test::temperature;

    for( unsigned iDev=0; iDev<Count(); ++iDev )
    {
        unsigned addy = addys[iDev];
    	TiXmlElement  *eDev = TiXML::CreateChild( mainElem, ("Прибор_"+IntToStr(addy)).c_str() );
        TiXMLAtr::Set( eDev, "адрес", addy);

        TFrameDevicePasport* frm = GetFramePaspOfAddy(addy);
        {
        TiXmlElement  *SCALE_CH_0_T = TiXML::CreateChild( eDev, "SCALE_CH_0_T");
            TiXmlElement  *Var2 = TiXML::CreateChild( SCALE_CH_0_T, "Var2");
                TiXMLAtr::Set( Var2, "_0", frm->Get(nTest, 2, 1) );
                TiXMLAtr::Set( Var2, "_1", frm->Get(nTest, 14, 1));
                TiXMLAtr::Set( Var2, "_2", frm->Get(nTest, 10, 1));
            TiXmlElement  *Tpp = TiXML::CreateChild( SCALE_CH_0_T, "Tpp");
                TiXMLAtr::Set( Tpp, "_0", ExT( frm->Get(nTest, 1, 1)) );
                TiXMLAtr::Set( Tpp, "_1", ExT( frm->Get(nTest, 13, 1)) );
                TiXMLAtr::Set( Tpp, "_2", ExT( frm->Get(nTest, 9, 1)) );
        }
        {
        TiXmlElement  *SCALE_CH_S_T = TiXML::CreateChild( eDev, "SCALE_CH_S_T");
            TiXmlElement  *Var2 = TiXML::CreateChild( SCALE_CH_S_T, "Var2");
                TiXMLAtr::Set( Var2, "_0", frm->Get(nTest, 2, 3));
                TiXMLAtr::Set( Var2, "_1", frm->Get(nTest, 14, 3));
                TiXMLAtr::Set( Var2, "_2", frm->Get(nTest, 10, 3));
            TiXmlElement  *Tpp = TiXML::CreateChild( SCALE_CH_S_T, "Tpp");
                TiXMLAtr::Set( Tpp, "_0", ExT( frm->Get(nTest, 1, 3)) );
                TiXMLAtr::Set( Tpp, "_1", ExT( frm->Get(nTest, 13, 3)) );
                TiXMLAtr::Set( Tpp, "_2", ExT( frm->Get(nTest, 9, 3)) );
        }
    }
    doc->SaveFile();
}

//------------------------------------------------------------------------------
void ClearDevicesData()
{
    for( unsigned i=0; i<GRDS_COUNT; ++i )
        my::Grd::DeleteCol( grdDvs[i].first, grdDvs[i].second, grdDvs[i].first->ColCount-grdDvs[i].second );
    lvDevs->Items->Clear();
    repTab.clear();
    paspTab.clear();
    chartTab.clear();

}
//------------------------------------------------------------------------------
void LoadFile(const AnsiString& fn)
{
    const DAK::Kefs& kfsLst = DAK::GetKefs();
    const unsigned kfsLstSize = kfsLst.size();
	boost::shared_ptr<TiXmlDocument> doc ( TiXML::CreateDocument( fn.c_str() ) );
    TiXmlElement* mainElem = doc->RootElement();

    if( !mainElem )
    {
    	ShowMessage( "формат файла не соответствует требуемому." );
    	return;
    }
    ClearDevicesData();
    unsigned iDev = 0;
    for( TiXmlElement* eDev = mainElem->FirstChildElement(); eDev; eDev = eDev->NextSiblingElement() )
    {
        const int
            addy    = TiXMLAtr::Get(eDev, "адрес",      -1),
            serial  = TiXMLAtr::Get(eDev, "серийный",   -1);
        if( addy<1 || serial<1  ) continue;
        AddDevice(serial, addy);
        const unsigned nDev = lvDevs->Items->Count-1;
        lvDevs->Items->Item[nDev]->Checked = TiXMLAtr::Get( eDev, "Выбран", true );
        TiXmlElement *eKoefs = TiXML::ChildElement( eDev, "кэфы");

        /*
        for( unsigned iKf=0; iKf<kfsLstSize; ++iKf )
            SetKef(nDev, iKf, TiXMLAtr::Get( eKoefs, FormatKNum(kfsLst[iKf].n),
                GetKef(nDev, iKf)) );
         for( unsigned iKf=0; iKf<kfsLstSize; ++iKf )
        {
            TiXmlElement *eKef = TiXML::CreateChild(eKoefs, "кэф");
            TiXMLAtr::Set( eKef, "n",  kfsLst[iKf].n );
            TiXMLText::SetStr( eKef, GetKef(iDev, iKf) );
        }
        */
        TFrameDevicePasport *frameDevicePasport = Devs::GetFramePasp(iDev);

        for( TiXmlElement* eKef = eKoefs->FirstChildElement(); eKef; eKef = eKef->NextSiblingElement() ) {
            const unsigned n = TiXMLAtr::Get<unsigned>( eKef, "n",  0 );
            const unsigned iKf = DAK::Kef::GetNumberOfKNum(n);
            if( iKf<kfsLstSize ) {
                AnsiString s = TiXMLText::GetStr( eKef ).c_str();
                SetKef(nDev, iKf, s );
                if(kfsLst[iKf].n==33) {
                    frameDevicePasport->Set( s,  DAK::Test::temperature, 0, 0);
                }  
            }  
        }

        frameDevicePasport->Load( TiXML::ChildElement(eDev, "проверки") );
        UpdateSelectDevice(nDev);
        ++iDev;
    }
}
//------------------------------------------------------------------------------
bool IsSelectedDevice(unsigned n)
{
	return lvDevs->Items->Item[n]->Checked;
}
//------------------------------------------------------------------------------
unsigned Count()
{
    return lvDevs->Items->Count;
}
//------------------------------------------------------------------------------
void SelectDevice(unsigned n, bool setSelected)
{
    if( n<Count() )
    {
        lvDevs->Items->Item[n]->Checked = setSelected;
        UpdateSelectDevice(n);
    }
}

void SelectAddy(unsigned addy, bool setSelected)
{
    const int n = IndexOfAddy(addy);
    if(n>-1)
        SelectDevice(n, setSelected);
}
//------------------------------------------------------------------------------
Nums GetSelectedAddys()
{
	const unsigned sz = Count();
	Nums ret;
    for( unsigned i=0; i<sz; ++i )
    	if( lvDevs->Items->Item[i]->Checked  )
        	ret.push_back( StrToInt( Lv::Get(lvDevs, 1, i) ) );
    return ret;
}
//------------------------------------------------------------------------------
int IndexOfAddy(unsigned slave)
{
	const Nums slaves = GetAddys();
    Nums::const_iterator i = std::find( slaves.begin(), slaves.end(), slave );
   	return i==slaves.end() ? -1 : i-slaves.begin();
}
//------------------------------------------------------------------------------
bool IsSelectedAddy(unsigned slave)
{
	const int idx = IndexOfAddy(slave);
    return idx!=-1 && IsSelectedDevice(idx);
}
//------------------------------------------------------------------------------
bool HasOneSelectedAddy()
{
    for( unsigned i=0; i<Count(); ++i )
    	if( IsSelectedDevice(i) ) return true;
    return false;
}
//------------------------------------------------------------------------------
unsigned Addy(unsigned nDev)
{
    assert(nDev<Count());
    return StrToInt( Lv::Get(lvDevs, 1, nDev) );
}
//-----------------------------------------------------------------------------
unsigned Serial(unsigned nDev)
{
    assert(nDev<Count());
    return StrToInt( Lv::Get(lvDevs, 0, nDev) );
}
//------------------------------------------------------------------------------

Nums GetAddys()
{
	const unsigned sz = Count();
	Nums ret(sz);
    for( unsigned i=0; i<sz; ++i )
    	ret[i] = Addy(i);
    return ret;
}

Nums GetSerials()
{
	const unsigned sz = Count();
	Nums ret(sz);
    for( unsigned i=0; i<sz; ++i )
    	ret[i] = Serial(i);
    return ret;
}

Items GetItems()
{
    const unsigned sz = Count();
	Items ret(sz);
    for( unsigned i=0; i<sz; ++i ) {
    	ret[i].first    = StrToInt( Lv::Get(lvDevs, 0, i) );
        ret[i].second   = StrToInt( Lv::Get(lvDevs, 1, i) );
    }
    return ret;
}

//------------------------------------------------------------------------------
bool HasOneGoodSelectedAddy()
{
    const Nums slaves = GetAddys();
    for( unsigned i=0; i<slaves.size(); ++i )
    	if( IsSelectedDevice(i) &&
            !IsNotAnsweredAddy(slaves[i]) )
            return true;
    return false;
}
//------------------------------------------------------------------------------
void SetNotAnswered(unsigned slave)
{
	const int idx = IndexOfAddy(slave);
    if(idx!=-1)
    {
        Lv::Set( lvDevs, 2, idx, "не отвечает");
        Lv::Set( lvDevs, 1, idx, Lv::Get( lvDevs, 1, idx) );
        Lv::Set( lvDevs, 0, idx, Lv::Get( lvDevs, 0, idx) );
        lvDevs->Column[0]->Width = lvDevs->Column[0]->Width - 1;
        lvDevs->Column[0]->Width = lvDevs->Column[0]->Width + 1;
        my::Grd::RedrawCell( grdVars, idx+1, 0 );
        my::Grd::RedrawCell( grdScnList, idx+5, 0 );
    }
}
//------------------------------------------------------------------------------
void SetAnswered(unsigned slave)
{
    const int idx = IndexOfAddy(slave);
    if(idx!=-1)
    {
        Lv::Set( lvDevs, 2, idx, "ок" );
        Lv::Set( lvDevs, 1, idx, Lv::Get( lvDevs, 1, idx) );
        Lv::Set( lvDevs, 0, idx, Lv::Get( lvDevs, 0, idx) );
        lvDevs->Column[0]->Width = lvDevs->Column[0]->Width - 1;
        lvDevs->Column[0]->Width = lvDevs->Column[0]->Width + 1;
        my::Grd::RedrawCell( grdVars, idx+1, 0 );
        my::Grd::RedrawCell( grdScnList, idx+5, 0 );
    }
}
//------------------------------------------------------------------------------
void SetKef(unsigned nDevice, unsigned kefNum, const AnsiString& s)
{
    struct
    {
        LRESULT SetKef1(unsigned nDevice, unsigned kefNum, const AnsiString& s)
        {
            unsigned rc = Devs::grdKfs->RowCount, cc = Devs::grdKfs->ColCount;
            if(nDevice+3<cc && kefNum+1<rc)
                Devs::grdKfs->Cells[nDevice+3][kefNum+1] = s;
            return 0;
        }
    } h;

    my::Synchronize3( &h.SetKef1, nDevice, kefNum, s, __FILE_LINE__);
}
//------------------------------------------------------------------------------
AnsiString GetKef(unsigned nDevice, unsigned kefNum)
{
    unsigned rc = Devs::grdKfs->RowCount, cc = Devs::grdKfs->ColCount;
    return (nDevice+3<cc && kefNum+1<rc) ?
        grdKfs->Cells[nDevice+3][kefNum+1] : AnsiString("");
}
//------------------------------------------------------------------------------
AnsiString GetSensorSlave(unsigned nDevice)
{
    unsigned rc = Devs::grdKfs->RowCount, cc = Devs::grdKfs->ColCount;
    if(nDevice+3<cc) {

        for( int row=0; row<rc; ++row)
            if(grdKfs->Cells[0][row]=="33") {
                AnsiString s = grdKfs->Cells[nDevice+3][row];
                MyInt vv = MyStrToInt( s );
                if ( vv.second ) {
                    return (Int2Str(vv.first,16)+std::string("h")).c_str();
                }
            }
    }
    return "";
}


void SetKefDefault( unsigned nDevice, unsigned koefNum )
{
    SetKef( nDevice, koefNum, grdKfs->Cells[2][koefNum+1] );
}
//------------------------------------------------------------------------------
void SetKefOfAddyDef( unsigned addy, unsigned koefNum )
{
    const int slaveIdx = IndexOfAddy(addy);
    if(slaveIdx==-1) return;
    assert(koefNum<DAK::GetKefs().size());
    SetKefDefault( slaveIdx, koefNum );
}
//------------------------------------------------------------------------------
AnsiString GetKefOfAddy( unsigned slave, unsigned kefNum)
{
    const int slaveIdx = IndexOfAddy(slave);
    return slaveIdx==-1 ? AnsiString() : GetKef(slaveIdx, kefNum);
}
//------------------------------------------------------------------------------
void SetKefOfAddy( unsigned slave, unsigned kefNum, const AnsiString &kef)
{
    const int slaveIdx = IndexOfAddy(slave);
    if( slaveIdx>-1) SetKef(slaveIdx, kefNum, kef);
}
//------------------------------------------------------------------------------
bool IsKefSelected(unsigned n)
{
    const unsigned rc = grdKfs->RowCount;
    return n+1<=rc ? my::Grd::IsRowSelected(grdKfs,n+1) : false;
}
//------------------------------------------------------------------------------
Nums GetSelectedKefsNums()
{
	const int rc = grdKfs->RowCount;
	Nums ret;
    for( int ro=1; ro<rc; ++ro )
    	if( my::Grd::IsRowSelected(grdKfs,ro) )
        {
        	ret.push_back( ro-1 );
        }
    return ret;
}
//------------------------------------------------------------------------------
void SetDefaultSelectedKefsItemsVals()
{
    const DAK::Kefs& kefs = DAK::GetKefs();
    const unsigned kefsCount = kefs.size();

    for(unsigned nDev=0; nDev<Count(); ++nDev)
        if( IsSelectedDevice(nDev) )
            for(unsigned nKef=0; nKef<kefsCount; ++nKef)
                if( IsKefSelected(nKef) )
                    SetKef(nDev, nKef, grdKfs->Cells[2][nKef+1] );

}
//------------------------------------------------------------------------------
void SaveDefaultKefsVals()
{
    const DAK::Kefs& kefs = DAK::GetKefs();
    const unsigned sz = kefs.size();
    using DAK::Kef;
    for( unsigned i=0; i<sz; ++i )
    {
        Kef& kef = const_cast<Kef&> (kefs[i]);
        kef.def = grdKfs->Cells[2][i+1];
    }
    DAK::SaveKefsList();

}
//------------------------------------------------------------------------------
void __fastcall Helper::lvDevsClick(TObject *Sender)
{
    TPoint pt;
    GetCursorPos( &pt );
    pt = lvDevs->ScreenToClient( pt );
    TListItem *item = lvDevs->GetItemAt( pt.x, pt.y );
    if( item!=NULL) 
        UpdateSelectDevice( item->Index );

}
void __fastcall Helper::lvDevsCustomDrawItem(TCustomListView *Sender,
      TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
    TCanvas *cnv = lvDevs->Canvas;
    cnv->Font->Name = "Arial";
    cnv->Font->Size = 10;
    if( !Item->Checked )
        cnv->Brush->Color = cl3DLight;

    if( IsNotAnsweredAddy( Addy(Item->Index) ) ) {
        cnv->Font->Color = clYellow;
        cnv->Font->Style = cnv->Font->Style + (TFontStyles() << fsBold);
        cnv->Brush->Color = clBlack;
    }
}
//---------------------------------------------------------------------------
void __fastcall Helper::lvDevsCustomDrawSubItem(TCustomListView *Sender,
      TListItem *Item, int SubItem, TCustomDrawState State,
      bool &DefaultDraw)
{
    TCanvas *cnv = lvDevs->Canvas;
    cnv->Font->Name = "Arial";
    cnv->Font->Size = 10;
    if( IsNotAnsweredAddy( Addy(Item->Index) ) ) {
        cnv->Font->Color = clYellow;
        cnv->Font->Style = cnv->Font->Style + (TFontStyles() << fsBold);
        cnv->Brush->Color = clBlack;
    } else if( !Item->Checked )
        cnv->Brush->Color = cl3DLight;
}
void __fastcall Helper::lvDevsKeyPress(TObject *Sender, char &Key)
{
    if(Key!=32) return;
    TListItem *item = lvDevs->Selected;
	if( item!=NULL) {
            //cbCurentScenaryChange(NULL);
        UpdateSelectDevice( item->Index );
    }
}

void PromptUserInputDevice()
{
    const Nums addys = GetAddys(), serials = GetSerials();

	go_there:
	AnsiString sS = "";
	if( !InputQuery( "Запрос значения",
    "Введите через пробел серийный номер и сетевой адрес", sS) ) return;

    std::stringstream sstr( sS.c_str() );
    std::string s1, s2;
    int serial, addy;
    sstr >> s1 >> s2;
    if( !TryStrToInt(s1.c_str(), serial) || !TryStrToInt(s2.c_str(), addy) ||
        serial<1 || addy<1 ||
        std::find( addys.begin(), addys.end(), addy )!=addys.end() ||
        std::find( serials.begin(), serials.end(), serial )!=serials.end() )
    {
    	ShowMessage("Некорректный ввод \""+sS+"\"!" );
        goto go_there;
    }
    AddDevice(serial,addy);
}

void PromptUserChangeDevice(unsigned nDev)
{
    Nums addys = GetAddys();
    addys.erase( addys.begin()+nDev );

	go_there:
	AnsiString sS = "";
	if( !InputQuery( "Запрос значения",
    "Введите серийный номер и сетевой адрес", sS) ) return;

    std::stringstream sstr( sS.c_str() );
    std::string s1, s2;
    int addy, serial;
    sstr >> s1 >> s2;
    if( !TryStrToInt(s1.c_str(), serial) || !TryStrToInt(s2.c_str(), addy) ||
        addy<1 || std::find( addys.begin(), addys.end(), addy )!=addys.end()  )
    {
    	ShowMessage("Некорректный ввод \""+sS+"\"!" );
        goto go_there;
    }
    Lv::Set(lvDevs, 1, nDev, addy);
    Lv::Set(lvDevs, 0, nDev, serial);
}



//------------------------------------------------------------------------------
}; // namespace Devs
//------------------------------------------------------------------------------
