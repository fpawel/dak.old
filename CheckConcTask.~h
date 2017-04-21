//------------------------------------------------------------------------------
#ifndef CheckConcTaskH
#define CheckConcTaskH
//------------------------------------------------------------------------------
#include "task_.h"
//------------------------------------------------------------------------------
void ClearTestIndicationData();
//------------------------------------------------------------------------------
class GetModbusConcTask : public ForEachAddys
{
public:
    explicit GetModbusConcTask();
private:
    virtual AnsiString ClassId() const { return "Опросить_концентрацию"; }
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
};
//------------------------------------------------------------------------------
class GetStendTask : public ForEachAddys
{
public:
    explicit GetStendTask();
private:
    virtual AnsiString ClassId() const { return "Опросить_стенд"; }
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
};
//------------------------------------------------------------------------------
class FixTestResultTask : public ForEachAddys
{
public:
    explicit FixTestResultTask(unsigned nTest=-1, unsigned nPGS=-1,
        const std::vector<unsigned> &nParams = std::vector<unsigned>() );
private:
    virtual AnsiString ClassId() const { return "Фиксировать_результат_проверки"; }
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
    virtual AnsiString What() const;
    virtual void DoSaveXML(TiXmlElement* elem) const;
    virtual void DoLoadXML(TiXmlElement* elem);
    unsigned nTest_, nPGS_;
    std::vector<unsigned> params_;

    static AnsiString Format(unsigned nTest, unsigned nPGS, const std::vector<unsigned> &nParams);
};

class FixTemperatureValTask : public ForEachAddys
{
public:
    explicit FixTemperatureValTask(unsigned gas=0, unsigned nPt = 0);
private:
    virtual AnsiString ClassId() const { return "Фиксировать_для_термокомпенсации"; }
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
    virtual AnsiString What() const;
    virtual void DoSaveXML(TiXmlElement* elem) const;
    virtual void DoLoadXML(TiXmlElement* elem);

    static AnsiString Format(unsigned gas, unsigned nPt);

    unsigned gas_;
    unsigned nPt_;
};

class CheckStabilityTask : public ForEachAddys
{
public:
    explicit CheckStabilityTask();
    virtual void PerformActionForAddy(unsigned addy, bool& isComplete, bool& isFailed);
private:
    std::set<unsigned> addys_;
    virtual AnsiString ClassId() const { return "Проверка_стабильности"; }
    virtual void PerformBegin();
    virtual void PerformEnd();

    void DoSaveXML(TiXmlElement* elem) const
    {
        tmHlpr_.SaveXML(elem);
    }

    void DoLoadXML(TiXmlElement* elem)
    {
        tmHlpr_.LoadXML(elem);
    }

    bool DoSetupParams()
    {
        return SetupParamsExt();
    }

};
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
