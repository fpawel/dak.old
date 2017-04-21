//---------------------------------------------------------------------------
#ifndef MasterSlaveIOSettingsIniH
#define MasterSlaveIOSettingsIniH
//---------------------------------------------------------------------------
#include "MasterSlaveIOSettings.hpp"
#include "tinyxml.h"

void SaveMasterSlaveIOSettingsToFile(const MasterSlaveIOSettings&,
    TiXmlElement *elem);

void LoadMasterSlaveIOSettingsFromFile(MasterSlaveIOSettings&,
    TiXmlElement *elem);


#endif
