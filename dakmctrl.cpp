//------------------------------------------------------------------------------
#include <vcl.h>
//------------------------------------------------------------------------------
#include <cassert>
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "Unit1.h"
#include "MyProcNfo.hpp"
#include "MyModFN.hpp"
#include "FreeAndNuLL.hpp"
#include "MyIostream.h"
#include "guicon.h"
#include "MyWindow.h"
#include "AnsiStringUtils_.h"
#include "MyFileCtrl.h"
#include "MyExcpt.hpp"
#include "TransferManage.h"
#include "ctrlsys.h"
#include "MySynchronize.h"
#include "AppXML.h"
//------------------------------------------------------------------------------
USEFORM("unit1.cpp", Form1);
USEFORM("Dialogs\uFrmWait.cpp", FrmWait);
USEFORM("uFormPneumoGrd.cpp", FormPneumoGrd);
USEFORM("uFormMdiChild.cpp", FormMdiChild);
USEFORM("uAppSetsDlg.cpp", FormAppSetsDlg);
USEFORM("uFrameGrid.cpp", FrameGrid); /* TFrame: File Type */
USEFORM("uFrameDevicePasport.cpp", FrameDevicePasport); /* TFrame: File Type */
USEFORM("uFrameTestConcData.cpp", FrameTestConcData); /* TFrame: File Type */
USEFORM("uFrameGraph.cpp", FrameGraph); /* TFrame: File Type */
USEFORM("uFrameDropDownPanel.cpp", FrameDropDownPanel); /* TFrame: File Type */
//---------------------------------------------------------------------------
void RunVCLApplication()
{
	try
	{
		Application->Initialize();
		Application->Title = "��������� ���";
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
}
//------------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{             
    my::FixMainThread();
	Form1 = NULL;
    // ������������� ������� �������
    CreateGUIConsole();

    MyCout(myHexToInt( "FFd8" ));
    MyCout("\n");
    try
    {
        CtrlSysImpl& ctrlSys = CtrlSys().Instance();
        TransferManagerT& tmngr = TransferManager().Instance();

    	// ����� ����� � VCL!!!
		RunVCLApplication();
    	//���������� �����
        tmngr.StopThread();
    	// ������� Form1
    	FreeAndNULL(Form1);
        
        delete &ctrlSys;
        delete &tmngr;
    }
    catch ( ... )
    {
    	const PMyExcpt excpt = MY_RETHROW_;
        const AnsiString msg = FormatMyExceptionMessages( excpt.get(), "\n\t",
            "%eFile:%eLine\n%eMsg"  );
        MyWCout("����������! ����� �� ����������!\n"+msg+"\n" );
        MyMessageBox(NULL, msg.c_str(), "��������� ���",
        	MB_OK | MB_ICONERROR | MB_SYSTEMMODAL   );
    }

    MyWCout("�������� ���������� ������� � �������\n");
    BackupGUIConsole();

    // ������� �������
    ::FreeConsole();
    GetAppXML().doc->SaveFile();

	return 0;
}
//---------------------------------------------------------------------------
