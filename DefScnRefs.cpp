//---------------------------------------------------------------------------


#pragma hdrstop

#include "DefScnRefs.h"
//------------------------------------------------------------------------------
#include "boost/assign/list_of.hpp"
//------------------------------------------------------------------------------
#include <forms.hpp>
//------------------------------------------------------------------------------
#include <math>
#include <map>
//------------------------------------------------------------------------------
#include "task_.h"
#include "..\my_include\MyNoDestroySingleton.hpp"
#include "..\my_include\win\MyModFN.hpp"
#include "guicon.h"
#include "pneumo.hpp"

#include "AnsiStringUtils_.h"
#include "ctrlsys.h"
#include "vardef.hpp"
#include "ScenaryFactory.h"
#include "xmlUtils_.h"
#include "MyWindow.h"
#include "MyStrUtils.h"
#include "CheckConcTask.h"
#include "my_array_size.h"
#include "Unit1.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------

enum
{
    SEC_MS = 1000,
    MINUTE_MS = SEC_MS*60,
    HOUR_MS = MINUTE_MS * 60,
    TUNE_LIMIT_MSEC = 1*MINUTE_MS
};
//------------------------------------------------------------------------------
void PushBackReferences(Scenary& scenary, const AnsiString& s)
{
    scenary.push_back( PTransfer( new ReferenceTask(s)));
}
//------------------------------------------------------------------------------
void PushBackReferences(Scenary& scenary, const Strings& strs )
{
    for( unsigned n=0; n<strs.size(); ++n)
        scenary.push_back( PTransfer( new ReferenceTask(strs[n] )));
}
//------------------------------------------------------------------------------
namespace ReferenceTaskInstance
{
//------------------------------------------------------------------------------
PTransfer AdjustNull()
{
    Scenary scenary;
    PUSH_BACK_TASK_( new BlowGas(0, 3*MINUTE_MS ) ));

    PUSH_BACK_TASK_( new AdjustConcTask(false) ));
    PushBackReferences(scenary, DELAY_AFTER_SEND_COMAND_ID );
    PUSH_BACK_TASK_( new GetModbusConcTask ));
    //PUSH_BACK_TASK_( new GetStendTask ));
    PTransfer ret( new ChainedScenaryAction(scenary, BLOW_NULL_ADJUST_ID ) );
    ret->SetObjetId(BLOW_NULL_ADJUST_ID);
    return ret;
}
//------------------------------------------------------------------------------
PTransfer AdjustSens()
{
    Scenary scenary;
    PUSH_BACK_TASK_( new BlowGas(Scn::SENS_GAS_IDX, 3*MINUTE_MS ) ));

    PUSH_BACK_TASK_( new AdjustConcTask(true) ));
    PushBackReferences(scenary, DELAY_AFTER_SEND_COMAND_ID );

    PUSH_BACK_TASK_( new GetModbusConcTask ));
    //PUSH_BACK_TASK_( new GetStendTask ));
    PTransfer ret( new ChainedScenaryAction(scenary, BLOW_SENS_ADJUST_ID ) );
    ret->SetObjetId(BLOW_SENS_ADJUST_ID);
    return ret;
}
//------------------------------------------------------------------------------
void Blow7PGS(Scenary &scenaryOut, const AnsiString& blowTaskId, unsigned gasIdx)
{
    const AnsiString s = MYSPRINTF_("Продувка ПГС%d", gasIdx+1);
    Scenary scenary;
    PUSH_BACK_TASK_( new BlowGas(gasIdx, 1*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new WriteModbusTask(DAK::Cmd::Code::reset, 0) ));
    PushBackReferences(scenary, DELAY_AFTER_SEND_COMAND_ID );

    PUSH_BACK_TASK_( new GetStendTask ));
    scenaryOut.push_back( PTransfer( new ChainedScenaryAction(scenary,s)));
    scenaryOut.back()->SetObjetId(blowTaskId+"_1");

    scenary.clear();
    PUSH_BACK_TASK_( new BlowGas(gasIdx, 2*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new GetStendTask ));
    PUSH_BACK_TASK_( new GetModbusConcTask ));
    scenaryOut.push_back( PTransfer( new ChainedScenaryAction(scenary,s)));
    scenaryOut.back()->SetObjetId(blowTaskId+"_2");
}
//------------------------------------------------------------------------------
void BlowLastPGS1(Scenary &scenaryOut)
{
    Scenary scenary;

    PUSH_BACK_TASK_( new BlowGas(0, 1*MINUTE_MS ) ));

    PUSH_BACK_TASK_( new WriteModbusTask(DAK::Cmd::Code::reset, 0) ));
    PushBackReferences(scenary, DELAY_AFTER_SEND_COMAND_ID );
    PUSH_BACK_TASK_( new GetStendTask ));
    scenaryOut.push_back( PTransfer( new ChainedScenaryAction(scenary, "Проверка порогов в точке 7, ПГС1")));
    scenaryOut.back()->SetObjetId(BLOW_1_7_ID);
}
//------------------------------------------------------------------------------

PTransfer CheckStability()
{
    Scenary scenary;
    PUSH_BACK_TASK_( new BlowGas(Scn::SENS_GAS_IDX, 3*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new CheckStabilityTask ));
    PUSH_BACK_TASK_( new SwitchOffPneumoTask ));

    PTransfer ret( new ChainedScenaryAction(scenary, "Проверка нестабильности показаний" ) );
    ret->SetObjetId(CHECK_STAB_TEST_ID);
    return ret;
}
//------------------------------------------------------------------------------
PTransfer Progrev20Short()
{
    Scenary scenary;
    PUSH_BACK_TASK_( new SwitchOffPneumoTask ));
    PUSH_BACK_TASK_( new SendHeatChamSetpointTask(200) ));
    PUSH_BACK_TASK_( new CheckDeviceConnectTask(3*60*60*1000) ));
    PTransfer ret( new ChainedScenaryAction(scenary, "Прогрев 20, три часа" ) );
    ret->SetObjetId(T20SHRT_ID);
    return ret;
}
//------------------------------------------------------------------------------
PTransfer Texprogon20()
{
    Scenary scenary;
    using namespace DAK::Test;
    PushBackReferences(scenary, T20SHRT_ID);
    scenary.push_back( CreateAdjustNullTask(tech_20_3) );
    scenary.push_back( CreateAdjustSensTask(tech_20_3) );
    CreateTest7PGSTask(scenary, tech_20_3);
    PUSH_BACK_TASK_( new SwitchOffPneumoTask ));
    PUSH_BACK_TASK_( new SendHeatChamSetpointTask(200) ));
    PUSH_BACK_TASK_( new CheckDeviceConnectTask(16*60*60*1000) ));
    CreateTest7PGSTask(scenary, tech_20_16);
    return PTransfer( new ChainedScenaryAction(scenary, DAK_TEST_TECHPROGON_20 ) );
}
//------------------------------------------------------------------------------
PTransfer Texprogon78()
{
    Scenary scenary;
    using namespace DAK::Test;
    PushBackReferences(scenary, T20SHRT_ID);
    scenary.push_back( CreateAdjustNullTask(tech_78_3) );
    scenary.push_back( CreateAdjustSensTask(tech_78_3) );
    CreateTest7PGSTask(scenary, tech_78_3);
    PUSH_BACK_TASK_( new SwitchOffPneumoTask ));
    PUSH_BACK_TASK_( new SendHeatChamSetpointTask(780) ));
    PUSH_BACK_TASK_( new CheckDeviceConnectTask(16*60*60*1000) ));
    PushBackReferences(scenary, T20SHRT_ID);
    CreateTest7PGSTask(scenary, tech_78_19);
    return PTransfer( new ChainedScenaryAction(scenary, DAK_TEST_TECHPROGON_78 ) );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
PTransfer MainKefs()
{
    Scenary scenary;
    const DAK::Sets sets = DAK::Sets::Get();

    std::vector<unsigned> kefs;
    ParseArrayOfIntervals("0-3 5-11 323 334 325 328", kefs);
    
    
    PUSH_BACK_TASK_( new WriteKefs1Task(kefs) ));
    PUSH_BACK_TASK_( new DelayTask(1*SEC_MS ) ));
    PUSH_BACK_TASK_( new SelectGasTask ));
    PTransfer ret( new ChainedScenaryAction(scenary, "Инициализация коэфициентов" ) );
    ret->SetObjetId(INITIALIZE_DAK_ID);
    return ret;
}
//------------------------------------------------------------------------------
PTransfer Create()
{
    Scenary scenary;

    scenary.push_back( Progrev20Short() );

    scenary.push_back( Texprogon78() );
    scenary.back()->SetObjetId(TEH78_ID);

    scenary.push_back( Texprogon20() );
    scenary.back()->SetObjetId(TEH20_ID);

    scenary.push_back( CreateCheckChangePowerScenary() );
    scenary.back()->SetObjetId(TEST_CHANGE_POWER_ID);

    scenary.push_back( PTransfer( new AdjustCurrentTask ) );
    scenary.back()->SetObjetId(ADJUST_CURRENT_ID);

    PUSH_BACK_TASK_( new BlowGas(0, 3*MINUTE_MS ) ));
    scenary.back()->SetObjetId(BLOW_NULL_AFTER_ADJUST_ID);

    PUSH_BACK_TASK_( new DelayTask(3*SEC_MS ) ));
    scenary.back()->SetObjetId(DELAY_AFTER_SEND_COMAND_ID);


    Blow7PGS(scenary, BLOW_1_1_ID, 0 );
    Blow7PGS(scenary, BLOW_2_2_ID, 1 );
    Blow7PGS(scenary, BLOW_3_3_ID, 2 );
    Blow7PGS(scenary, BLOW_2_4_ID, 1 );
    Blow7PGS(scenary, BLOW_1_5_ID, 0 );
    Blow7PGS(scenary, BLOW_3_6_ID, 2 );

    BlowLastPGS1(scenary);

    scenary.push_back( CheckStability() );
    scenary.push_back( AdjustNull() );
    scenary.push_back( AdjustSens() );
    scenary.push_back( MainKefs() );

    return PTransfer( new ChainedScenaryAction(scenary, "Private" ) );
}
//------------------------------------------------------------------------------
}; //namespace ReferenceTaskInstance
//------------------------------------------------------------------------------

void TermocompensationPoint(int temperature, unsigned nPt, Scenary& scenary)
{
    PUSH_BACK_TASK_( new SwitchOffPneumoTask ));
    PUSH_BACK_TASK_( new SendHeatChamSetpointTask(temperature) ));
    PUSH_BACK_TASK_( new CheckDeviceConnectTask(3*HOUR_MS) ));

    PUSH_BACK_TASK_( new BlowGas(0, 3*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new FixTemperatureValTask(0, nPt ) ));

    PUSH_BACK_TASK_( new BlowGas(1, 3*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new FixTemperatureValTask(1, nPt ) ));

    PUSH_BACK_TASK_( new BlowGas(2, 3*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new FixTemperatureValTask(2, nPt ) ));

    PUSH_BACK_TASK_( new BlowGas(0, 1*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new SwitchOffPneumoTask ));
    PUSH_BACK_TASK_( new SendHeatChamControlCommandTask(SendHeatChamControlCommandTask::stop) ));
}


PTransfer CreateAdjustNullTask( unsigned nTest )
{
    Scenary scenary;
    PushBackReferences(scenary, BLOW_NULL_ADJUST_ID);
    using namespace DAK::Test::Col;
    using namespace DAK::Test::Row;
    PUSH_BACK_TASK_( new FixTestResultTask(nTest, adjust0, INIT_NUMS_(pgs)(conc) )) ));
    return PTransfer( new ChainedScenaryAction(scenary, AnsiString("Корректировка нуля") ));
}
//------------------------------------------------------------------------------
PTransfer CreateAdjustSensTask(unsigned nTest)
{
    Scenary scenary;
    PushBackReferences(scenary, BLOW_SENS_ADJUST_ID);
    using namespace DAK::Test::Col;
    using namespace DAK::Test::Row;
    PUSH_BACK_TASK_( new FixTestResultTask(nTest, adjustS, INIT_NUMS_(pgs)(conc) )) ));
    PushBackReferences(scenary, BLOW_NULL_AFTER_ADJUST_ID);
    return PTransfer( new ChainedScenaryAction(scenary, AnsiString("Корректировка чувствительности ") ));
}
//------------------------------------------------------------------------------
void CreateTermo20AdjustTask(Scenary& scenary){

    PUSH_BACK_TASK_( new SwitchOffPneumoTask ));
    PUSH_BACK_TASK_( new SendHeatChamSetpointTask(200) ));
    PUSH_BACK_TASK_( new CheckDeviceConnectTask(3*HOUR_MS) ));
    scenary.push_back( CreateAdjustNullTask(DAK::Test::main) );
    scenary.push_back( CreateAdjustSensTask(DAK::Test::main) );

    PUSH_BACK_TASK_( new BlowGas(0, 3*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new FixTemperatureValTask(0, 1 ) ));

    PUSH_BACK_TASK_( new BlowGas(1, 3*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new FixTemperatureValTask(1, 1 ) ));

    PUSH_BACK_TASK_( new BlowGas(2, 3*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new FixTemperatureValTask(2, 1 ) ));

    PUSH_BACK_TASK_( new BlowGas(0, 1*MINUTE_MS ) ));
    PUSH_BACK_TASK_( new SwitchOffPneumoTask ));
    PUSH_BACK_TASK_( new SendHeatChamControlCommandTask(SendHeatChamControlCommandTask::stop) ));
}


PTransfer CreateTermocompensation()
{
    Scenary scenary;
    CreateTermo20AdjustTask(scenary);
    TermocompensationPoint(-400, 0, scenary);
    TermocompensationPoint(600, 2, scenary);

    TermocompensationPoint(800, 4, scenary);

    TermocompensationPoint(200, 3, scenary);
    
    return PTransfer( new ChainedScenaryAction(scenary, "Техпрогон в диапазоне рабочих температур" ) );
}


//------------------------------------------------------------------------------
PTransfer CreateTest7PGSTask1(unsigned nTest, const AnsiString& blowTaskId, unsigned nPGS)
{
    Scenary scenary;
    using namespace DAK::Test::Row;
    PushBackReferences(scenary, blowTaskId+"_1");
    PUSH_BACK_TASK_( new FixTestResultTask(nTest, nPGS, INIT_NUMS_(pgs)(porog1)(porog2) )) ));
    PushBackReferences(scenary, blowTaskId+"_2");
    PUSH_BACK_TASK_( new FixTestResultTask(nTest, nPGS, INIT_NUMS_(pgs)(i)(conc_i)(conc) )) ));

    const unsigned gas[6] = { 0,1,2,1,0,2 };

    return PTransfer( new ChainedScenaryAction(scenary, MYSPRINTF_(
        "Проверка показаний, точка %d, ПГС%d", nPGS+1, gas[nPGS]+1) ));
}
//------------------------------------------------------------------------------
void CreateTest7PGSTask(Scenary &scenary, unsigned nTest)
{
    using namespace DAK::Test::Col;
    PUSH_BACK_TASK_( new SetPorogsTask(SetPorogsTask::k90percent) ));
    PUSH_BACK_TASK_( CreateTest7PGSTask1(nTest, BLOW_1_1_ID, pgs1_1 )));
    PUSH_BACK_TASK_( CreateTest7PGSTask1(nTest, BLOW_2_2_ID, pgs2_2 )));
    PUSH_BACK_TASK_( CreateTest7PGSTask1(nTest, BLOW_3_3_ID, pgs3_3 )));
    PUSH_BACK_TASK_( CreateTest7PGSTask1(nTest, BLOW_2_4_ID, pgs2_4 )));
    PUSH_BACK_TASK_( CreateTest7PGSTask1(nTest, BLOW_1_5_ID, pgs1_5 )));
    PUSH_BACK_TASK_( new SetPorogsTask(SetPorogsTask::k5percent) ));
    PUSH_BACK_TASK_( CreateTest7PGSTask1(nTest, BLOW_3_6_ID, pgs3_6 )));

    PushBackReferences(scenary, BLOW_1_7_ID);
    using namespace DAK::Test::Row;
    PUSH_BACK_TASK_( new FixTestResultTask(nTest, pgs1_7, INIT_NUMS_ (porog1)(porog2) )) ));

    PUSH_BACK_TASK_( new SetPorogsTask(SetPorogsTask::kNominal) ));
    PUSH_BACK_TASK_( new SwitchOffPneumoTask ));

}
//------------------------------------------------------------------------------
PTransfer CreateCheckChangePowerScenary()
{
    Scenary scenary;
    using namespace DAK::Test;
    PUSH_BACK_TASK_( new MessageBoxTask(" ", "Установите пониженное напряжение питания.", MB_ICONWARNING) ));

    scenary.push_back( CreateAdjustNullTask(power_11) );
    scenary.push_back( CreateAdjustSensTask(power_11) );
    CreateTest7PGSTask(scenary, power_11);

    PUSH_BACK_TASK_( new MessageBoxTask(" ", "Установите повышенное напряжение питания.", MB_ICONWARNING) ));
    CreateTest7PGSTask(scenary, power_30);
    return PTransfer( new ChainedScenaryAction(scenary, DAK_TEST_POWER ) );
}
//------------------------------------------------------------------------------
PTransfer CreateAdjustCurrentScenary()
{
    Scenary scenary;
    PushBackReferences(scenary, ADJUST_CURRENT_ID );
    return PTransfer( new ChainedScenaryAction(scenary, "Калибровка тока" ) );
}
//------------------------------------------------------------------------------
PTransfer CreateSetMainKefsScenary()
{
    Scenary scenary;
    PushBackReferences(scenary, INITIALIZE_DAK_ID );
    return PTransfer( new ChainedScenaryAction(scenary, "Установка коэффициентов" ) );
}
//------------------------------------------------------------------------------
PTransfer CreateCheckMainPogrScenary()
{
    Scenary scenary;
    using namespace DAK::Test;
    const DAK::Sets sets = DAK::Sets::Get();
    scenary.push_back( CreateAdjustNullTask(main) );
    scenary.push_back( CreateAdjustSensTask(main) );
    CreateTest7PGSTask( scenary, main );
    return PTransfer( new ChainedScenaryAction(scenary, DAK_MAIN_TEST ) );
}
//------------------------------------------------------------------------------
PTransfer CreateAutomationCheckScenary()
{
    Scenary scenary;
    using namespace DAK::Test;
    const DAK::Sets sets = DAK::Sets::Get();

    PUSH_BACK_TASK_( new MessageBoxTask(" ", "Установите номинальное напряжение питания.",
        MB_ICONWARNING) ));
    PUSH_BACK_TASK_( new DelayTask(10*MINUTE_MS) ));

    PUSH_BACK_TASK_( CreateSetMainKefsScenary() ));

    PUSH_BACK_TASK_( CreateAdjustCurrentScenary() ));
    scenary.push_back( CreateAdjustNullTask(main) );
    scenary.push_back( CreateAdjustSensTask(main) );
    PUSH_BACK_TASK_( CreateCheckMainPogrScenary() ));

    PushBackReferences(scenary, INIT_STRS_
        (TEST_CHANGE_POWER_ID)
        (CHECK_STAB_TEST_ID)
        (TEH20_ID)
        (TEH78_ID) ));

    return PTransfer( new ChainedScenaryAction(scenary,
        "Автоматическая проверка" ) );
}
//------------------------------------------------------------------------------





PTransfer Create_TestHart()
{

    Scenary scenary;
    PUSH_BACK_TASK_( new TestHartTask() ));

    return PTransfer( new ChainedScenaryAction(scenary, "Проверка HART проткола" ) );
    //return PTransfer(new TestHartTask());

}
