//------------------------------------------------------------------------------
#ifndef task_H
#define task_H
//------------------------------------------------------------------------------
//std

#include <set>
#include <map>

//my
#include "MyAbstructTransfer.hpp"
#include "MyExcpt.hpp"
#include "scriptTask.h"
//------------------------------------------------------------------------------
class TiXmlElement;
//------------------------------------------------------------------------------
    class MessageBoxTask : public ScenaryAction
    {
    public:
        explicit MessageBoxTask(const AnsiString& title = "", const AnsiString& msg = "",
            unsigned flags = MB_OK);
    private:
        virtual void PerformAction();
        virtual AnsiString ClassId() const { return "Сообщение"; }
        virtual AnsiString What() const;
        virtual void DoSaveXML(TiXmlElement* node) const;
        virtual void DoLoadXML(TiXmlElement* node);
        virtual void OnBeginTransfer(bool);

        AnsiString title_, msg_;
        unsigned flags_;
    };
//--------------------------------------------------------------------------
class BlowGas : public ForEachAddys
{
public:
    explicit BlowGas(int gas =0, unsigned blowTimeMSec=1000);
private:
    int gas_;

    virtual void PerformBegin();
    virtual void PerformEnd();
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);

    AnsiString What() const;
    virtual AnsiString ClassId() const { return "Продувка_с_опросом"; }
    virtual void DoSaveXML(TiXmlElement* node) const;
    virtual void DoLoadXML(TiXmlElement* node);
    virtual bool DoSetupParams();
};
//--------------------------------------------------------------------------
    class SwitchOffPneumoTask : public ScenaryAction
    {
    public:
        explicit SwitchOffPneumoTask();
    private:
        virtual void PerformAction();
        virtual AnsiString ClassId() const { return "Выключить_пневмоблок"; }

    };
    //--------------------------------------------------------------------------
    class DelayTask : public ScenaryAction
    {
    public:
        explicit DelayTask(unsigned msec = 1000);
        AnsiString What() const;
    private:
        TimeLimitedTaskHelper tmHlpr_;
        virtual void PerformAction();
        virtual AnsiString ClassId() const { return "Задержка"; }
        virtual void DoSaveXML(TiXmlElement* node) const;
        virtual void DoLoadXML(TiXmlElement* node);
        virtual bool DoSetupParams() { return tmHlpr_.SetupParams(What());  }
    };
    //--------------------------------------------------------------------------
    
    

    //--------------------------------------------------------------------------
    class ManualTestTask : public ForEachAddys
    {
    public:
        explicit ManualTestTask();
    private:
        virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);

        virtual AnsiString ClassId() const { return "Ручная_настройка"; }
        virtual void DoSaveXML(TiXmlElement* node) const {}
        virtual void DoLoadXML(TiXmlElement* node) {}
    };
    class ManualGetTemperatureTask : public ScenaryAction
    {
    public:
        explicit ManualGetTemperatureTask();
    private:
        virtual void PerformAction();
        virtual AnsiString ClassId() const { return "измерение_температуры"; }
        virtual void DoSaveXML(TiXmlElement* node) const {}
        virtual void DoLoadXML(TiXmlElement* node) {}

    };
    //--------------------------------------------------------------------------
    class WriteModbusTask : public ForEachAddys
    {
    public:
        explicit WriteModbusTask( unsigned cmd = 0, double val = 0);
    private:
        unsigned cmd_;
        double val_;

        virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);

        virtual AnsiString ClassId() const { return "Отправка_команды_модбас"; }
        virtual AnsiString What() const;
        virtual void DoSaveXML(TiXmlElement* node) const;
        virtual void DoLoadXML(TiXmlElement* node);
    };


    class WriteModbusBroadcastTask : public ScenaryAction
    {
    public:
        explicit WriteModbusBroadcastTask( unsigned cmd = 0, double val = 0);
    private:
        unsigned cmd_;
        double val_;

        virtual void PerformAction();

        virtual AnsiString ClassId() const { return "Отправка_широковещательной_команды_модбас"; }
        virtual AnsiString What() const;
        virtual void DoSaveXML(TiXmlElement* node) const;
        virtual void DoLoadXML(TiXmlElement* node);
    };
    
    //--------------------------------------------------------------------------
    class SetAddyTask : public ScenaryAction
    {
    public:
        explicit SetAddyTask(unsigned addy);
    private:
        unsigned addy_;
        virtual void PerformAction();
    };
    //--------------------------------------------------------------------------
    class SendHeatChamControlCommandTask : public ScenaryAction
    {
    public:
        typedef enum
        {
            setpoint, start, stop, fix
        } Cmd;
        explicit SendHeatChamControlCommandTask(Cmd cmd = stop, int t=0);
        AnsiString  ClassId() const {return "Управление_термокамерой";}
        static AnsiString FormatCmd(Cmd cmd);
    private:
        const Cmd cmd_;
        int t_;
        virtual void PerformAction();
    };
    //--------------------------------------------------------------------------
    class SendHeatChamSetpointTask : public ForEachAddys
    {
    public:
        explicit SendHeatChamSetpointTask(int t = 200);
        virtual AnsiString ClassId() const { return "Уставка_термокамеры"; }
        virtual AnsiString What() const;
    private:
        int t_;
        virtual void PerformBegin();
        virtual void PerformEnd();
        virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);

        virtual void DoSaveXML(TiXmlElement* node) const;
        virtual void DoLoadXML(TiXmlElement* node);
        virtual bool DoSetupParams();
        bool IsComplete() const;
    };
    //--------------------------------------------------------------------------


    class CheckDeviceConnectTask : public ForEachAddys
    {
    public:
        explicit CheckDeviceConnectTask(unsigned msec = 3*60*60*1000);
        virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
    private:
        virtual AnsiString ClassId() const { return "Проверка_наличия_связи_с_приборами"; }
        virtual AnsiString What() const;
        virtual void DoSaveXML(TiXmlElement* node) const;
        virtual void DoLoadXML(TiXmlElement* node);
        virtual bool DoSetupParams();
    };
//------------------------------------------------------------------------------
class ProccessKoefs : public ForEachAddys
{
public:
    explicit ProccessKoefs( const AnsiString& what, const std::vector<unsigned>& kefsNums);
	virtual ~ProccessKoefs() {}
    virtual AnsiString What() const;
private:
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
    virtual void ProccessItem(unsigned addy, unsigned kNum) = 0;

    virtual void DoSaveXML(TiXmlElement* node) const;
    virtual void DoLoadXML(TiXmlElement* node);

    std::vector<unsigned> kefsNums_;
};


class WriteKefs1Task : public ForEachAddys
{
public:
    explicit WriteKefs1Task(const std::vector<unsigned>& kefs = std::vector<unsigned>() );
	virtual ~WriteKefs1Task() {}
    virtual AnsiString ClassId() const { return "Запись_кефов_1"; }
    virtual AnsiString What() const;
private:
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
    virtual void DoSaveXML(TiXmlElement* node) const;
    virtual void DoLoadXML(TiXmlElement* node);

    std::vector<unsigned> kefs_;
};

//------------------------------------------------------------------------------
class ReadKoefsTask : public ProccessKoefs
{
public:
    explicit ReadKoefsTask(const std::vector<unsigned>& kefsNums = std::vector<unsigned>() );
	virtual ~ReadKoefsTask(){}
    virtual AnsiString ClassId() const { return "Считывание_кефов"; }
private:
    virtual void ProccessItem(unsigned slave, unsigned kNum);
};
//------------------------------------------------------------------------------
class WriteKoefsTask : public ProccessKoefs
{
public:
    explicit WriteKoefsTask(const std::vector<unsigned>& kefsNums = std::vector<unsigned>() );
	virtual ~WriteKoefsTask() {}
    virtual AnsiString ClassId() const { return "Запись_кефов"; }
private:
    virtual void ProccessItem(unsigned slave, unsigned kNum); 
};
//------------------------------------------------------------------------------
class AdjustCurrentTask : public ForEachAddys
{
public:
    explicit AdjustCurrentTask(unsigned msec = 1*1000);
	virtual ~AdjustCurrentTask() {}
private:
    TimeLimitedTaskHelper tm_;
    virtual AnsiString ClassId() const { return "Калибровка_тока"; }   

    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
    virtual void DoSaveXML(TiXmlElement* elem) const{    tm_.SaveXML(elem); }
    virtual void DoLoadXML(TiXmlElement* elem)      {    tm_.LoadXML(elem); }

    virtual bool DoSetupParams() { return tm_.SetupParams(What());  }
    virtual void PerformBegin();
    virtual void PerformEnd();

};
//------------------------------------------------------------------------------
class SetPorogsTask : public ForEachAddys
{
public:
    enum { kNominal, k90percent, k5percent, kindsCount  };
    explicit SetPorogsTask(unsigned kind = kNominal);
	virtual ~SetPorogsTask() {}
private:
    unsigned kind_;
    static AnsiString Format(unsigned kind)
    {
        const char *s[kindsCount] = { "на выпуск в эксплуатацию", "90%", "5%" };
        return AnsiString("Установка порогов ")+( kind<kindsCount ? +s[kind] : "..." );
    }
    virtual AnsiString ClassId() const { return "Установка_порогов"; }
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
    virtual void DoSaveXML(TiXmlElement* elem) const;
    virtual void DoLoadXML(TiXmlElement* elem);
    virtual AnsiString What() const { return Format(kind_); }
};
//------------------------------------------------------------------------------
class AdjustConcTask : public ForEachAddys
{
public:
    explicit AdjustConcTask(bool isEndScale = true);
	virtual ~AdjustConcTask() {}
private:
    bool isEndScale_;
    static AnsiString Format(bool isEndScale) { return AnsiString().sprintf("Калибровка %s шкалы",
        isEndScale ? "конца" : "нуля"); }
    virtual AnsiString ClassId() const { return "Калибровка_показаний"; }
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
    virtual void DoSaveXML(TiXmlElement* elem) const;
    virtual void DoLoadXML(TiXmlElement* elem);
    virtual AnsiString What() const { return Format(isEndScale_); }
};
//------------------------------------------------------------------------------
class SelectGasTask : public ForEachAddys
{
public:
    explicit SelectGasTask() : ForEachAddys("Отпавить_команду_выбора_газа") {}
	virtual ~SelectGasTask() {}
private:
    bool isEndScale_;
    virtual AnsiString ClassId() const { return "Отпавить_команду_выбора_газа"; }
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
};

void runTestHart();
class TestHartTask : public ScenaryAction
    {
    public:
        explicit TestHartTask() : ScenaryAction("Проверка HART проткола") {}
    private:

        virtual AnsiString ClassId() const { return "TestHartTask"; }
        virtual AnsiString What() const { return "Проверка HART проткола"; }
        //virtual void DoSaveXML(TiXmlElement* node) const {}
        //virtual void DoLoadXML(TiXmlElement* node){}
        //virtual void OnBeginTransfer(bool) {}
        virtual void PerformAction()
        {
           runTestHart();
        }
    };

#endif
//---------------------------------------------------------------------------

