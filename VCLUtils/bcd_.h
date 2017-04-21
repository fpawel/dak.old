//---------------------------------------------------------------------------
#ifndef bcd_H
#define bcd_H
//---------------------------------------------------------------------------

//vcl
#include <SysUtils.hpp>

//std
#include <vector>

#define MAX_BCD_FLOAT 999999

//�������������� �������-����������� ������������ ����� � ������������
double BCDToFloat(const unsigned char* bcd);

bool TryBCDToFloat(const unsigned char* bcd, double&);
double BCDToFloatDef(const unsigned char* bcd, double def);

// ��������� ����� � BCD
void FloatToBCD(double val, unsigned char* res);
std::vector<unsigned char> FloatToBCD(double val);

// ������������� ������ � �����, ���� ����������, ������� MAX_BCD_FLOAT
double StrToFloatBCD( const AnsiString& s );

// ������������� ����� � ������, ���� ����� �� ������ >= MAX_BCD_FLOAT, ������� ""
AnsiString FloatBCDToStr( double val, char decimalSeparator = ',' );






#endif
