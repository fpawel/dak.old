//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "varsimpl.h"
#include "MyModFN.hpp"
//------------------------------------------------------------------------------
#include <strstream>
#include <vector>
#include <fstream>
//------------------------------------------------------------------------------
#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"
#include "boost\regex.hpp"
#include "boost\assign\list_of.hpp"
//------------------------------------------------------------------------------
#include "MyIostream.h"
#include "AnsiStringUtils_.h"
#include "guicon.h"
#include "MyModFN.hpp"
#include "xmlUtils_.h"
#include "MyModFN.hpp"
#include "MyWindow.h"
#include "MyNoDestroySingleton.hpp"
#include "MyStrUtils.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
namespace DAK {
//------------------------------------------------------------------------------
namespace Cmd {
//------------------------------------------------------------------------------
AnsiString Code2Str(unsigned code)
{
    for( unsigned i=0; i<Idx::count; ++i )
        if( Code::all[i]==code )
            return name[i];
            return " ";
}
//------------------------------------------------------------------------------
}; // namespace Cmd
//------------------------------------------------------------------------------
const std::string sKefsFileName = MyGetExePath()+"\\kefs.xml";
const char *kefsFileName = sKefsFileName.c_str();
//------------------------------------------------------------------------------
unsigned ExtractKNumOfMask(const Kefs& kefs, const AnsiString &mask, AnsiString& sError )
{
    const unsigned kefsCount = kefs.size();
    for( unsigned i=0; i<kefsCount; ++i )
        if(kefs[i].caption.LowerCase().Pos(mask.LowerCase()) )
            return i;
    sError += mask+"\n";
    return -1;
}
//------------------------------------------------------------------------------
class ImplT : public boost::noncopyable
{
public:
    explicit ImplT();
	const Kefs& GetKefs() const
    {
        return kefs_;
    }



private:
    const Kefs kefs_;
    AnsiString sError_;
    unsigned ExtrSpc(const char mask[])
    {
        return ExtractKNumOfMask(kefs_, mask, sError_ );
    }
public:
    std::vector<unsigned> mainKef_;
};
//------------------------------------------------------------------------------
Kefs LoadKefs();
//------------------------------------------------------------------------------
ImplT::ImplT()	: kefs_( LoadKefs() ), mainKef_( MainKef::count )
{
    using namespace MainKef;
    mainKef_[SoftVer]   = ExtrSpc("Версия программы");
    mainKef_[Year]      = ExtrSpc("Год выпуска");
    mainKef_[Serial]    = ExtrSpc("Серийный номер");
    mainKef_[Gas]       = ExtrSpc("Газ");   
    mainKef_[Units]     = ExtrSpc("Единицы измерения");
    mainKef_[Scale]     = ExtrSpc("Тип шкалы");
    mainKef_[Scale0]    = ExtrSpc("Начало шкалы");
    mainKef_[ScaleK]    = ExtrSpc("Конец шкалы");
    mainKef_[Isp]       = ExtrSpc("Исполнение");
    mainKef_[PGS1]      = ExtrSpc("ПГС1");
    mainKef_[PGS3]      = ExtrSpc("ПГС3");
    mainKef_[DevType]   = ExtrSpc("Тип прибора");
    mainKef_[Diap]      = ExtrSpc("Диапазон измерений");

    mainKef_[Porog1]    = ExtrSpc("Порог1");
    mainKef_[Porog2]    = ExtrSpc("Порог2");

    if(!sError_.IsEmpty())
        MyMessageBox(NULL, MYSPRINTF_(
        "Не найдены следующие к-ты в списке kefs.xml:\n%s", sError_).c_str(),
        "kefs.xml", MB_SYSTEMMODAL | MB_APPLMODAL | MB_OK | MB_ICONERROR );
}
//------------------------------------------------------------------------------
Kefs LoadKefs()
{
    boost::shared_ptr<TiXmlDocument>  doc( TiXML::CreateDocument(kefsFileName));
    Kefs ret;
    TiXmlElement* mainElem = doc->RootElement();
    if( !mainElem ) return ret;

    for( TiXmlElement* elem = mainElem->FirstChildElement();
        elem;
        elem = elem->NextSiblingElement() )
    {
        if( !strcmp( elem->Value(), "Кэф") ) {
            Kef kef;
            kef.n = TiXMLAtr::Get(elem, "n", kef.n);
            kef.def = TiXMLAtr::GetStr(elem, "def").c_str();
            kef.caption = TiXMLText::GetStr(elem).c_str();

            TiXmlElement* elemProps = TiXmlHandle( elem ).Child(1).ToElement();
            if(elemProps && !std::strcmp(elemProps->Value(),"vals")) {
                for( TiXmlElement* elemVal = elemProps->FirstChildElement();
                    elemVal; elemVal = elemVal->NextSiblingElement() )
                        if(!std::strcmp(elemVal->Value(),"val")){
                            Kef::Prop prop;
                            prop.key = TiXMLAtr::GetStr(elemVal, "key", "").c_str(),
                            prop.val = TiXMLAtr::GetStr(elemVal, "val", "").c_str();
                            if(!prop.key.IsEmpty()&&!prop.val.IsEmpty())
                                kef.props.push_back( prop );
                        }
            }
            ret.push_back(kef);
        }
    }
    return ret;
}
//------------------------------------------------------------------------------
struct ImplTTag {};
typedef MyNoDestroySingletonHolder<ImplT, ImplTTag> Impl;
//------------------------------------------------------------------------------
const Kefs& GetKefs()
{
	return Impl().Instance().GetKefs();
}
//------------------------------------------------------------------------------
void SaveKefsList()
{
    ::DeleteFile(kefsFileName);
    boost::shared_ptr<TiXmlDocument>  doc( TiXML::CreateDocument(kefsFileName));

    TiXmlElement* mainElem = TiXML::ChildElement( doc.get(), "Кэфы" );
    const Kefs& kefs = GetKefs();
    const unsigned sz = kefs.size();     

    for( unsigned i=0; i<sz; ++i )
    {
        const Kef& kef = kefs[i];
        if( kef.caption.IsEmpty() ) continue;
        TiXmlElement* eKef = mainElem->LinkEndChild( new TiXmlElement( "Кэф" ) )->ToElement();

        TiXMLAtr::Set(eKef, "n", kef.n);
        TiXMLAtr::Set(eKef, "def", kef.def);
        TiXMLText::SetStr(eKef, kef.caption.c_str() );
        if( kef.props.empty() ) continue;
        TiXmlElement* eProps = eKef->LinkEndChild( new TiXmlElement( "vals" ) )->ToElement();
        for( unsigned nVal=0; nVal<kef.props.size(); ++nVal ) {
            TiXmlElement* eVal = eProps->LinkEndChild( new TiXmlElement( "val" ) )->ToElement();
            TiXMLAtr::Set(eVal, "key", kef.props[nVal].key);
            TiXMLAtr::Set(eVal, "val", kef.props[nVal].val);
        }
    }
    doc->SaveFile();
}
//------------------------------------------------------------------------------
AnsiString Kef::Val2Key(const AnsiString &val) const
{
    if(props.empty()) return val;
    for(unsigned i=0; i<props.size(); ++i)
        if( val==props[i].val )
            return props[i].key.c_str();
    return val;
}
//------------------------------------------------------------------------------
AnsiString Kef::Key2Val(const AnsiString &key) const
{
    if(props.empty()) return key;
    for(unsigned i=0; i<props.size(); ++i)
        if( key==props[i].key )
            return props[i].val.c_str();
    return key;
}
//------------------------------------------------------------------------------
void Kef::SetStringsKeys( TStrings* strs ) const
{
    strs->Clear();
    for( unsigned i=0; i<props.size();++i )
        strs->Add( props[i].key );
}
//------------------------------------------------------------------------------
unsigned Kef::GetNumberOfKNum(unsigned kNum)
{
    const Kefs& kefs = GetKefs();
    for( unsigned i=0; i<kefs.size(); ++i )
        if( kefs[i].n==kNum )
            return i;
    return -1;
}

namespace MainKef
{
//------------------------------------------------------------------------------
const unsigned* Num()
{
    return &Impl().Instance().mainKef_.at(0);
}
//------------------------------------------------------------------------------
const DAK::Kef& GetKef(unsigned n)
{
    return DAK::GetKefs().at( GetKefNum(n) );
}
unsigned GetKefNum(unsigned n)
{
    assert(n<count);
    assert(Num()[n]<DAK::GetKefs().size());
    return Num()[n];
}
//------------------------------------------------------------------------------
Scale0K ExtractScaleOfCode(const AnsiString &codeScale)
{
    return ExtractScaleOfValue( GetKef(Scale).Val2Key(codeScale) );
}
//------------------------------------------------------------------------------
Scale0K ExtractScaleOfValue(const AnsiString &ss)
{
    std::string s = ss.c_str();

    std::string::const_iterator xItStart = s.begin(), xItEnd = s.end(), xIt = xItStart;
    boost::cmatch xResults;
    const char numRegEx[] = "\\s*([0-9]+)\\s*\\-\\s*([0-9]+)\\s*";
    if( !boost::regex_search(xIt, xItEnd, xResults, boost::regex(numRegEx) ) )
        return Scale0K();

    return Scale0K(
        std::string( xResults[1].first, xResults[1].second ).c_str(),
        std::string( xResults[2].first, xResults[2].second ).c_str() );
}

//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
};//namespace DAK
//------------------------------------------------------------------------------


