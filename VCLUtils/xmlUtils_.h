//------------------------------------------------------------------------------
#ifndef xmlUtils_H
#define xmlUtils_H
//------------------------------------------------------------------------------
// my
#include "tinyxml.h"
#include "MyStrUtils.h"
#include "my_static_check.h"
// std
#include <utility>
#include <vector>
#include <set>
//
#include <grids.hpp>

//------------------------------------------------------------------------------
namespace TiXML
{
    TiXmlDocument* CreateDocument( const char* fn );

    TiXmlElement* ChildElement( TiXmlNode* elem, const std::string& s);

    TiXmlElement* CreateChild( TiXmlNode* elem, const std::string& s);

    void SaveWindowPlacement( TiXmlElement* elem, void* hWnd );
	void LoadWindowPlacement( TiXmlElement* elem, void* hWnd );
    void SaveConsolePlacement( TiXmlElement* elem );
    void LoadConsolePlacement( TiXmlElement* elem );

    namespace GrdProp
    {
        typedef enum
        {
            cells, col_widths, row_selections
        } T;
    };
    typedef std::set<GrdProp::T> GrdProps;
    void SaveGrd( TiXmlElement* elem, TStringGrid*);
    void LoadGrd( TiXmlElement* elem, TStringGrid*);

}; // namespace TiXML
//------------------------------------------------------------------------------
namespace TiXMLAtr
{
TiXmlAttribute* GetNode(TiXmlElement*, const std::string& attributeName);

//------------------------------------------------------------------------------
typedef std::pair<TiXmlElement*,std::string> ElemName;
typedef std::pair<ElemName, PStringAdapter> Holder;
Holder CreateHolder( TiXmlElement*, const std::string&, StringAdapter* adtp );
//------------------------------------------------------------------------------
template <typename T>
Holder CreateHolderRef( TiXmlElement* elem, const std::string& id, T& ref )
{
    return CreateHolder( elem, id, new DefaultStrAdpt<T>(ref) );
}
//------------------------------------------------------------------------------
std::string GetStr( TiXmlElement* elem, const std::string& id, const std::string& def = "");
void SetStr( TiXmlElement* elem, const std::string& id, const std::string& val );
//------------------------------------------------------------------------------
template<typename T>
void Set( TiXmlElement* elem, const std::string& id, const T& val )
{
    SetStr( elem, id, lexical_cast<std::string>(val) );
}
//------------------------------------------------------------------------------
template<typename T> T Get( TiXmlElement* elem, const std::string& id, const T& val )
{
    return lexical_cast_def<T,std::string>( GetStr( elem, id, lexical_cast<std::string>(val) ), val );
}
//------------------------------------------------------------------------------
template<>
void Set<std::string>( TiXmlElement* elem, const std::string& id, const std::string& val )
{
    SetStr( elem, id, val );
}
template<>
std::string Get<std::string>( TiXmlElement* elem, const std::string& id, const std::string& val )
{
    return GetStr( elem, id, val );
}    
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
template<typename T> void Extract( TiXmlElement *xml, const std::string& id, T& v)
{
    v = Get<T>(xml, id, v);
    Set<T>(xml, id, lexical_cast<std::string>(v));
}
//------------------------------------------------------------------------------
template<typename T> void ExtractMinMax( TiXmlElement *xml, std::pair<T,T>& par)
{
    ExtractAtr( xml, "минимум", par.first);
    ExtractAtr( xml, "максимум", par.second);
}
//------------------------------------------------------------------------------
}; // namespace TiXML
//------------------------------------------------------------------------------
namespace TiXMLText
{
//------------------------------------------------------------------------------
std::string GetStr(TiXmlNode* elem, const std::string& def = "");
void SetStr( TiXmlNode* elem, const std::string& s);
//------------------------------------------------------------------------------
template <typename T> void Set(TiXmlElement* elem, const T& val)
{
    SetStr(elem, lexical_cast<std::string>(val) );
}
//------------------------------------------------------------------------------
template <typename T> T Get(TiXmlElement* elem, const T& val)
{
    return lexical_cast_def<T,std::string>( GetStr( elem, lexical_cast<std::string>(val) ), val );
}
//------------------------------------------------------------------------------
template<> void Set<std::string>(TiXmlElement* elem, const std::string& val)
{
    SetStr(elem, val );
}
template<> std::string Get<std::string>(TiXmlElement* elem, const std::string& val)
{
    return GetStr(elem, val);
}
//------------------------------------------------------------------------------
}; // namespace TiXMLText
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
