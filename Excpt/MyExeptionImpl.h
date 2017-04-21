//---------------------------------------------------------------------------

#ifndef MyExeptionImplH
#define MyExeptionImplH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
//---------------------------------------------------------------------------
#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"

//std
//#include <vector>

//my
#include "MyExcpt.hpp"
    

    /**
	*		MyExternalException
	*		����� ������� ����� ��� ������������ ����������, ���������� �������� ��
	*		��������� � ��������, ����������� MyException.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(External, Exception );
	/**
	*      ���������� ��� ������������ std::exception � �������, ����������� �� ����.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(Std, ExternalException );
    /**
	*  ���������� ��� ������������ Exception �� VCL � �������, ����������� �� ����.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(VCL, ExternalException );
    /**
	*  ���������� ��� ������������ Exception �� VCL � �������, ����������� �� ����.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(AccessViolation, ExternalException );
    /**
	*  	���������� ��� �������������� � ������� ����������, ��� �������� �� �������
    *	����������.
	*/
    DECLARATE_AND_DEFINE_MY_EXCEPTION_CLASS_(Unknown, ExternalException );

    

#endif
