//------------------------------------------------------------------------------
#ifndef DEVICE_CALCULATION_VARIABLE_DEFINITION_HEADER_INCLUDED
#define DEVICE_CALCULATION_VARIABLE_DEFINITION_HEADER_INCLUDED
//------------------------------------------------------------------------------
//std
#include <vector>
#include <utility>
#include <string>
#include <map>
//------------------------------------------------------------------------------
//vcl
#include <system.hpp>
#include <Classes.hpp>
//------------------------------------------------------------------------------
#define DAK_MAIN_TEST           "Проверка диапазона измерений"
#define DAK_TEST_POWER          "Проверка устойчивости при изменении напряжения питания"
#define DAK_TEST_TECHPROGON_20  "Техпрогон НКУ"
#define DAK_TEST_TECHPROGON_78  "Техпрогон при повышенной температуре"
//------------------------------------------------------------------------------
namespace Scn{ enum {
    NULL_GAS_IDX = 0,

    MIDL_GAS_IDX = 1,

    SENS_GAS_IDX = 2
};};
//------------------------------------------------------------------------------
namespace DAKGas
{
    enum { CO2 = false, CH = true, count};
    const AnsiString cpt[count] ={ "СО2", "углеводороды" };
    double MaxPogr(bool dakGas, unsigned scale, double c);
    double Porog1(bool dakGas, unsigned scale );
    double Porog2(bool dakGas, unsigned scale );

};
//------------------------------------------------------------------------------
namespace DAK
{
//------------------------------------------------------------------------------
// максимально допустимая погрешность концентрации
double MaxPogrOfConc(double c);
// пересчитать ток выхода в концентрацию
double I2Conc(double i);
namespace Format
{
    AnsiString Porog1(bool state, unsigned gasIdx);
    AnsiString Porog2(bool state, unsigned gasIdx);
    AnsiString Porog(bool porog1, bool porog2, unsigned gasIdx);

    AnsiString Porog1(bool state);
    AnsiString Porog2(bool state);
    AnsiString Porog(bool porog1, bool porog2);

    AnsiString Conc(double conc);
    AnsiString Conc(double conc, unsigned gasIdx);
    AnsiString Iout(double conc);
    AnsiString Iout(double conc, unsigned gasIdx);
    bool TryGetValue1(const AnsiString &srcS, double &val);
    bool TryGetDelta(const AnsiString &srcS, double &val);

    AnsiString Variac1(double conc1, double conc2);
    AnsiString Variac2(const AnsiString &conc1, const AnsiString &conc2);

    AnsiString RelPogr(double);
    AnsiString DeltaConc(double, unsigned nPGS);
    AnsiString DeltaStab(double);

    AnsiString FFloat(double, unsigned);
    AnsiString Res(bool res);
    AnsiString Res1(bool res);

    AnsiString ConvertLongConc2Short(const AnsiString& s);
};
//------------------------------------------------------------------------------
typedef std::pair<bool, bool> Porog1Porog2Par;
//------------------------------------------------------------------------------
const Porog1Porog2Par testPorog[3] =
{
    Porog1Porog2Par(false, false),
    Porog1Porog2Par(true, false),
    Porog1Porog2Par(true, true)
};
//------------------------------------------------------------------------------
struct Sets
{
    unsigned    endScale;
    bool        isCH;
    double      porog1, porog2;
    AnsiString  gas, softVer, ispolnenie, scale, units;
    std::vector<double>      pgs, maxPogr;
    double      UpowerNomilal;

    static Sets Get();
};
//------------------------------------------------------------------------------
namespace RelePin
{
    enum { rele_rn, rele1, rele2, count };
    const char * const cpt[count] ={ "Реле РН", "П1", "П2",  };
};
//------------------------------------------------------------------------------
namespace Vars
{
    enum{ C, C_Iout, Iout, rele1, rele2, var1, Tpp, count };
    const char * const cpt[count] =
    {
        "Конц.",
        "Конц.(Iвых)",
        "Iвых, мА",
        "П1",
        "П2",
        "Var1",
        "Tpp"
    };
    void Set(unsigned addy, unsigned nVar, const AnsiString& s);
    AnsiString Get(unsigned addy, unsigned nVar );
};
//------------------------------------------------------------------------------
namespace Cmd {
//------------------------------------------------------------------------------
    namespace Idx
    {
        enum
        {
            adj_0,
            adj_s,
            por_1,
            por_2,
            select_gas,
            test_watch_dog,
            set_tok,
            reset,
            test_watch_dog1,
            addy,
            count
        };
    };
    namespace Code
    {
        enum
        {
            adj_0 = 1,
            adj_s = 2,
            por_1 = 3,
            por_2 = 4,
            select_gas = 0x10,
            test_watch_dog = 0x51,
            set_tok = 0x53,
            reset = 0x20,
            test_watch_dog1  = 96,
            addy = 7
        };

        const unsigned all[Idx::count] =
        {
            adj_0,
            adj_s,
            por_1,
            por_2,
            select_gas,
            test_watch_dog,
            set_tok,
            reset,
            test_watch_dog1,
            addy
        };

    };
    const char * const name[Idx::count] =
    {
        "Корректировка нулевых показаний",
        "Корректировка чувствительности",
        "Установка значения ПОРОГ1",
        "Установка значения ПОРОГ2",
        "Выбор определяемого компонента",
        "проверка сторожевого таймера",
        "калибровка тока",
        "сброс сигнализации",
        "проверка ватчдога",
        "Установка адреса сигнализатора"
    };
    AnsiString Code2Str(unsigned code);
//------------------------------------------------------------------------------
};  // namespace Cmd
//------------------------------------------------------------------------------
struct Kef
{
    struct Prop { AnsiString key, val;    };
    typedef std::vector<Prop> Props;
	AnsiString caption, def;
    unsigned n;
    Props props;
    void SetStringsKeys( TStrings* strs ) const;
    AnsiString Val2Key(const AnsiString &val) const;
    AnsiString Key2Val(const AnsiString &key) const;

    static unsigned GetNumberOfKNum(unsigned);
};
//------------------------------------------------------------------------------
typedef std::vector<Kef> Kefs;
const Kefs& GetKefs();
void SaveKefsList();
//------------------------------------------------------------------------------
namespace MainKef
{
    enum
    {
        SoftVer, Year, Serial, Gas, Units, Scale, Scale0, ScaleK,
        Isp, DevType, PGS1, PGS3, Diap, Porog1, Porog2, count
    };
    const unsigned* Num();
    typedef std::pair<AnsiString,AnsiString> Scale0K;
    Scale0K ExtractScaleOfCode(const AnsiString &codeScale);
    Scale0K ExtractScaleOfValue(const AnsiString &valueScale);

    const DAK::Kef& GetKef(unsigned n);
    unsigned GetKefNum(unsigned n);
};
//------------------------------------------------------------------------------
namespace Test
{
//------------------------------------------------------------------------------
namespace Row {
    enum { pgs, conc, conc_i, i, variac, porog1, porog2, difference, count };
    const char * const name[count] = {
    "Конц.ПГС", "Конц.", "Конц.ток" , "Ток,мА",  "Вариация", "ПОРОГ 1", "ПОРОГ 2", "Разность" };
    AnsiString GetVar(unsigned addy, unsigned nRow);
};
//------------------------------------------------------------------------------
namespace Col {
    enum { pgs1_1, pgs2_2, pgs3_3, pgs2_4, pgs1_5, pgs3_6, pgs1_7, maxPogr, adjust0, adjustS };
    enum { variac1 = pgs2_2, variac2=DAK::Test::Col::pgs2_4 };
    enum { concMin, concMax, concDelta };
}; // namespace Col {
//------------------------------------------------------------------------------
AnsiString FormatTestParam(unsigned nTest, unsigned nPGS, const std::vector<unsigned> &params);
AnsiString FormatTestParam(unsigned nTest, unsigned nPGS, unsigned param);

    enum{
        main,
        stability,
        power_11,
        power_30,
        tech_20_3,
        tech_20_16,
        tech_78_3,
        tech_78_19,
        temperature,
        count };

    const char * const name[count] =
    {
        DAK_MAIN_TEST,
        "Проверка нестабильности показаний",
        DAK_TEST_POWER" 11 В",
        DAK_TEST_POWER" 30 В",
        DAK_TEST_TECHPROGON_20" 3 часа",
        DAK_TEST_TECHPROGON_20" 16 часов",
        DAK_TEST_TECHPROGON_78" 3 часа",
        DAK_TEST_TECHPROGON_78" 19 часов",
        "Компенсация влияния температуры"

    };
    const char * const id[count] =
    {
        "main",
        "нестабильность",
        "test11V",
        "test30V",
        "техпрогон20постановка",
        "техпрогон20снятие",
        "техпрогон78постановка",
        "техпрогон78снятие",
        "temperature"

    };
    const char NumberVar[] = "номер_проверки";
    const char ColumnNumberVar[] = "номер_столбца_проверки";
//------------------------------------------------------------------------------
}; // namespace Test
//------------------------------------------------------------------------------
}; // namespace DAK
//------------------------------------------------------------------------------
#define MAIN_KEF_N_(nn) DAK::MainKef::GetKefNum(DAK::MainKef::##nn##)
#define MAIN_KEF_(nn) DAK::MainKef::GetKef(DAK::MainKef::##nn##)

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
