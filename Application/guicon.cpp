//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "guicon.h"
#include "MyFileCtrl.h"
#include "AnsiStringUtils_.h"
#include "winerr.hpp"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
#include <sysutils.hpp>
#include <dateutils.hpp>
#include <Forms.hpp>

#include <vector>
#include <fstream>

#include "MyModFN.hpp"




	std::pair<unsigned,unsigned> GetOrders()
    {
   		CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;
   		GetConsoleScreenBufferInfo( GetStdHandle(STD_OUTPUT_HANDLE), &lConsoleScreenBufferInfo );
		return std::make_pair( lConsoleScreenBufferInfo.dwSize.X,
			lConsoleScreenBufferInfo.dwSize.Y);
    }

    // control signal type
    BOOL WINAPI HandlerRoutine( const DWORD dwCtrlType)
    {
        const bool isCtrlBreakEvent =
            (dwCtrlType==CTRL_BREAK_EVENT	|| dwCtrlType==CTRL_C_EVENT);
        if( !isCtrlBreakEvent )
            return FALSE;
        printf( "\n\t\t---Ctrl+Break pressed!---\n" );
        return TRUE;
    }
	//--------------------------------------------------------------------------
    
	void CreateGUIConsole()
	{
        ::AllocConsole();
		HWND hWnd = ::GetConsoleWindow(), hMenu = ::GetSystemMenu(hWnd, FALSE);
		::ShowWindow(hWnd, SW_SHOW);
        ::DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		std::freopen("CONIN$", "r", stdin);
		std::freopen("CONOUT$", "w", stderr);
		std::freopen("CONOUT$", "w", stdout);
		std::system("MODE CON COLS=100");
		std::system("MODE CON LINES=9999");
		std::system("MODE CON CP SELECT=866");
        ::SetConsoleCtrlHandler( HandlerRoutine, TRUE);
	}
    //--------------------------------------------------------------------------
    

    void SaveConsoleToFile(const AnsiString& fn)
	{
        ForceDirectories(  ExtractFilePath(fn) );

		struct FindEndNotSpace
    	{
    		char* operator()( char *b, char *e ) const
			{
				while( (*(e-1)==' ' || *(e-1)=='\0' || *(e-1)=='\t') && --e!=b ) ;
    			return e;
			}
    	};
		const HANDLE hStdout = ::GetStdHandle(STD_OUTPUT_HANDLE);
   		DWORD cWritten;

   		COORD coordBufSize, coordBufCoord;
   		SMALL_RECT srctReadRect;
   		CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;

   		// Get the screen buffer information and set the rectangles
   		if( GetConsoleScreenBufferInfo( hStdout, &lConsoleScreenBufferInfo )==FALSE )
        	return;

   		const int
    		szX = lConsoleScreenBufferInfo.dwSize.X,
    		szY = lConsoleScreenBufferInfo.dwSize.Y;

   		std::vector<char> lineBuff( szX+1 );

   		DWORD numberOfCharsRead;

        ::SetLastError(0);
		std::ofstream logCout( fn.c_str() );
        if( !logCout.is_open() )
        {
        	ShowLastWindowsError();
            return;
        }

    	COORD rdCrd;
    	for( rdCrd.Y = 0; rdCrd.Y<szY; ++rdCrd.Y )
    	{
        	rdCrd.X = 0;
            char *pBuff = &*lineBuff.begin()
            //, pBuffEnd = &*lineBuff.end()
            ;
        	::ReadConsoleOutputCharacterA
        	(
    			hStdout,			// handle of a console screen buffer
    			pBuff,			    // address of buffer to receive characters
    			szX,				// number of character cells to read from
    			rdCrd,				// coordinates of first cell to read from
    			&numberOfCharsRead 	// address of number of cells read from
   			);
            char *lineBuffEnd = FindEndNotSpace()( &*lineBuff.begin(), &*lineBuff.end() );
			*lineBuffEnd = '\0';
			const unsigned cchDstLength = lineBuffEnd - &*lineBuff.begin();

			if( cchDstLength==0 ) continue;


			std::vector<char> translatedBuff( cchDstLength+1 );
			char *pTranslatedBuff = &*translatedBuff.begin();

			::OemToCharBuffA
			(
				pBuff,	                    // pointer to string to translate
				pTranslatedBuff,	        // pointer to buffer for translated string
				cchDstLength  	            // size of buffer
			);

			logCout << pTranslatedBuff << std::endl;
		}
		logCout.close();
	}
//------------------------------------------------------------------------------
void BackupGUIConsole()
{
    Word year, month, day, hour, minute, second, milliSecond;
    DecodeDateTime( Now(), year, month, day, hour, minute, second, milliSecond);

    const AnsiString
        logDir = MYSPRINTF_( "%s\\Log\\%d\\%s\\", MyGetExePath().c_str(), year, MyFormatMonth(month) ),
        logFn = MYSPRINTF_( "%s%d_%d_%d_%d_%d.log",logDir,  day, hour, minute, second, milliSecond );
    ForceDirectories(logDir);
    SaveConsoleToFile( logFn );

}
//------------------------------------------------------------------------------

ConsoleScreenAtrributesHolder::ConsoleScreenAtrributesHolder(IncExcAttr inc_exc)
{
    HANDLE hConsoleOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
    ::GetConsoleScreenBufferInfo( hConsoleOutput,  &csbi );
    attrs_ = csbi.wAttributes;
    ::SetConsoleTextAttribute(hConsoleOutput, (attrs_ | inc_exc.first ) & ~inc_exc.second );
}
//------------------------------------------------------------------------------
ConsoleScreenAtrributesHolder::~ConsoleScreenAtrributesHolder()
{
    ::SetConsoleTextAttribute( ::GetStdHandle(STD_OUTPUT_HANDLE), attrs_ );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void MyWriteConsole(const wchar_t* str, DWORD nNumberOfCharsToWrite)
{
	DWORD lpNumberOfCharsWritten  = 0;
	::WriteConsoleW( ::GetStdHandle(STD_OUTPUT_HANDLE), (const void*) str,
		nNumberOfCharsToWrite, &lpNumberOfCharsWritten, NULL);
}
//------------------------------------------------------------------------------
void MyWCout(const WideString& s)
{
	const DWORD nNumberOfCharsToWrite = s.Length();
	MyWriteConsole( s.c_bstr(), nNumberOfCharsToWrite );
}
//------------------------------------------------------------------------------
void MyWCout(const WideString& str,  IncExcAttr inc_exc  )
{
	const ConsoleScreenAtrributesHolder obj(inc_exc);
    MyWCout( str );
}
//------------------------------------------------------------------------------
void MyCout(const AnsiString& str)
{
	WideString s = str;
	const DWORD nNumberOfCharsToWrite = s.Length();
	MyWriteConsole( s.c_bstr(), nNumberOfCharsToWrite );
}
//------------------------------------------------------------------------------
void MyCout(const AnsiString& str, IncExcAttr inc_exc )
{
	const ConsoleScreenAtrributesHolder obj(inc_exc);
	MyCout( str );
}
//------------------------------------------------------------------------------
void MyCoutTime(const AnsiString& str, IncExcAttr inc_exc  )
{
    const ConsoleScreenAtrributesHolder obj(inc_exc);
    MYCOUT_( "<%s> %s\n", TimeToStr( Time() ), str ));
}
//------------------------------------------------------------------------------
DelayCounter::DelayCounter() : startMoment_( ::GetTickCount() )
{
}

void DelayCounter::Reset()
{
    startMoment_ = ::GetTickCount();
}

unsigned DelayCounter::Count() const
{
    return ::GetTickCount() - startMoment_;
}

AnsiString DelayCounter::ToStr() const
{
    return FormatDateTime("n:s", IncMilliSecond(0, Count() ) );
}
//------------------------------------------------------------------------------
bool IsConsoleVisible()
{
    HWND hCon = ::GetConsoleWindow();
    WINDOWPLACEMENT conPl = {0};
    ::GetWindowPlacement(hCon, &conPl);
    const bool
        conWndNotVis = ::IsWindowVisible(hCon)==FALSE,
        notVis = conWndNotVis || ( !conWndNotVis && (conPl.showCmd==2) );
    return !notVis;
}
//------------------------------------------------------------------------------
WINDOWPLACEMENT MyShowConsole(unsigned showCmd)
{
    HWND hCon = ::GetConsoleWindow();
    WINDOWPLACEMENT conPl = {0};
    ::GetWindowPlacement(hCon, &conPl);
    const WINDOWPLACEMENT origConPl = conPl;
    long
        &l = conPl.rcNormalPosition.left,
        &t = conPl.rcNormalPosition.top,
        &r = conPl.rcNormalPosition.right,
        &b = conPl.rcNormalPosition.bottom;
    ::ShowWindow(hCon, showCmd );
    if(t>Screen->Width-100 || l>Screen->Height-100 )
    {
        conPl.showCmd = SW_SHOWNORMAL;
        const long
            conWidth = r - l,
            conHeight = b - t;
        l = 10;
        t = 10;
        r = l + conWidth;
        b = t + conHeight;
        ::SetWindowPlacement(hCon, &conPl);
    }
    return origConPl;
}
//------------------------------------------------------------------------------
GUIConsoleInputHelper::GUIConsoleInputHelper() :
    hMainForm_( Screen->ActiveForm ? Screen->ActiveForm->Handle  : HWND() ),
    hCon_(::GetConsoleWindow()),
    hConPar_( ::SetParent( hCon_, NULL ) )
{
    std::freopen("CONIN$", "r", stdin);
    if(hMainForm_)
        ::ShowWindow(hMainForm_, SW_HIDE );

    conPlOrig_ = MyShowConsole(SW_SHOWNORMAL | SW_MAXIMIZE);
}
//------------------------------------------------------------------------------
GUIConsoleInputHelper::~GUIConsoleInputHelper()
{
    ::SetParent( hCon_, hConPar_ );
    if(hMainForm_)
        ShowWindow(hMainForm_, SW_SHOW );
    ::SetWindowPlacement(hCon_, &conPlOrig_);
}
//------------------------------------------------------------------------------
void MyCoutTime(const AnsiString& str  )
{
    const ConsoleScreenAtrributesHolder obj(MY_SET_CONSOLE_GREEN_TEXT);
    MYCOUT_( "<%s> %s\n", TimeToStr( Time() ), str ));
}
//------------------------------------------------------------------------------
ConsoleMultioutputHelper::ConsoleMultioutputHelper()
{
    CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo_ = {0};
    hCon_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ::GetConsoleScreenBufferInfo( hCon_, &lConsoleScreenBufferInfo_ );
    pos0_ = lConsoleScreenBufferInfo_.dwCursorPosition;
    curWriteConsoleCount_ = 0;
}
//------------------------------------------------------------------------------
void ConsoleMultioutputHelper::Set(const AnsiString& s)
{
    CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo_ = {0};
    ::GetConsoleScreenBufferInfo( hCon_, &lConsoleScreenBufferInfo_ );
    const COORD pos = lConsoleScreenBufferInfo_.dwCursorPosition;

    if( curWriteConsoleCount_!=0 )
    {
        ::SetConsoleCursorPosition( hCon_, pos0_);
        MyWCout( AnsiString::StringOfChar(' ', curWriteConsoleCount_) );
        ::SetConsoleCursorPosition( hCon_, pos0_);
    }
    MyWCout(s);
    curWriteConsoleCount_ = s.Length();
    ::SetConsoleCursorPosition( hCon_, pos);
}
//------------------------------------------------------------------------------



