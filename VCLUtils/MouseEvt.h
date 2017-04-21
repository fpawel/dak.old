/*
����� ������ � �������
*/

//---------------------------------------------------------------------------
#ifndef MouseEvtH
#define MouseEvtH
//---------------------------------------------------------------------------

// vcl
#include <Controls.hpp>

namespace my
{
    // ���� ����� - true ���� ���� - false
	enum { iMouseLeaved, iMouseEntered };
    enum { replaceOldHandler = true };

    typedef void (__closure *MouseOccurrenceHandler)(TWinControl*, bool);

	void RegisterOnMouseOccurrence( TWinControl* ctlr, MouseOccurrenceHandler cmd);
    void UnregisterOnMouseOccurrence( TWinControl* ctlr);


};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
