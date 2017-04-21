//---------------------------------------------------------------------------


#pragma hdrstop

#include "MasterSlaveIOSettingsIni.h"

#include "xmlUtils_.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//------------------------------------------------------------------------------

#define WRITE_DELAY_ID "Задержка_записи_мсек"
#define TIME_OUT_ID "таймаут_ожидания_ответа_мсек"
#define TRANSFER_REPEAT_COUNT "количество_повторов_запроса"
#define RXD_FRAME_SILENT_TIME "длительность_ожидания_символа_приёма_мсек"
#define MUST_I_LOG_COMPORT_DATA "лагать"

void LoadMasterSlaveIOSettingsFromFile(MasterSlaveIOSettings& sets_,
    TiXmlElement *elem)
{
    //port_->LoadSettingsFromIniFile(ini_, sekt_);
    sets_.tmWriteDelay_ = TiXMLAtr::Get(elem, WRITE_DELAY_ID, 0);
    sets_.timeOut_      = TiXMLAtr::Get(elem, TIME_OUT_ID, 2000);
    sets_.silentTime_   = TiXMLAtr::Get(elem, RXD_FRAME_SILENT_TIME, 50);
    sets_.repeatCount_  = TiXMLAtr::Get(elem, TRANSFER_REPEAT_COUNT, 1);
    sets_.mustLogData_  = TiXMLAtr::Get(elem, MUST_I_LOG_COMPORT_DATA, false);
}
//------------------------------------------------------------------------------
void SaveMasterSlaveIOSettingsToFile(const MasterSlaveIOSettings& sets_,
    TiXmlElement *elem)
{
    //port_->SaveSettingsToIniFile(ini_, sekt_);
    TiXMLAtr::Set(elem, TIME_OUT_ID, sets_.timeOut_ );
    TiXMLAtr::Set(elem, RXD_FRAME_SILENT_TIME, sets_.silentTime_ );
    TiXMLAtr::Set(elem, WRITE_DELAY_ID, sets_.tmWriteDelay_ );
    TiXMLAtr::Set(elem, MUST_I_LOG_COMPORT_DATA, sets_.mustLogData_ );
	TiXMLAtr::Set(elem, TRANSFER_REPEAT_COUNT,  sets_.repeatCount_ );
}