//------------------------------------------------------------------------------
#ifndef scriptTaskH
#define scriptTaskH
//------------------------------------------------------------------------------
#include "MyAbstructTransfer.hpp"
#include "MyExcpt.hpp"
//------------------------------------------------------------------------------
#include <deque>
//------------------------------------------------------------------------------
class ScenaryAction : public NamedTransfer
{
public:
    explicit ScenaryAction(const AnsiString& what);
    virtual ~ScenaryAction();
private:
    PMyExcpt excpt_;


protected:
    bool MustBreakTransfersProcessing() const;
    bool isFirstCicle_;

    virtual bool DoHandleBeginTransfer();
    virtual bool DoHandleEndTransfer();
    virtual void OnBeginTransfer(bool isFirstCicle) {}
    virtual void OnEndTransfer() {}
};
//------------------------------------------------------------------------------
class ChainedScenaryAction : public ScenaryAction
{
public:
    ChainedScenaryAction(const Scenary& script = Scenary(),
        const AnsiString& what = "" );
    virtual ~ChainedScenaryAction() {}
    virtual AnsiString What() const;
    virtual AnsiString ClassId() const {return "Сценарий";}

    virtual Scenary Items() { return script_; }
protected:
    Scenary script_;
private:
    virtual void PerformAction();
    virtual void DoSaveXML(TiXmlElement* node) const;
    virtual void DoLoadXML(TiXmlElement* node);
    virtual bool DoSetupParams();
    AnsiString what_;
};
//------------------------------------------------------------------------------
class ReferenceTask : public ScenaryAction
{
public:
    ReferenceTask(const AnsiString& refId = "");
    virtual ~ReferenceTask() {}
    virtual AnsiString ClassId() const {return "Ссылка";}

    PTransfer GetReferencedTask() { return ref_; }


private:
    virtual AnsiString What() const;

	virtual bool DoHandleBeginTransfer();
    virtual bool DoHandleEndTransfer();

    virtual Scenary Items();
    virtual void DoFindReference(const Id2TransferObjectMapT&);
    virtual void PerformAction();
    virtual void DoSaveXML(TiXmlElement* node) const;
    virtual void DoLoadXML(TiXmlElement* node);
    virtual bool DoSetupParams();

    virtual PTransfer GetRef() { return ref1_; }
    virtual void DoCalculateReference();

    AnsiString refId_, arg1_;
    PTransfer ref1_, ref_;

};
//------------------------------------------------------------------------------
class TimeLimitedTaskHelper
{
public:
    explicit TimeLimitedTaskHelper(unsigned msec) : msec_(msec), tmStart_(0) {}
    unsigned MSec() const { return msec_; }
    unsigned Duration() const;
    AnsiString ToStr() const;
    void Start();
    void SetMSec(unsigned msec) { msec_ = msec; }
    bool SetupParams(const AnsiString& what);
    void SaveXML(TiXmlElement* node) const;
    void LoadXML(TiXmlElement* node);

    bool NotEnough() const;

private:
    unsigned msec_, tmStart_;
};
//------------------------------------------------------------------------------
typedef std::deque<unsigned> AddysDeqT;
//------------------------------------------------------------------------------
class ForEachAddys : public ScenaryAction
{
public:
    explicit ForEachAddys(const AnsiString& what, unsigned timeLimitMS = -1);
    virtual ~ForEachAddys() {}
    const AddysDeqT& GetFailedAddys() const { return failed_; }
    bool SetupParamsExt() { return tmHlpr_.SetupParams(What());  }
private:
    AddysDeqT addys_, failed_;

    virtual void PerformAction();
    virtual void PerformBegin() {}
    virtual void PerformEnd() {}
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed) = 0;
protected:
    TimeLimitedTaskHelper tmHlpr_;
};
//--------------------------------------------------------------------------
class InitializeTask : public ForEachAddys
{
public:
    explicit InitializeTask();
private:
    virtual void PerformBegin();
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
    virtual AnsiString ClassId() const { return "Инициализация"; }
};
//--------------------------------------------------------------------------
class FinalizeTask : public ScenaryAction
{
public:
    explicit FinalizeTask();
private:
    virtual void PerformAction();
    virtual AnsiString ClassId() const { return "Завершение"; }
    LRESULT Report();
};
//--------------------------------------------------------------------------





#endif
