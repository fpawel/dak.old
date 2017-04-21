//------------------------------------------------------------------------------
#ifndef ScenariesH
#define ScenariesH
//------------------------------------------------------------------------------
#include "MyAbstructTransfer.hpp"
//------------------------------------------------------------------------------
PTransfer CreateDefaultScenary();

void SaveScenaryToXML( const AnsiString& fn, const PTransfer scn );
PTransfer LoadScenaryFromXML( const AnsiString& fn);







//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
