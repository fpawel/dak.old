//------------------------------------------------------------------------------
#pragma hdrstop
#include "scriptTask.h"
#include "guicon.h"
#include "Devices.h"
#include "MasterSlaveIO.h"
#include "TransferManage.h"
#include "xmlUtils_.h"
#include "ScenaryFactory.h"
#include "AnsiStringUtils_.h"
#include "pneumo.hpp"
#include "HeatChamber.h"
#include "MySynchronize.h"
#include "Unit1.h"
#include "MyWindow.h"
#include "ctrlsys.h"
#include "ModbusAdapter.h"
#include "vardef.hpp"
#include "pasport.h"
#include "uFrmWait.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
MyTransferTask::ScnVarsListT MyTransferTask::commonScenaryVariables_;
bool MyTransferTask::operationIsSkiped_ = false;
//------------------------------------------------------------------------------
AnsiString MyTransferTask::GetCommonScenaryValue(const AnsiString& name)
{
    IScnVarsList i = MyTransferTask::commonScenaryVariables_.find(name);
    return i==MyTransferTask::commonScenaryVariables_.end() ? AnsiString("") : i->second;
}
//------------------------------------------------------------------------------
bool MyTransferTask::MustCancelOperation()
{
    return !Form1 || operationIsSkiped_ || TransferManager::Instance().IsThreadStopedOrTerminated();
}
void MyTransferTask::Sleep(unsigned msec, bool showInfoPanel, const AnsiString& s )
{
    TransferManagerT &tmngr = TransferManager::Instance();
    if(showInfoPanel)    {
        const AnsiString ss = s.IsEmpty() ? MYSPRINTF_("Задержка %s",
            FormatMillisecond2Minutes(msec) ) : s;
        tmngr.ShowTopPanel( ss, true );
        tmngr.SetTopPanelProgress( 0, msec, 0 );
        tmngr.SetEnableTopPanelTimeProgress(1);
    }
    const unsigned moment0 = ::GetTickCount();

    MyTransferTask::operationIsSkiped_ = false;
    while( !MustCancelOperation() && GetTickCount()-moment0 < msec )
        tmngr.SyncronizedSleep(1);
    if(showInfoPanel)
        tmngr.HideTopPanel();
}
//------------------------------------------------------------------------------
void MyTransferTask::Perform()
{
    AnsiString s1 = What();
    AnsiString s2 = ClassId();
    RichTextAttr reas = RichTextAttr().TextColor(clNavy). FontStyles(TFontStyles() <<
        fsUnderline).TextSize(9).Time();
    if(!GetRef())
        Form1->AddLogAll("\""+s1+"\". "+"Старт.", reas );

    MyTransferTask::operationIsSkiped_ = false;
    for( IScnVarsList i=scnVars_.begin(); i!=scnVars_.end(); ++i )
        MyTransferTask::commonScenaryVariables_[i->first] = i->second;
    PerformAction();

    if(!GetRef())
        Form1->AddLogAll("\""+s1+"\". "+"Окончание.", reas );
}
//------------------------------------------------------------------------------
void MyTransferTask::SaveXML(TiXmlElement* node) const
{
    node->Clear();
    TiXMLAtr::Set(node, "описание", What() );
    TiXMLAtr::Set(node, "id", objectId_ );
    if( !scnVars_.empty() ) {
        TiXmlElement* varsElem = TiXML::ChildElement(node, "переменные");
        for( ScnVarsListT::const_iterator i=scnVars_.begin(); i!=scnVars_.end(); ++i )
            TiXMLText::SetStr( TiXML::ChildElement(varsElem, i->first.c_str() ),
                i->second.c_str() );
    }
    DoSaveXML(node);
}
//------------------------------------------------------------------------------
void MyTransferTask::LoadXML(TiXmlElement* node)
{
    objectId_ = TiXMLAtr::Get(node, "id", objectId_);
    TiXmlElement* varsElem = node->FirstChildElement("переменные");
    if(varsElem)  for( TiXmlElement* varElem = varsElem->FirstChildElement();
        varElem; varElem = varElem->NextSiblingElement() )
        scnVars_[varElem->Value()] = TiXMLText::GetStr(varElem).c_str();
    DoLoadXML(node);
}
//------------------------------------------------------------------------------
bool MyTransferTask::SetupParams()
{
    return DoSetupParams();
}
//------------------------------------------------------------------------------
// Обработка события начала трансфера. Возвращает - выполнять/не выполнять
bool MyTransferTask::HandleBeginTransfer()
{
    AnsiString s1 = What();
    AnsiString s2 = ClassId();
    const bool
        r = DoHandleBeginTransfer(),
        mustSkip = Form1->MustSkipTask(this);
    return mustSkip ? false : r;
}
//------------------------------------------------------------------------------
// Обработка события окончания трансфера. Возвращает - повторить/не повторять
bool MyTransferTask::HandleEndTransfer()
{
    return DoHandleEndTransfer();
}
//------------------------------------------------------------------------------
void MyTransferTask::CalculateChildrenClassesIds(Id2TransferObjectMapT& id2obj)
{
    AnsiString what = What();
    AnsiString id1 = ClassId();
    AnsiString objid = ObjectId();
    for( unsigned i=0; i<Items().size(); ++i)
    {
        PTransfer task = Items()[i];
        const AnsiString id = task->objectId_;
        if( !id.IsEmpty() )
        {
            if( !id2obj.insert( std::make_pair(id, task) ).second )
                MY_THROW_(MYSPRINTF_( "Обнаружено дублирование идентификаторов операций \"%s\"!", id ));
        }
        task->CalculateChildrenClassesIds(id2obj);
    }
}
//------------------------------------------------------------------------------
void MyTransferTask::RootAceptReference()
{
    Id2TransferObjectMapT id2obj;
    CalculateChildrenClassesIds(id2obj);
    FindReference(id2obj);
    CalculateReference();
}
//------------------------------------------------------------------------------
void MyTransferTask::FindReference(const Id2TransferObjectMapT& id2obj)
{
    DoFindReference(id2obj);
    for( unsigned i=0; i<Items().size(); ++i)
        Items()[i]->FindReference(id2obj);

}
//------------------------------------------------------------------------------
void MyTransferTask::CalculateReference()
{
    DoCalculateReference();
    for( unsigned i=0; i<Items().size(); ++i)
        Items()[i]->CalculateReference();
}

//------------------------------------------------------------------------------
ScenaryAction::ScenaryAction(const AnsiString& what) : NamedTransfer(what), isFirstCicle_(1)
{
}
//------------------------------------------------------------------------------
ScenaryAction::~ScenaryAction()
{
}
//------------------------------------------------------------------------------
bool ScenaryAction::DoHandleBeginTransfer()
{
    AnsiString s = this->ClassId();
    OnBeginTransfer(isFirstCicle_);
    isFirstCicle_ = false;
    const bool ret = !MustBreakTransfersProcessing();
    if(ret)
        excpt_.reset();
    return ret;
}
//------------------------------------------------------------------------------
bool ScenaryAction::DoHandleEndTransfer()
{
    OnEndTransfer();
    return false;
}
//------------------------------------------------------------------------------
bool ScenaryAction::MustBreakTransfersProcessing() const
{
    if( !Devs::HasOneGoodSelectedAddy() )
        return true;
    if(!excpt_ || excpt_->TypeOf<MyNoAnswerException>() )
        return false;
    return true;
}

//------------------------------------------------------------------------------
ChainedScenaryAction::ChainedScenaryAction(const Scenary& script,
        const AnsiString& what ) :
    ScenaryAction(""), script_(script), what_(what)
{}
//------------------------------------------------------------------------------
void ChainedScenaryAction::PerformAction()
{
    TransferManagerT& tmng = TransferManager::Instance();
    for(unsigned i=0; i<script_.size() && !MyTransferTask::MustCancelOperation(); ++i )
        script_[i]->Perform();
}
//------------------------------------------------------------------------------
void ChainedScenaryAction::DoSaveXML(TiXmlElement* node) const
{
    if( !what_.IsEmpty() ) node->SetAttribute("описание", what_.c_str() );
    node->Clear();
    for(unsigned i=0; i<script_.size() ; ++i )
    {
        PTransfer tsk = script_[i];
        const AnsiString id = tsk->ClassId();
        TiXmlElement* elem = node->LinkEndChild( new TiXmlElement( id.c_str() ) )->ToElement();
        tsk->SaveXML(elem);
    }
}
//------------------------------------------------------------------------------
void ChainedScenaryAction::DoLoadXML(TiXmlElement* elem)
{
    what_ = StrReplaceSymbols( TiXMLAtr::Get(elem, "описание", what_), '_', ' ');
    script_.clear();
    for( elem = elem->FirstChildElement(); elem; elem=elem->NextSiblingElement() )
    {
        const AnsiString id = elem->Value();
        PTransfer task ( CreateScenaryAction(id) );
        task->LoadXML(elem);
        script_.push_back(task);
    }
}
//------------------------------------------------------------------------------
AnsiString ChainedScenaryAction::What() const
{
    if(!what_.IsEmpty()) return what_;
    AnsiString ret;
    for(unsigned i=0; i<script_.size() ; ++i )
        ret = ret + script_[i]->What() + " ";
    return ret;
}
//------------------------------------------------------------------------------
bool ChainedScenaryAction::DoSetupParams()
{
    for(unsigned i=0; i<script_.size() ; ++i )
    {
        if( !script_[i]->SetupParams() )
            return 0;
    }
    return 1;
}
//------------------------------------------------------------------------------
ReferenceTask::ReferenceTask(const AnsiString &refId) :
    ScenaryAction("Ссылка на \""+refId+"\""), refId_(refId), ref1_(), ref_()
{}
//------------------------------------------------------------------------------
AnsiString ReferenceTask::What() const
{
    return ref_ ? ref_->What() : "Ссылка на \""+refId_+"\"";
}
//------------------------------------------------------------------------------
bool ReferenceTask::DoHandleBeginTransfer()
{
    AnsiString s = What();
    return ref_->HandleBeginTransfer();
}
//------------------------------------------------------------------------------
bool ReferenceTask::DoHandleEndTransfer()
{
    return ref_->HandleEndTransfer();
}
//------------------------------------------------------------------------------
Scenary ReferenceTask::Items()
{
    return ref_ ? ref_->Items() : Scenary();
}
//------------------------------------------------------------------------------
void ReferenceTask::PerformAction()
{
    ref_->Perform();
}
//------------------------------------------------------------------------------
bool ReferenceTask::DoSetupParams()
{
    return ref_->SetupParams();
}
//------------------------------------------------------------------------------
void ReferenceTask::DoFindReference(const Id2TransferObjectMapT& id2ref)
{
    AnsiString what = What();
    AnsiString id = ClassId();
    AnsiString objid = ObjectId();
    Id2TransferObjectMapT::const_iterator e = id2ref.end(), i = id2ref.find(refId_);
    if(i==e)
        MY_THROW_(MYSPRINTF_( "Обнаружена неинициализированная ссылка на операцию \"%s\"!", refId_ ));
    ref1_ = i->second;
}
//------------------------------------------------------------------------------
void ReferenceTask::DoCalculateReference()
{
    PTransfer task = ref1_;
    for( ; task->GetRef(); task = task->GetRef() )
    {
        if( task.get()==this )
            MY_THROW_(MYSPRINTF_( "Обнаружена циклическая ссылка на операцию \"%s\"!", refId_ ));
    }
    if( !task )
        MY_THROW_(MYSPRINTF_( "Не удалось разыменивать ссылку на операцию \"%s\"!", refId_ ));

    ref_ = task;
}
//------------------------------------------------------------------------------
void ReferenceTask::DoSaveXML(TiXmlElement* node) const
{
    TiXMLAtr::Set(node, "ref", refId_ );
}
void ReferenceTask::DoLoadXML(TiXmlElement* node)
{
    refId_ = TiXMLAtr::Get(node, "ref", refId_);
}
//------------------------------------------------------------------------------
InitializeTask::InitializeTask() : ForEachAddys("Старт сценария")
{
}
//------------------------------------------------------------------------------
void InitializeTask::PerformBegin()
{
    MyTransferTask::commonScenaryVariables_.clear();
	Pneumo::SwitchGas(-1);
}
//------------------------------------------------------------------------------
void InitializeTask::PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed)
{
    
}
//------------------------------------------------------------------------------
FinalizeTask::FinalizeTask() : ScenaryAction("Завершение сценария")
{
}
//------------------------------------------------------------------------------
void FinalizeTask::PerformAction()
{
    try
    {
        Pneumo::SwitchOffIgnoreConnectionError();
    } catch(...){ }
    my::SynchronizeMthd( &Report, __FILE_LINE__);
}
//------------------------------------------------------------------------------
LRESULT FinalizeTask::Report()
{

    AnsiString msg;
    msg.sprintf(
        "Выбранная проверка\n\"%s\"\nзавершена %s.\n"
        "Результаты смотрите в прооколе проверки\nи таблице сценария.",
            Form1->cbCurentScenary->Text,
            Form1->ScenaryErrorFixed() ? "с ошибками" : "без ошибок" );
    const unsigned flags = MB_SYSTEMMODAL | MB_APPLMODAL | MB_OK |
        ( Form1->ScenaryErrorFixed() ? MB_ICONERROR : MB_ICONWARNING );
    TransferManager::Instance().HideTopPanel();
    MyMessageBox(NULL, msg.c_str(), "Сценарий", flags );


    return 0;
}
//------------------------------------------------------------------------------
AddysDeqT MakeAddysList()
{
    AddysDeqT addys_;
    const Devs::Nums selected = Devs::GetSelectedAddys();
    for( unsigned i=0; i<selected.size(); ++i )
    {
        const unsigned addy = selected[i];
        addys_.push_back( addy);
    }
    return addys_;
}
//------------------------------------------------------------------------------
ForEachAddys::ForEachAddys(const AnsiString& what, unsigned timeLimitMS)
    :
    ScenaryAction( what), addys_( MakeAddysList() ), tmHlpr_(timeLimitMS)
{
}
//------------------------------------------------------------------------------
void ForEachAddys::PerformAction()
{
    CtrlSysImpl& ctrlSys = CtrlSys::Instance();
    CtrlSysImpl::IOSets io = CtrlSys::Instance().GetIOSets();
    ModbusAdapter& modbus = *io.modbus;

    TransferManagerT& tmngr = TransferManager::Instance();
    addys_ = MakeAddysList();

    PerformBegin();
    if( Form1 && !Devs::HasOneGoodSelectedAddy() )
        MY_THROW_("Отсутсвуют активные приборы!");

    if( tmHlpr_.MSec()!=-1 ) {
        tmngr.ShowTopPanel( What(), true );
        tmngr.SetTopPanelProgress( 0, tmHlpr_.MSec(), 0 );
        tmngr.SetEnableTopPanelTimeProgress(1);
        tmHlpr_.Start();
    }

	while( tmHlpr_.NotEnough() && !addys_.empty()  )
    {
        tmngr.SyncronizedSleep(1);

        bool isComplete = true;
        const unsigned addy = addys_.front();
        if( Devs::IsSelectedAddy(addy) && !IsNotAnsweredAddy(addy) )
        {
            try
            {
                bool isFailed = 0;
                PerformActionForAddy(addy, isComplete, isFailed);
                if( isFailed )
                    failed_.push_back(addy);
            }
            catch(...) {
                const MyException *excpt = MY_RETHROW_.get();
                const MyNoAnswerException *eNoAns = excpt->As<MyNoAnswerException>();
                if( eNoAns && eNoAns->IO()==&modbus.MasterSlave() && modbus.Addy()==addy ) {
                    failed_.push_back(addy);
                    Form1->LogError( "Не отвечает!");
                } else throw;
            }
        }
        addys_.pop_front();
        if(!isComplete) addys_.push_back(addy);
    }
    PerformEnd();
}
//------------------------------------------------------------------------------
bool TimeLimitedTaskHelper::SetupParams(const AnsiString& what)
{
    AnsiString sS = msec_/1000;
	if( !InputQuery( what, what+". Введите длительность в секундах.", sS) )
        return 0;
    msec_ = 1000*StrToIntDef(sS, 1);
    return 1;
}
//------------------------------------------------------------------------------
AnsiString TimeLimitedTaskHelper::ToStr() const
{
    return FormatMillisecond2Minutes(msec_)+" ч:мин:с";
}
//------------------------------------------------------------------------------
void TimeLimitedTaskHelper::Start()
{
    tmStart_ = ::GetTickCount();
}
//------------------------------------------------------------------------------
unsigned TimeLimitedTaskHelper::Duration() const
{
    return ::GetTickCount()-tmStart_;
}
//------------------------------------------------------------------------------
void TimeLimitedTaskHelper::SaveXML(TiXmlElement* elem) const
{
    TiXMLAtr::Set( elem, "длительность_мс", msec_ );
}
//------------------------------------------------------------------------------
void TimeLimitedTaskHelper::LoadXML(TiXmlElement* elem)
{
    msec_ = TiXMLAtr::Get( elem, "длительность_мс", msec_);
}
//------------------------------------------------------------------------------
bool TimeLimitedTaskHelper::NotEnough() const
{
    return Form1 && !MyTransferTask::MustCancelOperation() && ( msec_==-1 || Duration()<MSec() );
}
//------------------------------------------------------------------------------


