//---------------------------------------------------------------------------
#pragma hdrstop

//------------------------------------------------------------------------------
#include "boost/regex.hpp"
#include <math>
//------------------------------------------------------------------------------
#include "DAKUtils.h"
#include "ctrlsys.h"
#include "vardef.hpp"
#include "pneumo.hpp"
#include "Unit1.h"
#include "AnsiStringUtils_.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
namespace DAKGas
{
    double MaxPogr(bool dakGas, unsigned scale, double c)
    {
        if( dakGas==CH )            return 2.5+0.05*c;
        else if( dakGas==CO2 ) {
            if(scale==4)            return 0.2+0.05*c;
            else if(scale==10)      return 0.5;
            else if(scale==20)      return 1.0;
        }
        return 0;
    }
    double Porog1(bool dakGas, unsigned scale )
    {
        if( dakGas==CH )
            return 7;
        else if( dakGas==CO2 )
        {
            if(scale==4)
                return 0.5;
            else if(scale==10)
                return 1.25;
            else if(scale==20)
                return 2.5;
        }
        return 0;
    }
    double Porog2(bool dakGas, unsigned scale )
    {
        if( dakGas==CH )
            return 12;
        else if( dakGas==CO2 )
        {
            if(scale==4)
                return 1;
            else if(scale==10)
                return 2.5;
            else if(scale==20)
                return 5;
        }
        return 0;
    }
};
//------------------------------------------------------------------------------
namespace DAK{
//------------------------------------------------------------------------------
namespace Vars {
void Set(unsigned addy, unsigned nVar, const AnsiString& s)
{
    Form1->SetMeasured(addy, nVar, s );
}
AnsiString Get(unsigned addy, unsigned nVar )
{
    return Form1->GetMeasured(addy, nVar );
}
//------------------------------------------------------------------------------
}; //namespace Var {
//------------------------------------------------------------------------------
double I2Conc(double i)
{
    const Sets sets = Sets::Get();
    return (i-4.0)/( 16.0/sets.endScale );
}
double Conc2I(double conc)
{
    const Sets sets = Sets::Get();
    const double k = sets.isCH ? 100.0 : 1.0;
    return (16.0*conc/k) + 4.0;
}
//------------------------------------------------------------------------------
double MaxPogrOfConc(double c)
{
    const Sets sets = Sets::Get();
    return DAKGas::MaxPogr( sets.isCH, sets.endScale, c);
}
//------------------------------------------------------------------------------
namespace Format
{
#define REGEX_EXTRACT_FIRST_FLOAT boost::regex("^([+\\-]?[0-9]+(?:[,\\.][0-9]*)?)")
#define REGEX_EXTRACT_FIRST_WORD boost::regex("^\\s*([^\\s]*)\\s*")
//------------------------------------------------------------------------------
AnsiString ConvertLongConc2Short(const AnsiString& s)
{
    const std::string src = s.c_str();
    std::string::const_iterator xItStart = src.begin(), xItEnd = src.end(), xIt = xItStart;
    boost::cmatch xRes;
    if( !boost::regex_search(xIt, xItEnd, xRes, REGEX_EXTRACT_FIRST_FLOAT ) )
        return s;
    return std::string( xRes[1].first, xRes[1].second ).c_str();
}
//------------------------------------------------------------------------------
bool TryGetValue1(const AnsiString &srcS, double &val)
{
    const std::string src = srcS.c_str();
    std::string::const_iterator xItStart = src.begin(), xItEnd = src.end(), xIt = xItStart;
    boost::cmatch xRes;
    if( !boost::regex_search(xIt, xItEnd, xRes, REGEX_EXTRACT_FIRST_FLOAT ) )
        return false;
    AnsiString s = std::string( xRes[1].first, xRes[1].second ).c_str();
    return MyTryStrToFloat( s, &val);
}
//------------------------------------------------------------------------------
bool TryGetDelta(const AnsiString &srcS, double &val)
{
    const std::string src = srcS.c_str();
    const char sRegEx[] = "([0-9]+(?:[,\\.][0-9]*)?)d";
    std::string::const_iterator xItStart = src.begin(), xItEnd = src.end(), xIt = xItStart;
    boost::cmatch xRes;
    if( !boost::regex_search(xIt, xItEnd, xRes,
        boost::regex(sRegEx) ) ) return false;
    AnsiString s = std::string( xRes[1].first, xRes[1].second ).c_str();
    return MyTryStrToFloat( s, &val);
}

//------------------------------------------------------------------------------
AnsiString Res(bool res)                {    return res ? "+" : "-"; }
AnsiString Res1(bool res)               {    return "["+Res(res)+"]"; }
AnsiString FFloat( double v, int n )    {    return FormatFloat( "#0." + AnsiString::StringOfChar('#', n), v); }
//------------------------------------------------------------------------------
AnsiString P1(bool state)               {    return "Ï1"+Res(state); }
AnsiString P2(bool state)               {    return "Ï2"+Res(state); }
AnsiString P(bool porog1, bool porog2)  {    return P1(porog1)+" "+P2(porog2); }
AnsiString Porog1(bool state, unsigned gasIdx){
    assert(gasIdx<Pneumo::Count());
    return P1(state)+" "+Res1(state==testPorog[gasIdx].first);
}
AnsiString Porog2(bool state, unsigned gasIdx){
    assert(gasIdx<Pneumo::Count());
    return P2(state)+" "+Res1(state==testPorog[gasIdx].second);
}
AnsiString Porog(bool porog1, bool porog2, unsigned gasIdx){
    return Porog1(porog1, gasIdx)+" "+Porog2(porog2, gasIdx);
}
//------------------------------------------------------------------------------
AnsiString Porog1(bool state)
{
    const unsigned gasIdx = Pneumo::Current(), count = Pneumo::Count();
    return gasIdx<count ? Porog1(state, gasIdx ) : P1(state);
}
AnsiString Porog2(bool state)
{
    const unsigned gasIdx = Pneumo::Current(), count = Pneumo::Count();
    return gasIdx<count ? Porog2(state, gasIdx ) : P2(state);
}
AnsiString Porog(bool porog1, bool porog2)
{
    const unsigned gasIdx = Pneumo::Current(), count = Pneumo::Count();
    return gasIdx<count ? Porog(porog1, porog2, gasIdx ) : P(porog1, porog2);
}
//------------------------------------------------------------------------------
AnsiString Conc(double conc, unsigned gasIdx)
{
    assert(gasIdx<Pneumo::Count());
    const Sets sets = Sets::Get();
    const double
        pgs = sets.pgs[gasIdx],
        maxD = sets.maxPogr[gasIdx],
        d = conc-pgs, d_abs = std::fabs(d),
        delta = maxD==0 ? 1 : d_abs/maxD;
    const bool good = std::fabs(d)<maxD;
    return
        FFloat( conc, 3)+" "+
        Res(d>=0)+FFloat(d_abs,3)+(good ? "<" : ">" )+FFloat(maxD,3)+" "+
        FFloat(delta,1)+"d "+Res1(good);
}
//------------------------------------------------------------------------------
AnsiString Iout(double i, unsigned gasIdx)
{
    //assert(gasIdx<Pneumo::Count());
    /*
    const Sets sets = Sets::Get();
    const double
        i_pgs   = Conc2I( sets.pgs[gasIdx] ),
        maxD    = std::fabs( Conc2I( sets.maxPogr[gasIdx] )-Conc2I(0) ),
        d       = i-i_pgs, d_abs = std::fabs(d),
        delta   = i_pgs==0 ? 0 : d_abs/i_pgs;
    const bool good = std::fabs(d)<maxD;
    return
        FFloat( i, 1)+"ìÀ "+
        Res(d>=0)+FFloat(d_abs,1)+(good ? "<" : ">" )+FFloat(maxD,1)+" "+
        FFloat(delta,1)+"d "+Res1(good);
    */
    return FFloat( i, 3);
}
AnsiString Iout(double i)
{
    //const unsigned gasIdx = Pneumo::Current(), count = Pneumo::Count();
    //return gasIdx<count ? Iout(i, gasIdx) : FFloat( i, 3);
    return FFloat( i, 3);
}

//------------------------------------------------------------------------------
AnsiString Conc(double conc)
{
    const unsigned gasIdx = Pneumo::Current(), count = Pneumo::Count();
    return gasIdx<count ? Conc(conc, gasIdx) : FFloat( conc, 3);
}
//------------------------------------------------------------------------------
AnsiString Variac1(double conc1, double conc2)
{
    const Sets sets = Sets::Get();
    const double v = std::fabs(conc1-conc2), maxD = sets.maxPogr[1]*0.5;
    const bool good = v<maxD;
    return FFloat( v, 3)+" "+(good ? "<" : ">" )+FFloat(maxD,3)+" "+Res1(good);
}
//------------------------------------------------------------------------------
AnsiString Variac2(const AnsiString &sConc1, const AnsiString &sConc2)
{
    double conc1, conc2;
    if( TryGetValue1(sConc1, conc1) && TryGetValue1(sConc2, conc2) )
        return Variac1(conc1, conc2);
    else
        return "";
}
AnsiString RelPogr(double d)
{
    return FFloat( d, 2) +"d " + Res1(std::fabs(d)<0.9);
}
AnsiString DeltaConc(double v, unsigned nPGS)
{
    if(nPGS>=DAK::Test::Col::maxPogr)
        return "";
    const Sets sets = Sets::Get();
    const unsigned
        gasIdxs[DAK::Test::Col::maxPogr] = { 0,1,2,1,0,2 },
        gasIdx = gasIdxs[nPGS];
    const double maxD = sets.maxPogr[gasIdx], d = maxD==0 ? 1 : v/maxD;
    return FFloat( v, 2) + " " +FFloat( d, 2) +"d " + Res1(std::fabs(d)<0.8);
}
AnsiString DeltaStab(double v)
{
    const Sets sets = Sets::Get();
    const double maxD = sets.maxPogr[2], d = maxD==0 ? 1 : v/maxD;
    return FFloat( v, 2) + " " +FFloat( d, 2) +"d " + Res1(std::fabs(d)<0.2);
}
//------------------------------------------------------------------------------
}; // namespace Format
//------------------------------------------------------------------------------
Sets Sets::Get()
{
    const CtrlSysImpl::DAKSets instSets = CtrlSys::Instance().GetDAKSets();
    Sets ret;

    ret.endScale    = instSets.endScale;
    ret.isCH        = instSets.isCH;
    ret.porog1      = DAKGas::Porog1(ret.isCH, ret.endScale);
    ret.porog2      = DAKGas::Porog2(ret.isCH, ret.endScale);
    ret.gas         = instSets.gas;
    ret.softVer     = instSets.softVer;
    ret.ispolnenie  = instSets.ispolnenie;
    ret.scale       = instSets.scale;
    ret.units       = instSets.units;
    const unsigned gasCount = Pneumo::GetGases().size();
    ret.pgs.resize      (gasCount );
    ret.maxPogr.resize  (gasCount );
    for( unsigned i=0; i<gasCount; ++i )
    {
        ret.pgs[i]          = Pneumo::Conc(i);
        ret.maxPogr[i]      = DAKGas::MaxPogr( ret.isCH, ret.endScale, ret.pgs[i] );
    }

    const unsigned isp = StrToIntDef(ret.ispolnenie,0);
    if(isp>=23 && isp<=25 || isp>=31 && isp<=33)
        ret.UpowerNomilal = 13;
    else if(isp>=26 && isp<=30)
        ret.UpowerNomilal = 27;
    else
        ret.UpowerNomilal = 13;
    return ret;
}
namespace Test
{
    AnsiString FormatTestParam(unsigned nTest, unsigned nPGS, const std::vector<unsigned> &params)
    {
        return CtrlSys::Instance().FormatTestParam(nTest, nPGS, params);
    }

    AnsiString FormatTestParam(unsigned nTest, unsigned nPGS, unsigned param)
    {
        return CtrlSys::Instance().FormatTestParam(nTest, nPGS, param);
    }

//------------------------------------------------------------------------------
namespace Row{

AnsiString GetVar(unsigned addy, unsigned nRow)
{
    //pgs, conc, conc_i, i, variac, porog1, porog2, difference
    //enum{ C, C_Iout, Iout, rele1, rele2, count };
    if(nRow==pgs) {
        const Sets sets = Sets::Get();
        const int gasIdx = Pneumo::Current();
        return sets.pgs[gasIdx];
    } else if(nRow<variac) {
        return Vars::Get(addy, nRow-1);
    } else if(nRow==porog1) {
        return Vars::Get(addy, Vars::rele1);
    } else if(nRow==porog2) {
        return Vars::Get(addy, Vars::rele2);
    }
    return "";

}

}; //namespace Row{
//------------------------------------------------------------------------------
}; // namespace Test
//------------------------------------------------------------------------------
}; //namespace DAK
//------------------------------------------------------------------------------
