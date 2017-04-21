//------------------------------------------------------------------------------
#ifndef MY_ABSTRUCT_TRANSFER_HEADER_INCLUDED_____
#define MY_ABSTRUCT_TRANSFER_HEADER_INCLUDED_____
//------------------------------------------------------------------------------
#include <utility>
#include <vector>
#include <map>
//------------------------------------------------------------------------------
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
//------------------------------------------------------------------------------
#include <system.hpp>
//------------------------------------------------------------------------------
#include "..\my_include\free_types_.h"
//------------------------------------------------------------------------------
// элемент XML-дерева для вода-вывода
class TiXmlElement;
//------------------------------------------------------------------------------
class MyTransferTask;
//------------------------------------------------------------------------------
typedef boost::shared_ptr<MyTransferTask> PTransfer;
//------------------------------------------------------------------------------
typedef std::vector< PTransfer > Scenary;
typedef std::map< AnsiString, PTransfer > Id2TransferObjectMapT;
//------------------------------------------------------------------------------
class MyTransferTask : public boost::noncopyable
{
public:
    virtual ~MyTransferTask() {}

    virtual AnsiString  ClassId() const {return "";}
    virtual AnsiString  What() const { return " "; }
    AnsiString          ObjectId() const {return objectId_;}

	// Обработка события начала трансфера. Возвращает - выполнять/не выполнять
	bool HandleBeginTransfer();
    // Обработка события окончания трансфера. Возвращает - повторить/не повторять
	bool HandleEndTransfer();
	// выполнить. task.cpp
    void Perform();

    // ввод/вывод
    void SaveXML(TiXmlElement* node) const;
    void LoadXML(TiXmlElement* node);

    virtual PTransfer   GetRef() { return PTransfer(); }
    virtual Scenary     Items() { return Scenary(); }

    void SetObjetId(const AnsiString& objectId) { objectId_ = objectId; }
    void RootAceptReference();
    bool SetupParams();
    void SetScenaryVar(const AnsiString& name, const AnsiString& value) {
        scnVars_[name] = value;
    }
    static AnsiString   GetCommonScenaryValue(const AnsiString& name);
    static void         Sleep(unsigned msec, bool showInfoPanel = true, const AnsiString& s="" );
    static void SkipCurrentOperation() {
        operationIsSkiped_ = true;
    }
    static bool MyTransferTask::MustCancelOperation();
private:
    virtual void PerformAction() = 0;
    virtual void DoSaveXML(TiXmlElement* node) const {}
    virtual void DoLoadXML(TiXmlElement* node) {}
    virtual bool DoSetupParams() {return true;}
    virtual bool DoHandleBeginTransfer() { return true; }
    virtual bool DoHandleEndTransfer() = 0;

    void CalculateChildrenClassesIds(Id2TransferObjectMapT& id2obj);
    void FindReference(const Id2TransferObjectMapT&);
    void CalculateReference();
    virtual void DoFindReference(const Id2TransferObjectMapT&) {}
    virtual void DoCalculateReference() {}

    AnsiString objectId_;

    typedef std::map<AnsiString, AnsiString> ScnVarsListT;
    typedef ScnVarsListT::iterator IScnVarsList;
    ScnVarsListT scnVars_;

    static ScnVarsListT commonScenaryVariables_;
    static bool operationIsSkiped_;
    friend class InitializeTask;

};
//------------------------------------------------------------------------------

class NamedTransfer : public MyTransferTask
{
public:
    explicit NamedTransfer(const AnsiString& what) : what_(what) {}
    virtual ~NamedTransfer() {}
	virtual AnsiString What() const { return what_; }
private:
    const AnsiString what_;
};
//------------------------------------------------------------------------------


#endif