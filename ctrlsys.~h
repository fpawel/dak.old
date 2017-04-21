//------------------------------------------------------------------------------
#ifndef ctrlsysH
#define ctrlsysH
//------------------------------------------------------------------------------
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
//------------------------------------------------------------------------------
#include "MyNoDestroySingleton.hpp"
#include "MyAbstructTransfer.hpp"
//------------------------------------------------------------------------------
class TransferManagerT;
class ModbusAdapter;
class MasterSlaveIOImpl;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class CtrlSysImpl : public boost::noncopyable
{
public:
    explicit CtrlSysImpl();
    
    struct IOSets
    {
        ModbusAdapter *modbus;
        MasterSlaveIOImpl* heatCham;
        unsigned pneumoAddy, stendAddy;
        AnsiString lastErrorSource;
        bool isDeviceNoAnswer;
    };
    IOSets GetIOSets();  

    struct DAKSets
    {
       unsigned    endScale;
       bool        isCH;
       AnsiString  gas, softVer, ispolnenie, scale, units;
    };
    DAKSets GetDAKSets();

    void SetupDialog();
    void Adjust(unsigned cmd, double pgs);

    AnsiString FormatTestParam(unsigned nTest, unsigned nPGS, const std::vector<unsigned> &params);
    AnsiString FormatTestParam(unsigned nTest, unsigned nPGS, unsigned param);

private:
    class Impl;
    boost::shared_ptr<Impl> impl_;
};

struct CtrlSysTag {};
typedef MyNoDestroySingletonHolder<CtrlSysImpl, CtrlSysTag> CtrlSys;

bool IsNotAnsweredAddy(unsigned addy);
bool CheckIndication(unsigned addy, double &conc, double pgs, double dConcMax);
void AdjustNull();
void AdjustEnd();
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
