//---------------------------------------------------------------------------

#include <windows.h>
#include <winspool.h>
//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
// my
#include "CommSetsIOHelper.h"
#include "MyDCBHelper.h"
#include "xmlUtils_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
namespace my
{
namespace RS232
{
    void SaveSetingsXML( TiXmlElement *xml, const Setings& srcSets)
    {

#define WRITE_INI(id,val) TiXMLAtr::Set(xml, id, AnsiString().sprintf("%u",val) );
        for( unsigned idx=0; idx<DCB_FIELDS_COUNT; ++idx )
        {
        	const DWORD val = GetDCBField(srcSets.dcb_, idx);
            WRITE_INI(GetDCBFieldName(idx), val);
        }
#define SAVE_TIME_OUT_FIELD(name) WRITE_INI( ""#name, srcSets.timeOuts_.##name )
        SAVE_TIME_OUT_FIELD(ReadIntervalTimeout);
        SAVE_TIME_OUT_FIELD(ReadTotalTimeoutMultiplier);
        SAVE_TIME_OUT_FIELD(ReadTotalTimeoutConstant);
        SAVE_TIME_OUT_FIELD(WriteTotalTimeoutMultiplier);
        SAVE_TIME_OUT_FIELD(WriteTotalTimeoutConstant);
#undef SAVE_TIME_OUT_FIELD(name)
		WRITE_INI("txdBuffSize", srcSets.txdBuffSize_ );
        WRITE_INI("rxdBuffSize", srcSets.rxdBuffSize_ );
#undef WRITE_INI(id,val)
    }

    void LoadSetingsXML( TiXmlElement *xml, Setings& dstSets)
    {
        const DCB defDcb = GetDefaultDCB();
        const COMMTIMEOUTS defTimeOuts = GetDefaultCommTimeOuts();

        for( unsigned idx=0; idx<DCB_FIELDS_COUNT; ++idx )
        SetDCBField
        (
        	TiXMLAtr::Get( xml, GetDCBFieldName(idx), GetDCBField(defDcb, idx) ),
            dstSets.dcb_,idx
        );

#define SET_TIME_OUT_FIELD(name) dstSets.timeOuts_.##name= TiXMLAtr::Get( xml, ""#name, defTimeOuts.##name )
		SET_TIME_OUT_FIELD(ReadIntervalTimeout);
        SET_TIME_OUT_FIELD(ReadTotalTimeoutMultiplier);
        SET_TIME_OUT_FIELD(ReadTotalTimeoutConstant);
        SET_TIME_OUT_FIELD(WriteTotalTimeoutMultiplier);
        SET_TIME_OUT_FIELD(WriteTotalTimeoutConstant);
#undef SET_TIME_OUT_FIELD(name)
        
		dstSets.txdBuffSize_ = TiXMLAtr::Get( xml, "txdBuffSize", 1024 );
        dstSets.rxdBuffSize_ = TiXMLAtr::Get( xml, "rxdBuffSize", 1024 );
        
    }

};
};
