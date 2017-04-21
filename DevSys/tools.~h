//------------------------------------------------------------------------------
#ifndef toolsH
#define toolsH
//------------------------------------------------------------------------------
#include "MyExcpt.hpp"
#include <vector>
#include <map>

#include "vardef.hpp"


namespace DAK
{
    void InitializeConc();
    struct ConcT
    {
        double conc;
        AnsiString sConc;
    };
    ConcT GetModbusConc(unsigned addy, bool enableLog = true);


    struct DevState
    {
        double current;
        bool rele[RelePin::count];
        AnsiString sIout, sConcI, sPorog1, sPorog2;
    };

    DevState GetDevState(unsigned addy, bool enableLog = true);

    void WriteKef(unsigned addy, unsigned kNum);
    double ReadKef(unsigned addy, unsigned kNum);

    double ReadVar1(unsigned addy, bool enableLog);
    double ReadT(unsigned addy, bool enableLog);
    double ReadIlampWorkkRefk (unsigned addy, bool enableLog);
    double ReadIlampOn (unsigned addy, bool enableLog);
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
