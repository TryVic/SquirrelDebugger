//---------------------------------------------------------------------------


#pragma hdrstop

#include "Path.h"

#include <shlwapi.h>
#pragma link "shlwapi.lib"

//---------------------------------------------------------------------------
TPath Path;
//---------------------------------------------------------------------------
UnicodeString TPath::RelativePath (UnicodeString FullPath)
{
	if (!FullPath.Length())
		return "";

	if (*Scripts.LastChar()!='\\')
		Scripts += '\\';

	wchar_t res[MAX_PATH];
	PathRelativePathToW(res,
		Scripts.c_str(),
		FILE_ATTRIBUTE_NORMAL,
		FullPath.c_str(),
		FILE_ATTRIBUTE_NORMAL);

	UnicodeString us = res;
	if (!us.Length())
		us = FullPath;

	us = StringReplace(us, "\\" , "/", TReplaceFlags()<<rfReplaceAll);
	if (us.Pos("./")==1)
		us.Delete(1,2);
	return us;
}
//---------------------------------------------------------------------------
UnicodeString TPath::RestorePath (UnicodeString vFileName)
{
	if (!vFileName.Length()) return "";

	vFileName = StringReplace(vFileName, "/" , "\\", TReplaceFlags()<<rfReplaceAll);
	if (DirectoryExists(vFileName) || FileExists(vFileName))
		return vFileName;

	// Обработка по указанной схеме
	wchar_t file[MAX_PATH];

	if (!vFileName.Pos(":\\"))
		vFileName = ".\\"+vFileName;

	if (*Scripts.LastChar()!='\\')
		Scripts += '\\';

	// поиск проекта
	PathCombineW(file, Scripts.c_str(), vFileName.c_str());
	if (DirectoryExists(vFileName) || FileExists(file))
		return file;

	return vFileName;
}
//---------------------------------------------------------------------------
UnicodeString TPath::RelativePath (UnicodeString FullPath, UnicodeString Dir)
{
	if (!FullPath.Length() || !Dir.Length())
		return "";

	if (*Dir.LastChar()!='\\')
		Dir += '\\';

	wchar_t res[MAX_PATH];

	PathRelativePathToW(res,
		Dir.c_str(),
		FILE_ATTRIBUTE_NORMAL,
		FullPath.c_str(),
		FILE_ATTRIBUTE_NORMAL);

	UnicodeString us = res;
	if (!us.Length())
		us = FullPath;

	us = StringReplace(us, "\\" , "/", TReplaceFlags()<<rfReplaceAll);
	if (us.Pos("./")==1)
		us.Delete(1,2);
	return us;
}
//---------------------------------------------------------------------------
UnicodeString TPath::RestorePath (UnicodeString vFileName, UnicodeString Dir)
{
	if (!vFileName.Length() || !Dir.Length()) return "";

	Dir = StringReplace(Dir,"/","\\",TReplaceFlags()<<rfReplaceAll);
	vFileName = StringReplace(vFileName, "/" , "\\", TReplaceFlags()<<rfReplaceAll);
	if (DirectoryExists(vFileName) || FileExists(vFileName))
		return vFileName;

	// Обработка по указанной схеме
	wchar_t file[MAX_PATH];

	if (!vFileName.Pos(":\\"))
		vFileName = ".\\"+vFileName;

	if (*Dir.LastChar()!='\\')
		Dir += '\\';

	// поиск проекта
	PathCombineW(file, Dir.c_str(), vFileName.c_str());
	if (DirectoryExists(vFileName) || FileExists(file))
		return file;

	return vFileName;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
