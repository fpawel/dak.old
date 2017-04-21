//---------------------------------------------------------------------------
#include <windows.h>
#include "Loki\singleton.h"

#pragma hdrstop

#include "MyWindow.h"
#include <forms.hpp>
#include "MySynchronize.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//using std::cout;
//using std::endl;



class  WindowsHandleMessageManagerT : public boost::noncopyable
{
public:
    // добавить к оконной продцедуре метод объекта
	void ChainWindowProc( HWND hWnd, HandleWindowMessageMethod method )
    {
    	CIter e = wndDt_.end(), fnd = wndDt_.find(hWnd);
    	assert( fnd==e );

    	MyWindowData wndDt;
        wndDt.method_ = method;
        wndDt.originalWndProc_ = (WNDPROC)SetWindowLong(hWnd,
			GWL_WNDPROC, (long)MyWndProc);
        wndDt_[hWnd] = wndDt;
    }
private:
	struct MyWindowData
    {
    	HandleWindowMessageMethod method_;
        WNDPROC originalWndProc_;
    };


    typedef std::map< HWND, MyWindowData > MyWindowDataMap;
    typedef MyWindowDataMap::const_iterator CIter;

    MyWindowDataMap wndDt_;
    static LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam,
    	LPARAM lParam);
};

typedef Loki::SingletonHolder
    	<
    		WindowsHandleMessageManagerT,
        	Loki::CreateUsingNew,
    		Loki::NoDestroy
    	> WindowsHandleMessageManager;

LRESULT CALLBACK WindowsHandleMessageManagerT::MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WindowsHandleMessageManagerT& inst = WindowsHandleMessageManager::Instance();
    const MyWindowDataMap& wndDtMap = inst.wndDt_;
    CIter e = wndDtMap.end(), fnd = wndDtMap.find(hWnd);
    if( fnd!=e )
    {
    	const MyWindowData& wndDt = fnd->second;
    	bool isHandled = false;
        if( wndDt.method_!=NULL )
        {
    		const LRESULT res = wndDt.method_(hWnd, msg, wParam, lParam, isHandled);
    		return isHandled ? res : ::CallWindowProc( (FARPROC)wndDt.originalWndProc_,
    			hWnd, msg, wParam, lParam);
        }
        else
        	return ::DefWindowProc( hWnd, msg, wParam, lParam );
    }
    else
    	return ::DefWindowProc( hWnd, msg, wParam, lParam );


}

// добавить к оконной продцедуре метод объекта
void MyChainWindowProc( HWND hWnd, HandleWindowMessageMethod method )
{
	WindowsHandleMessageManager::Instance().ChainWindowProc( hWnd, method );
}

HWND MyAllocateWindow( const char* wndClassName, const char* wndName,
	HandleWindowMessageMethod method )
{
    WNDCLASS w = {0};
    w.style = CS_HREDRAW | CS_VREDRAW;
    w.lpfnWndProc = ::DefWindowProc;
    w.hInstance = NULL;
    w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    w.lpszClassName = wndClassName;
    RegisterClass(&w);
    const HWND ret = CreateWindow(wndClassName, wndName, WS_OVERLAPPEDWINDOW,
        0, 0, 100, 100, NULL, NULL, NULL, NULL);
    MyChainWindowProc( ret, method );
    return ret;
}

void MyMessageBox(HWND hWnd,	LPCTSTR lpText,	LPCTSTR lpCaption,	UINT uType  )
{
    struct
    {
        LRESULT Do(HWND hWnd,	LPCTSTR lpText,	LPCTSTR lpCaption,	UINT uType  )
        {

            #define MSGBOX_(hh) ::MessageBox(hh, lpText, lpCaption, uType )
	        if(hWnd!=NULL) return MSGBOX_(hWnd);
            if(TCustomForm *frm = Screen->ActiveCustomForm)
                return MSGBOX_(frm->Handle);
            const HWND myHWnd = MyAllocateWindow( "MyMessageBoxWindow", NULL, NULL );
            ::ShowWindow( myHWnd,  SW_SHOWMINIMIZED );
            const int ret  = MSGBOX_(myHWnd);
            ::DestroyWindow(myHWnd);
            return ret;
            #undef MSGBOX_(hh)
        }
    } h;
    my::Synchronize4( &h.Do, hWnd,	lpText,	lpCaption,	uType, __FILE_LINE__ );

}

