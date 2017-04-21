//------------------------------------------------------------------------------
#ifndef DevicesH
#define DevicesH
//------------------------------------------------------------------------------
#include <grids.hpp>
#include <ComCtrls.hpp>
#include <vector>
//------------------------------------------------------------------------------
#include "vardef.hpp"
#include "uFrameDevicePasport.h"
//------------------------------------------------------------------------------
namespace Devs
{
    typedef std::vector<unsigned> Nums;

    typedef std::pair< unsigned, unsigned > Item;
    typedef std::vector<Item> Items;

    struct InitT
    {
        TStringGrid *grdVars, *grdScnList, *grdKfs;
        TListView *lvDevs;
        TPageControl *pgCtRep, *pgPasp, *pgChart;
    };
    void Initialize(InitT);
    void Finalize();

    TRichEdit *GetLogOfAddy(unsigned addy);
    TRichEdit *GetLog(unsigned nDev);

    TFrameDevicePasport* GetFramePasp(unsigned nDev);
    TFrameDevicePasport* GetFramePaspOfAddy(unsigned addy);


    void AddDevice(unsigned serial, unsigned addy);
    void DelDevice(unsigned n);
    void ClearDevices(unsigned n);
    void ChangeDeviceAddy(unsigned n, unsigned addy);
    unsigned Count();
    AnsiString Caption(unsigned n);


    void SaveFile(const AnsiString& fn);
    void LoadFile(const AnsiString& fn);
    void ClearDevicesData();

    void SelectAddy(unsigned addy, bool setSelected);
    void SelectDevice(unsigned n, bool setSelected);
    
    bool IsSelectedDevice(unsigned n);

    Nums GetSelectedAddys();
    Nums GetAddys();
    Nums GetSerials();   
    Items GetItems();

    unsigned Addy(unsigned nDev);
    unsigned Serial(unsigned nDev);

    int IndexOfAddy(unsigned addy);
    bool IsSelectedAddy(unsigned addy);
    bool HasOneSelectedAddy();
    bool HasOneGoodSelectedAddy();

    void SetNotAnswered(unsigned slave);
    void SetAnswered(unsigned slave);

    AnsiString GetKefOfAddy( unsigned slave, unsigned kefNum);
    void SetKefOfAddy( unsigned slave, unsigned koefNum, const AnsiString&);
    void SetKefDefault( unsigned nDevice, unsigned koefNum );
    void SetKefOfAddyDef( unsigned slave, unsigned koefNum );
    void SetKef(unsigned nDevice, unsigned kefNum, const AnsiString& s);
    AnsiString GetKef(unsigned nDevice, unsigned kefNum);
    AnsiString GetSensorSlave(unsigned nDevice);

    Nums GetSelectedKefsNums();
    bool IsKefSelected(unsigned n);

    void SaveDefaultKefsVals();
    void SetDefaultSelectedKefsItemsVals();

    void SaveForTermoFile(const AnsiString& fn);

    void PromptUserInputDevice();
    void PromptUserChangeDevice(unsigned nDev);
};
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
