//---------------------------------------------------------------------------
#ifndef guiconH
#define guiconH
//---------------------------------------------------------------------------
#include <windows.h>

#include <system.hpp>

#include <utility.h>

#include <boost/noncopyable.hpp>

//------------------------------------------------------------------------------
#define MYWCOUT_ MyWCout( WideString().sprintf
#define MYCOUT_ MyCout( AnsiString().sprintf

//------------------------------------------------------------------------------
typedef std::pair<WORD,WORD> IncExcAttr;

void MyWCout(const WideString& str );
void MyWCout(const WideString& str,  IncExcAttr inc_exc  );

void MyCout(const AnsiString& str );
void MyCout(const AnsiString& str, IncExcAttr inc_exc );
void MyCoutTime(const AnsiString& str  );

void MyCoutTime(const AnsiString& str, IncExcAttr inc_exc = IncExcAttr() );
void CreateGUIConsole();
void SaveGUIConsoleToFile(const AnsiString& fn);
void BackupGUIConsole();
//------------------------------------------------------------------------------
const IncExcAttr
	MY_SET_CONSOLE_VELVET_TEXT( FOREGROUND_BLUE  | FOREGROUND_RED | FOREGROUND_INTENSITY, FOREGROUND_GREEN),
	MY_SET_CONSOLE_AQUA_TEXT( FOREGROUND_BLUE  | FOREGROUND_GREEN | FOREGROUND_INTENSITY, FOREGROUND_RED),
	MY_SET_CONSOLE_BLUE_TEXT( FOREGROUND_BLUE | FOREGROUND_INTENSITY, FOREGROUND_RED | FOREGROUND_GREEN ),
    MY_SET_CONSOLE_BLUE_TEXT1( FOREGROUND_BLUE, FOREGROUND_RED | FOREGROUND_GREEN ),
	MY_SET_CONSOLE_RED_TEXT( FOREGROUND_RED  | FOREGROUND_INTENSITY, FOREGROUND_BLUE | FOREGROUND_GREEN),
    MY_SET_CONSOLE_RED_TEXT1( FOREGROUND_RED , FOREGROUND_BLUE | FOREGROUND_GREEN),
	MY_SET_CONSOLE_YELLOW_TEXT( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, FOREGROUND_BLUE ),
    MY_SET_CONSOLE_GREEN_TEXT( FOREGROUND_GREEN | FOREGROUND_INTENSITY, FOREGROUND_BLUE | FOREGROUND_RED ),
    MY_SET_CONSOLE_GREEN_TEXT1( FOREGROUND_GREEN, FOREGROUND_BLUE | FOREGROUND_RED );
//------------------------------------------------------------------------------
class ConsoleScreenAtrributesHolder : public boost::noncopyable
{
public:
	explicit ConsoleScreenAtrributesHolder(IncExcAttr inc_exc);
	~ConsoleScreenAtrributesHolder();
private:
	WORD attrs_;
};
//------------------------------------------------------------------------------
class DelayCounter : public boost::noncopyable
{
public:
    explicit DelayCounter();
    AnsiString ToStr() const;
    void Reset();
    unsigned DelayCounter::Count() const;
private:
    unsigned startMoment_;
};
//------------------------------------------------------------------------------
bool IsConsoleVisible();
WINDOWPLACEMENT MyShowConsole(unsigned showCmd);
//------------------------------------------------------------------------------
class GUIConsoleInputHelper : public boost::noncopyable
{
public:
	GUIConsoleInputHelper();
	~GUIConsoleInputHelper();
private:
	void *hMainForm_, *hCon_, *hConPar_;
    WINDOWPLACEMENT conPlOrig_;
};
//------------------------------------------------------------------------------
class ConsoleMultioutputHelper : public boost::noncopyable
{
public:
    explicit ConsoleMultioutputHelper();
    void Set(const AnsiString& s);
private:
    HANDLE hCon_;
    COORD pos0_;
    unsigned curWriteConsoleCount_;
};


#endif
//---------------------------------------------------------------------------




