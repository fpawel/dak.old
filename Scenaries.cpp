//------------------------------------------------------------------------------
#pragma hdrstop
#include "Scenaries.h"
//------------------------------------------------------------------------------
#include "boost/assign/list_of.hpp"
//------------------------------------------------------------------------------
#include <forms.hpp>
//------------------------------------------------------------------------------
#include <math>
#include <map>
//------------------------------------------------------------------------------
#include "task_.h"
#include "..\my_include\MyNoDestroySingleton.hpp"
#include "guicon.h"
#include "pneumo.hpp"

#include "AnsiStringUtils_.h"
#include "ctrlsys.h"
#include "vardef.hpp"
#include "ScenaryFactory.h"
#include "xmlUtils_.h"
#include "MyWindow.h"
#include "MyStrUtils.h"
#include "CheckConcTask.h"
#include "my_array_size.h"
#include "DefScnRefs.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
PTransfer CreateManualTestScenary()
{
    Scenary scenary;
    PUSH_BACK_TASK_( new ManualTestTask ));
    return PTransfer( new ChainedScenaryAction(scenary, "Ручная настройка" ) );
}
//------------------------------------------------------------------------------

PTransfer CreateDefaultScenary()
{
    Scenary scenary;
    scenary.push_back( CreateManualTestScenary() );
    PUSH_BACK_TASK_( CreateSetMainKefsScenary() ));
    PUSH_BACK_TASK_( CreateAdjustCurrentScenary() ));
    PUSH_BACK_TASK_( CreateAdjustNullTask(0) ));
    PUSH_BACK_TASK_( CreateAdjustSensTask(0) ));
    scenary.push_back( CreateCheckMainPogrScenary() );
    PushBackReferences(scenary, INIT_STRS_
        (TEST_CHANGE_POWER_ID)
        (CHECK_STAB_TEST_ID)
        (TEH20_ID)
        (TEH78_ID) ));
    scenary.push_back( CreateTermocompensation() );
    scenary.push_back( Create_TestHart() );

    scenary.push_back( CreateAutomationCheckScenary() );
    scenary.push_back( ReferenceTaskInstance::Create() );



    PTransfer ret( new ChainedScenaryAction(scenary, "Сценарий" ) );
    try
    {
        ret->RootAceptReference();
    }
    catch(...)
    {
        ::MessageBox( Screen->ActiveForm->Handle,
            MY_RETHROW_->Message().c_str(),
                "", MB_OK | MB_APPLMODAL | MB_ICONERROR  );
    }
    return ret;
}
//------------------------------------------------------------------------------

void SaveTaskXML( TiXmlElement* parElem, const PTransfer task )
{
    parElem->Clear();
    const AnsiString id = task->ClassId().c_str(), what = task->What();
    assert( !id.IsEmpty() );
    assert( !what.IsEmpty() );
    TiXmlElement* elem = parElem->LinkEndChild( new TiXmlElement( id.c_str() ) )->ToElement();
    task->SaveXML(elem);
}
//------------------------------------------------------------------------------
TiXmlElement* GetReferencedElement1(TiXmlElement* elem)
{
    TiXmlDocument* doc = elem->GetDocument();
    TiXmlHandle xml(doc);

    //TiXmlElement* ret = 0;
    for( ; elem; elem=elem->NextSiblingElement() )
    {
        const char *s = elem->Value();
        const unsigned n = StrToIntDef( elem->Attribute("n"), 0 );
        xml = xml.ChildElement( s, n );
    }
    return xml.ToElement();
}
//------------------------------------------------------------------------------
PTransfer CreateTaskFromXML(TiXmlElement* elem)
{
    const AnsiString id = elem->Value();
    assert( !id.IsEmpty() );
    MyTransferTask* pTask = CreateScenaryAction(id);
    PTransfer task ( pTask );
    task->LoadXML(elem);
    return task;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void SaveScenaryToXML( const AnsiString& fn, const PTransfer scn )
{
    ::DeleteFile( fn.c_str() );
    TiXmlDocument* doc = TiXML::CreateDocument( fn.c_str() );
    TiXmlElement* mainElem = doc->LinkEndChild( new TiXmlElement( "Сценарий" ) )->ToElement();
    scn->SaveXML(mainElem);
    doc->SaveFile( fn.c_str() );
}
//------------------------------------------------------------------------------
PTransfer LoadScenariesXML1( const AnsiString& fn)
{
    boost::shared_ptr<TiXmlDocument> doc( new TiXmlDocument );
    if( !doc->LoadFile( fn.c_str() )  )
    {
        AnsiString msg;
        msg.sprintf("%d:%d %s", doc->ErrorRow(), doc->ErrorCol(), doc->ErrorDesc() );
        MyMessageBox(NULL, msg.c_str(), "Не соответствие формата файла сценария!",
            MB_OK | MB_APPLMODAL | MB_ICONERROR );
        return PTransfer();
    }

    TiXmlElement* elem = doc->FirstChildElement();
    if(!elem) return PTransfer();
    return CreateTaskFromXML(elem);
}
//------------------------------------------------------------------------------
PTransfer LoadScenaryFromXML( const AnsiString& fn)
{
    try
    {
        PTransfer ret = LoadScenariesXML1( fn );
        ret->RootAceptReference();
        return ret;
    }
    catch(...)
    {
        ::MessageBox( Screen->ActiveForm->Handle,
                MY_RETHROW_->Message().c_str(),
                "", MB_OK | MB_APPLMODAL | MB_ICONERROR  );
    }
    return PTransfer();
}
//------------------------------------------------------------------------------
