//---------------------------------------------------------------------------
#ifndef DefScnRefsH
#define DefScnRefsH
//---------------------------------------------------------------------------
#include "MyAbstructTransfer.hpp"

#define PUSH_BACK_TASK_ scenary.push_back( PTransfer
#define INIT_STRS_ Strings( boost::assign::list_of< AnsiString >
#define INIT_NUMS_ std::vector<unsigned>( boost::assign::list_of< unsigned >
//------------------------------------------------------------------------------
typedef std::vector<AnsiString> Strings;
//------------------------------------------------------------------------------
const char
    BLOW_NULL_ADJUST_ID[] = "корректировка_нуля_продувка",
    BLOW_SENS_ADJUST_ID[] = "корректировка_чувствительности_продувка",
    BLOW_NULL_AFTER_ADJUST_ID[] = "продувка_воздуха_после_калибровки",
    DELAY_AFTER_SEND_COMAND_ID[] = "задержка_после_отправки_команды",
    BLOW_1_1_ID[] =     "продувка_ПГС1_1",
    BLOW_2_2_ID[] =     "продувка_ПГС2_2",
    BLOW_3_3_ID[] =     "продувка_ПГС3_3",
    BLOW_2_4_ID[] =     "продувка_ПГС2_4",
    BLOW_1_5_ID[] =     "продувка_ПГС1_5",
    BLOW_3_6_ID[] =     "продувка_ПГС3_6",
    BLOW_1_7_ID[] =     "продувка_ПГС1_7",
    CHECK_STAB_TEST_ID[] =  "проверка_нестабильности",
    T20SHRT_ID[] =       "Progrev20short",
    TEH20_ID[] =         "Texprogon20",
    TEH78_ID[] =         "Texprogon78",
    TEST_CHANGE_POWER_ID[] =  "test_change_power",
    
    INITIALIZE_DAK_ID[] =     "инициализация_коэфициентов",
    ADJUST_CURRENT_ID[] =      "калибровка_тока"
    ;

//------------------------------------------------------------------------------
void PushBackReferences(Scenary& scenary, const AnsiString& s);
void PushBackReferences(Scenary& scenary, const Strings& strs );
//------------------------------------------------------------------------------
PTransfer CreateAdjustNullTask( unsigned nTest );
PTransfer CreateAdjustSensTask(unsigned nTest);
//------------------------------------------------------------------------------
void CreateTest7PGSTask(Scenary& scenary, unsigned nTest);
//------------------------------------------------------------------------------
PTransfer CreateAutomationCheckScenary();
PTransfer CreateAdjustCurrentScenary();
PTransfer CreateAdjustNullTask( unsigned nTest );
PTransfer CreateAdjustSensTask(unsigned nTest);
PTransfer CreateSetMainKefsScenary();
PTransfer CreateCheckChangePowerScenary();
PTransfer CreateCheckMainPogrScenary();

PTransfer CreateTermocompensation();
PTransfer Create_TestHart();

namespace ReferenceTaskInstance
{
    PTransfer Create();
};
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
