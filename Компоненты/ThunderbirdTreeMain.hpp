// Borland C++ Builder
// Copyright (c) 1995, 2002 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'ThunderbirdTreeMain.pas' rev: 6.00

#ifndef ThunderbirdTreeMainHPP
#define ThunderbirdTreeMainHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <Dialogs.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <ExtCtrls.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <ComCtrls.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Thunderbirdtreemain
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TThunderbirdSection;
class DELPHICLASS TThunderbirdTree;
class PASCALIMPLEMENTATION TThunderbirdTree : public Extctrls::TPanel 
{
	typedef Extctrls::TPanel inherited;
	
private:
	TThunderbirdSection* fRoot;
	Forms::TFormBorderStyle FBorderStyle;
	Controls::TImageList* fImageList;
	bool fShowHeaders;
	HIDESBASE void __fastcall SetBorderStyle(const Forms::TBorderStyle Value);
	void __fastcall setHeadersVisible(const bool Value);
	
protected:
	virtual void __fastcall CreateParams(Controls::TCreateParams &Params);
	HIDESBASE void __fastcall Autosize(void);
	
public:
	HIDESBASE MESSAGE void __fastcall WMPaint(Messages::TWMPaint &Message);
	__fastcall virtual TThunderbirdTree(Classes::TComponent* Aowner);
	__fastcall virtual ~TThunderbirdTree(void);
	TThunderbirdSection* __fastcall AddSection(AnsiString Caption);
	void __fastcall AttachControl(TThunderbirdSection* Section, Controls::TControl* Control, bool Expand = false);
	
__published:
	__property Forms::TBorderStyle BorderStyle = {read=FBorderStyle, write=SetBorderStyle, default=1};
	__property Font ;
	__property bool ShowHeaders = {read=fShowHeaders, write=setHeadersVisible, nodefault};
public:
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TThunderbirdTree(HWND ParentWindow) : Extctrls::TPanel(ParentWindow) { }
	#pragma option pop
	
};


class PASCALIMPLEMENTATION TThunderbirdSection : public System::TObject 
{
	typedef System::TObject inherited;
	
public:
	TThunderbirdSection* operator[](int index) { return Item[index]; }
	
private:
	Classes::TList* fList;
	TThunderbirdTree* fOwner;
	TThunderbirdSection* fParent;
	AnsiString fCaption;
	Extctrls::TPanel* fPanel;
	Extctrls::TImage* fImage;
	bool fExpanded;
	Extctrls::TPanel* fHeaderPanel;
	Extctrls::TPanel* fContentPanel;
	Extctrls::TPanel* fLeftPanel;
	Extctrls::TPanel* fRightPanel;
	void __fastcall CreatePanel(void);
	void __fastcall CreateLabel(void);
	int __fastcall getCount(void);
	TThunderbirdSection* __fastcall getItem(int index);
	void __fastcall ExpandClick(System::TObject* Sender);
	int __fastcall getPreferedHeight(void);
	void __fastcall RealignAll(void);
	
public:
	void __fastcall Contract(void);
	void __fastcall Expand(void);
	__property Extctrls::TPanel* Content = {read=fRightPanel};
	void __fastcall HeadersVisible(bool yesNo);
	void __fastcall Clear(void);
	void __fastcall Delete(int index);
	__property int Count = {read=getCount, nodefault};
	__property TThunderbirdSection* Item[int index] = {read=getItem/*, default*/};
	__property AnsiString Caption = {read=fCaption};
	TThunderbirdSection* __fastcall Add(AnsiString Caption);
	__fastcall virtual TThunderbirdSection(TThunderbirdTree* Owner, TThunderbirdSection* Parent);
	__fastcall virtual ~TThunderbirdSection(void);
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Thunderbirdtreemain */
using namespace Thunderbirdtreemain;
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// ThunderbirdTreeMain
