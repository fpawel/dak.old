#ifndef MY_GET_MODULE_FILE_NAME_HEADER_INCLUDED__________
#define MY_GET_MODULE_FILE_NAME_HEADER_INCLUDED__________
//------------------------------------------------------------------------------
#include <string>
#include <windows>
//------------------------------------------------------------------------------
std::string MyExtractFileName( const std::string& path )
{
	char lpBuffer[MAX_PATH],  *lpFilePart = NULL;
	GetFullPathName( path.c_str(),  MAX_PATH-1, lpBuffer, &lpFilePart);
	lpBuffer[MAX_PATH-1] = '\0';
	return lpFilePart==NULL ? std::string("") : std::string( lpFilePart );
}
//------------------------------------------------------------------------------
std::string MyExtractFilePath( const std::string& path )
{
	char lpBuffer[MAX_PATH],  *lpFilePart = NULL;
	GetFullPathName( path.c_str(),  MAX_PATH-1, lpBuffer, &lpFilePart);
	lpBuffer[MAX_PATH-1] = '\0';
	return lpFilePart==NULL ? std::string(lpBuffer) :
    std::string( static_cast<const char*>(lpBuffer), static_cast<unsigned int>(lpFilePart-lpBuffer) );
}
//------------------------------------------------------------------------------
std::string MyGetExeFileName()
{
	char lpFilename[MAX_PATH];
	const DWORD len=::GetModuleFileName( NULL, lpFilename, MAX_PATH );
    lpFilename[MAX_PATH-1] = '\0';
    return len==0 ? "" : lpFilename;
}
//------------------------------------------------------------------------------
std::string MyGetExePath()
{
	return MyExtractFilePath( MyGetExeFileName() );
}
//------------------------------------------------------------------------------
std::string MyGetExeName()
{
	return MyExtractFileName( MyGetExeFileName() );
}
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
