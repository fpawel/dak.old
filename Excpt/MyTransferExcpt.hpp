//------------------------------------------------------------------------------
#ifndef MY_TRANSFER_EXCEPTION_CLASS_DECLARATION__HEADER_INCLUDED__________
#define MY_TRANSFER_EXCEPTION_CLASS_DECLARATION__HEADER_INCLUDED__________
//------------------------------------------------------------------------------

//my
#include "MyExcpt.hpp"


	

    class MyNoAnswerException : public MyException
    {
 	public:

    	MyTransferException(const FileLine& loc, const AnsiString& msg ) :
        	MyException( loc, msg )
        {}
    };

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------