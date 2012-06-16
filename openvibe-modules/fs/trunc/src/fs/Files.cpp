#include "Files.h"

#if defined FS_OS_Linux
 #include <sys/stat.h>
#elif defined FS_OS_Windows
 #include <windows.h>
#else
#endif

#include <iostream>

using namespace FS;
using namespace std;

//  * 2006-08-30 YRD - Portability note : using namespace FS confuses
//                     windows platform SDK because it defines itself
//                     a 'boolean' type. Thus the following define to
//                     force the use of FS::boolean !
#define boolean FS::boolean

#if defined FS_OS_Linux

boolean Files::equals(const char* pFile1, const char* pFile2)
{
	bool l_bResult=true;
	if(pFile1 && pFile2)
	{
		struct stat l_oStat1;
		struct stat l_oStat2;
		boolean l_bStat1=!stat(pFile1, &l_oStat1);
		boolean l_bStat2=!stat(pFile2, &l_oStat2);

		if(!l_bStat1 && !l_bStat2)
		{
			l_bResult=true;
		}
		else if(l_bStat1 && l_bStat2)
		{
			l_bResult=
				(l_oStat1.st_dev==l_oStat2.st_dev)&&
				(l_oStat1.st_ino==l_oStat2.st_ino)&&
				(l_oStat1.st_size==l_oStat2.st_size)&&
				(l_oStat1.st_mtime==l_oStat2.st_mtime);
		}
		else
		{
			l_bResult=false;
		}
	}
	return l_bResult;
}

#elif defined FS_OS_Windows

boolean Files::equals(const char* pFile1, const char* pFile2)
{
	boolean l_bResult=true;
	if(pFile1 && pFile2)
	{
		::HANDLE l_pHandle1=::CreateFile(pFile1, 0, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
		::HANDLE l_pHandle2=::CreateFile(pFile2, 0, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
		if(l_pHandle1 && l_pHandle2)
		{
			::BY_HANDLE_FILE_INFORMATION l_oStat1;
			::BY_HANDLE_FILE_INFORMATION l_oStat2;
			::BOOL l_bStat1=::GetFileInformationByHandle(l_pHandle1, &l_oStat1);
			::BOOL l_bStat2=::GetFileInformationByHandle(l_pHandle2, &l_oStat2);
			if(!l_bStat1 && !l_bStat2)
			{
				l_bResult=true;
			}
			else if(l_bStat1 && l_bStat2)
			{
				l_bResult=
					(l_oStat1.dwVolumeSerialNumber==l_oStat2.dwVolumeSerialNumber)&&
					(l_oStat1.nFileIndexHigh==l_oStat2.nFileIndexHigh)&&
					(l_oStat1.nFileIndexLow==l_oStat2.nFileIndexLow)&&
					(l_oStat1.nFileSizeHigh==l_oStat2.nFileSizeHigh)&&
					(l_oStat1.nFileSizeLow==l_oStat2.nFileSizeLow)&&
					(l_oStat1.ftLastWriteTime.dwHighDateTime==l_oStat2.ftLastWriteTime.dwHighDateTime)&&
					(l_oStat1.ftLastWriteTime.dwLowDateTime==l_oStat2.ftLastWriteTime.dwLowDateTime);
			}
			else
			{
				l_bResult=false;
			}
			::CloseHandle(l_pHandle1);
			::CloseHandle(l_pHandle2);
		}
		else
		{
			if(l_pHandle1) ::CloseHandle(l_pHandle1);
			if(l_pHandle2) ::CloseHandle(l_pHandle2);
		}
	}
	return l_bResult;
}

#else

#endif
