
//------------------------------------------------------------------------------
#pragma hdrstop
#include "xmlUtils_.h"

//------------------------------------------------------------------------------
#include <fstream>
#include <windows.h>
#include "SGrdHlpr.h"
//------------------------------------------------------------------------------
#define DEFAULT_FIRST_XML_STRING "<?xml version=\"1.0\" encoding=\"Windows-1251\" ?>"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
namespace TiXML
{
//------------------------------------------------------------------------------
TiXmlDocument* CreateDocument( const char* fn )
{
	TiXmlDocument* ret = new TiXmlDocument;
    if( !ret->LoadFile( fn )  )
        ret->Parse(DEFAULT_FIRST_XML_STRING);
    return ret;
}
//------------------------------------------------------------------------------
TiXmlElement* CreateChild( TiXmlNode* elem, const std::string& s)
{
    return elem->LinkEndChild( new TiXmlElement( s.c_str() ) )->ToElement();
}

TiXmlElement* ChildElement( TiXmlNode* elem, const std::string& s)
{
	assert(elem);
    TiXmlElement* ret = elem->FirstChildElement( s.c_str() );
    return ret ? ret : elem->LinkEndChild( new TiXmlElement( s.c_str() ) )->ToElement();
}
//------------------------------------------------------------------------------

void SaveWindowPlacement( TiXmlElement* elem, HWND hWnd )
{
    WINDOWPLACEMENT wndpl = {0};
    ::GetWindowPlacement(hWnd, &wndpl);
    #define SAVE_(nn) elem->SetAttribute(""#nn, wndpl.##nn )
    SAVE_(length);
    SAVE_(flags);
    SAVE_(showCmd);
    SAVE_(ptMinPosition.x);
    SAVE_(ptMinPosition.y);
    SAVE_(ptMaxPosition.x);
    SAVE_(ptMaxPosition.y);
    SAVE_(rcNormalPosition.left);
    SAVE_(rcNormalPosition.top);
    SAVE_(rcNormalPosition.right);
    SAVE_(rcNormalPosition.bottom);
    #undef SAVE_
}
//------------------------------------------------------------------------------
void LoadWindowPlacement( TiXmlElement* elem, HWND hWnd )
{
    WINDOWPLACEMENT wndpl = {0};
    ::GetWindowPlacement(hWnd, &wndpl);
    #define LOAD_(nn) wndpl.##nn = TiXMLAtr::Get( elem, ""#nn, wndpl.##nn )
    LOAD_(length);
    LOAD_(flags);
    LOAD_(showCmd);
    LOAD_(ptMinPosition.x);
    LOAD_(ptMinPosition.y);
    LOAD_(ptMaxPosition.x);
    LOAD_(ptMaxPosition.y);
    LOAD_(rcNormalPosition.left);
    LOAD_(rcNormalPosition.top);
    LOAD_(rcNormalPosition.right);
    LOAD_(rcNormalPosition.bottom);
    #undef LOAD_
    ::SetWindowPlacement(hWnd, &wndpl);
}
//------------------------------------------------------------------------------
void SaveConsolePlacement( TiXmlElement* elem )
{
    SaveWindowPlacement( elem, ::GetConsoleWindow() );
}
//------------------------------------------------------------------------------
void LoadConsolePlacement( TiXmlElement* elem )
{
    const HWND hCon = ::GetConsoleWindow();
    WINDOWPLACEMENT wndpl = {0};
    LoadWindowPlacement( elem, hCon );
    ::SetWindowPlacement( hCon, &wndpl );

    const bool conIsVis =
    	wndpl.showCmd==SW_NORMAL ||
        wndpl.showCmd==SW_SHOWMAXIMIZED ||
        wndpl.showCmd==SW_SHOWNOACTIVATE ||
        wndpl.showCmd==SW_SHOW
    ;
    if( !conIsVis )
    	::ShowWindow( hCon, SW_HIDE );
}
//------------------------------------------------------------------------------
template< class SetT>
bool Contains(const SetT& theSet, const typename SetT::key_type &key)
{
    return theSet.find(key)!=theSet.end();
}
//------------------------------------------------------------------------------
void SaveGrd( TiXmlElement* elem, TStringGrid* grd)
{
    elem = ChildElement( elem, grd->Name.c_str() );
    //using namespace GrdProp;
    //if( Contains(props, row_selections) )
        TiXMLAtr::Set(elem, "выбранные_строки", my::Grd::RowsSelectionInfoToStr(grd) );
    //if( Contains(props, col_widths) )
        TiXMLText::Set( ChildElement( elem, "ширина_колонок" ),
            my::Grd::ColWidthsToStr( grd ) );

}
//------------------------------------------------------------------------------
void LoadGrd( TiXmlElement* elem, TStringGrid* grd)
{
    elem = ChildElement( elem, grd->Name.c_str() );
    //using namespace GrdProp;

    //if( Contains(props, row_selections) )
        my::Grd::SetRowsSelectionInfo(grd, TiXMLAtr::Get(elem, "выбранные_строки", AnsiString("") ) );

    //if( Contains(props, col_widths) )
        my::Grd::SetColWidthsAsStr( grd,
            TiXMLText::Get( ChildElement( elem, "ширина_колонок" ),
                my::Grd::ColWidthsToStr( grd ) ) );
}
//------------------------------------------------------------------------------
};//namespace TiXML
//------------------------------------------------------------------------------
namespace TiXMLAtr
{
//------------------------------------------------------------------------------
TiXmlAttribute* GetNode(TiXmlElement* elem, const std::string& attributeName)
{
    for( TiXmlAttribute* attr = elem->FirstAttribute(); attr; attr=attr->Next() )
        if( attr->Name()==attributeName )
            return attr;
    return NULL;
}
//------------------------------------------------------------------------------

std::string GetStr( TiXmlElement* elem, const std::string& id, const std::string& def )
{
    TiXmlAttribute* attr = GetNode(elem, id);
    return attr  ? std::string( attr->Value() ) : def;
}
//------------------------------------------------------------------------------
void SetStr( TiXmlElement* elem, const std::string& id, const std::string& val )
{
    if(!val.empty()) {
	    elem->SetAttribute(id.c_str(), val.c_str() );
    } else if( TiXmlAttribute* attr = GetNode(elem, id) )
            attr->SetValue("");
}
//------------------------------------------------------------------------------
struct SaveValueOnDeleteStringAdapter
{
    SaveValueOnDeleteStringAdapter(TiXmlElement* elem, const std::string& name) :
        elem_(elem), name_(name) {}
    void operator()(StringAdapter* adpt)
    {
        TiXMLAtr::SetStr(elem_, name_, adpt->Get());
        delete adpt;
    }
    TiXmlElement* elem_;
    const std::string name_;
};
//------------------------------------------------------------------------------
Holder CreateHolder( TiXmlElement* elem, const std::string& name, StringAdapter* adpt )
{
    Holder ret(
        ElemName(elem, name),
        PStringAdapter( adpt, SaveValueOnDeleteStringAdapter(elem, name)));
    ret.second->Set( GetStr(elem,name) );
    return ret;
}
//------------------------------------------------------------------------------
}; // namespace TiXML
//------------------------------------------------------------------------------
namespace TiXMLText
{
//------------------------------------------------------------------------------
std::string GetStr(TiXmlNode* elem, const std::string& def )
{
    elem = elem->FirstChild();
    if(!elem) return "";
    TiXmlText* ret = elem->ToText();
    return ret ? std::string( ret->Value() ) : def;
}
//
void SetStr( TiXmlNode* elem, const std::string& s)
{
    elem->Clear();
    if( !s.empty() )
        elem->LinkEndChild( new TiXmlText( s.c_str() ));
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
}; //namespace TiXMLText
//------------------------------------------------------------------------------


