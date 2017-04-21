//---------------------------------------------------------------------------
#include <dialogs.hpp>

#pragma hdrstop

#include "uDevListHlpr.h"
#include "Unit1.h"
#include "uFrameDropDownPanel.h"
#include "SGrdHlpr.h"
#include "uFrameGraph.h"
#include "xmlUtils_.h"
#include "listViewHelpr_.h"
#include "ctrlsys.h"
#include "ModbusAdapter.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

void TForm1::AddDevice(unsigned adr, const AnsiString& caption)
{
    TListItem *lItem = lvDevs->Items->Add();
    lItem->Caption = adr;
    lItem->SubItems->Add(caption);
    lItem->Checked = 1;
    const int devsCount = DevsCount();

    grdVars->ColCount = devsCount + 3;
    grdVars->Cells[devsCount+2][0] = adr;

    grdScnList->ColCount = devsCount + 5;
    grdScnList->Cells[devsCount+4][0] = adr;
}
//------------------------------------------------------------------------------
void TForm1::DelDevice(unsigned idx)
{
	assert(  idx<DevsCount() );
    my::Grd::DeleteCol( grdVars, idx+4, 1);
    my::Grd::DeleteCol( grdScnList, idx+6, 1);

    lvDevs->Items->Delete( idx );
    for( unsigned i= idx; i< DevsCount(); ++i )
    	UpdateSelectDevice(i);
}
//------------------------------------------------------------------------------
void TForm1::SaveFile(const AnsiString& fn)
{
	DeleteFile(fn);
    TiXmlDocument* doc = TiXML::CreateXMLDocument( fn );
    TiXmlElement* mainElem = TiXML::GiveNotNulElement( doc, "Main");
    for( unsigned iDev=0; iDev<DevsCount(); ++iDev )
    {
    	const AnsiString sAdr = lvDevs->Items->Item[iDev]->Caption;
    	TiXmlElement
        	*eDev = TiXML::GiveNotNulElement( mainElem, "Прибор_"+sAdr ),
            *eVars = TiXML::GiveNotNulElement( eDev, "vars")
        ;
        TiXML::SetAtr( eDev, "адрес", sAdr );
        TiXML::SetIntAtr( eDev, "Выбран", lvDevs->Items->Item[iDev]->Checked );
    }
    doc->SaveFile();
}
//------------------------------------------------------------------------------
void TForm1::ClearDevicesData()
{
    my::Grd::DeleteCol( grdVars, 4, grdVars->ColCount-4 );
    my::Grd::DeleteCol( grdScnList, 6, grdScnList->ColCount-6 );
    lvDevs->Items->Clear();
}
//------------------------------------------------------------------------------
void TForm1::LoadFile(const AnsiString& fn)
{
	TiXmlDocument* doc = TiXML::CreateXMLDocument( fn );
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
        const MyInt vAdr = MyStrToInt( TiXML::GetStrAtr( eDev, "адрес", "" ) );
        if( !vAdr.second || vAdr.first<1 || vAdr.first>255 ) continue;
        const unsigned slave = vAdr.first;
        AddDevice(slave, "");
        const unsigned nDev = lvDevs->Items->Count-1;
        lvDevs->Items->Item[ nDev ]->Checked = TiXML::GetIntAtrDef( eDev, "Выбран", 1 );
        UpdateSelectDevice(nDev);
        ++iDev;
    }
}
//------------------------------------------------------------------------------
void TForm1::UpdateSelectDevice(unsigned n )
{
	const bool vis = IsDeviceSelected(n);
    my::Grd::SetColVis(grdVars, n+3, vis);
    my::Grd::SetColVis(grdScnList, n+5, vis);


    /*
    const VInt8 selectedSlaves = GetSelectedSlaves();
    std::vector<int> addrs(257);
    for( unsigned i=0; i<selectedSlaves.size(); ++i ) addrs.at( selectedSlaves[i] ) = true;
    LabeledEdit2->Text = my::FormatBitset(addrs).c_str();
    */
}
//------------------------------------------------------------------------------
bool TForm1::IsDeviceSelected(unsigned n) const
{
	return lvDevs->Items->Item[n]->Checked;
}
//------------------------------------------------------------------------------
LRESULT TForm1::SelectAddy(unsigned addy, bool setSelected)
{
    const int n = IndexOfSlave(addy);
    if(n>-1)
    {
        lvDevs->Items->Item[n]->Checked = setSelected;
        UpdateSelectDevice(n);
    }
    return 0;
}
//------------------------------------------------------------------------------
VInt8 TForm1::GetSelectedSlaves()
{
	const unsigned sz = DevsCount();
	VInt8 ret;
    for( unsigned i=0; i<sz; ++i )
    	if( lvDevs->Items->Item[i]->Checked  )
        	ret.push_back( StrToInt( Lv::Get(lvDevs, 0, i) ) );
    return ret;
}
//------------------------------------------------------------------------------
int TForm1::IndexOfSlave(unsigned slave) const
{
	const VInt8 slaves = GetSlaves();
    VInt8::const_iterator i = std::find( slaves.begin(), slaves.end(), slave );
   	return i==slaves.end() ? -1 : i-slaves.begin();
}
//------------------------------------------------------------------------------
bool TForm1::IsSlaveSelected(unsigned slave)
{
	const int idx = IndexOfSlave(slave);
    return idx!=-1 && IsDeviceSelected(idx);
}
//------------------------------------------------------------------------------
bool TForm1::HasOneSlaveSelected()
{
    for( unsigned i=0; i<DevsCount(); ++i )
    	if( IsDeviceSelected(i) ) return true;
    return false;
}
//------------------------------------------------------------------------------
VInt8 TForm1::GetSlaves() const
{
	const unsigned sz = DevsCount();
	VInt8 ret(sz);
    for( unsigned i=0; i<sz; ++i )
    	ret[i] = StrToInt( Lv::Get(lvDevs, 0, i) );
    return ret;
}
//------------------------------------------------------------------------------
AnsiString TForm1::DeviceCaption(unsigned n) const
{
	assert( n<DevsCount() );
    return  "Прибор "+lvDevs->Items->Item[n]->Caption;
}
//------------------------------------------------------------------------------
bool TForm1::HasOneGoodSlaveSelected() const
{
    const VInt8 slaves = GetSlaves();

    for( unsigned i=0; i<slaves.size(); ++i )
    	if( IsDeviceSelected(i) &&
            !IsNotAnsweredAddy(slaves[i]) )
            return true;
    return false;
}
//------------------------------------------------------------------------------

