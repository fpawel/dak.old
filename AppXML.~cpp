//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "AppXML.h"
//------------------------------------------------------------------------------
#include "boost/noncopyable.hpp"
//------------------------------------------------------------------------------
#include "MyModFN.hpp"
#include "MyNoDestroySingleton.hpp"
//------------------------------------------------------------------------------
#include "xmlUtils_.h"
#include "MyStrUtils.h"
//------------------------------------------------------------------------------
//#include <sysutils.hpp>
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
class ThisLibraryT : public boost::noncopyable
{

public:
	explicit ThisLibraryT();
	AppXML GetAppXML() const
    {
        AppXML appXML;
        appXML.doc = doc;
        appXML.main = main;
        appXML.windowsPlacements = windowsPlacements;
        appXML.config = config;
        appXML.form1 = form1;
        appXML.pneumoBlock = pneumoBlock;
        return appXML;
    }

private:
    const std::string fn;
	TiXmlDocument* doc;
	TiXmlElement *main, *windowsPlacements, *config, *form1, *pneumoBlock;

};
//------------------------------------------------------------------------------
ThisLibraryT::ThisLibraryT() :
    fn( ChangeFileExtension(MyGetExeFileName(),"xml") ),
    doc( TiXML::CreateDocument( fn.c_str() ) ),
    main( TiXML::ChildElement( doc, "main" ) ),
    config( TiXML::ChildElement( main, "config" ) ),
    pneumoBlock( TiXML::ChildElement( config, "pneumo" ) ),
    form1( TiXML::ChildElement( main, "form1" ) ),
    windowsPlacements( TiXML::ChildElement( main, "Расположение_окон" ) )
{

}
//------------------------------------------------------------------------------
struct ThisLibTagT {};
typedef MyNoDestroySingletonHolder<ThisLibraryT, ThisLibTagT > ThisLib;
//------------------------------------------------------------------------------
AppXML GetAppXML()
{
    return ThisLib().Instance().GetAppXML();
}
//------------------------------------------------------------------------------

