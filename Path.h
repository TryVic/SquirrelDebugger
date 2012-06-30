//---------------------------------------------------------------------------

#ifndef PathH
#define PathH

#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TPath
{
public:
	TPath () {
		Program = ExtractFilePath(Application->ExeName);

		FileConfig = Program+"config.xml";
	}

	UnicodeString RelativePath (UnicodeString FullPath);
	UnicodeString RestorePath (UnicodeString vFileName);

	UnicodeString RelativePath (UnicodeString FullPath, UnicodeString Dir);
	UnicodeString RestorePath (UnicodeString vFileName, UnicodeString Dir);

	UnicodeString Program;
	UnicodeString Scripts;

	UnicodeString FileConfig;
	UnicodeString FileProject;
};

extern TPath Path;
//---------------------------------------------------------------------------
#endif
