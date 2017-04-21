//------------------------------------------------------------------------------
#ifndef MY_PNEUMOBLOCK_HEADER_INCLUDED______
#define MY_PNEUMOBLOCK_HEADER_INCLUDED______
//------------------------------------------------------------------------------
#include <system.hpp>
#include <vector>
//------------------------------------------------------------------------------
#include "pneumo_gas.hpp"
//------------------------------------------------------------------------------
namespace Grids
{
    class TStringGrid;
};

namespace Pneumo
{
    void SwitchGas(unsigned);
    void SwitchOffIgnoreConnectionError();
    void AddSwitchGasTask(unsigned nGas);
    unsigned Current();
    unsigned CurrentDuration();
	const Gases& GetGases();
    Grids::TStringGrid *GetControl();

    double Conc(unsigned nGas);
    void SaveConc();

    AnsiString PneumoblockGasAction2Str(unsigned nGas);
    AnsiString BlowGasAction2Str(unsigned nGas, unsigned blowTimeMSec);

    void PlaneSwitchGasTask( unsigned nGas );
    unsigned Count() { return GetGases().size(); }
};


//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
