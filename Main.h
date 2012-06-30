//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "IdBaseComponent.hpp"
#include "IdComponent.hpp"
#include "IdTCPClient.hpp"
#include "IdTCPConnection.hpp"
#include "sButton.hpp"
#include "sCustomComboEdit.hpp"
#include "sMaskEdit.hpp"
#include "sMemo.hpp"
#include "sTooledit.hpp"
#include <Mask.hpp>
#include "sComboBox.hpp"
#include <ExtCtrls.hpp>
#include "sStatusBar.hpp"
#include <ComCtrls.hpp>
#include <msxmldom.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>
#include "sTreeView.hpp"
#include "sSpeedButton.hpp"
#include <Buttons.hpp>
#include "sEdit.hpp"
#include "sListBox.hpp"
#include "sPanel.hpp"
#include "sSplitter.hpp"
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include <Menus.hpp>
#include "acAlphaImageList.hpp"
#include <ImgList.hpp>
#include "sDialogs.hpp"
#include <Dialogs.hpp>

#include <vector>
#include <map>
//---------------------------------------------------------------------------
class TfmMain : public TForm
{
__published:	// IDE-managed Components
	TIdTCPClient *tcp;
	TsStatusBar *sb;
	TTimer *tm;
	TXMLDocument *xml;
	TsPanel *sPanel1;
	TsListBox *mm;
	TsPanel *sPanel2;
	TsTreeView *stack;
	TsTreeView *var;
	TsPanel *sPanel3;
	TsSpeedButton *sSpeedButton6;
	TsSpeedButton *sSpeedButton5;
	TsSpeedButton *sSpeedButton4;
	TsSpeedButton *sSpeedButton3;
	TsSpeedButton *sSpeedButton2;
	TsSpeedButton *sSpeedButton1;
	TsDirectoryEdit *dir;
	TsSplitter *sSplitter1;
	TsSplitter *sSplitter2;
	TsSkinManager *skin;
	TsSkinProvider *provider;
	TMainMenu *menu;
	TMenuItem *btConnect;
	TMenuItem *Debug1;
	TMenuItem *Stepin1;
	TMenuItem *Stepover1;
	TMenuItem *Step1;
	TMenuItem *N1;
	TMenuItem *Run1;
	TMenuItem *Pause1;
	TMenuItem *N2;
	TMenuItem *Stop1;
	TMenuItem *View1;
	TMenuItem *Skins1;
	TMenuItem *Cold1;
	TMenuItem *Acryl1;
	TMenuItem *MacOS1;
	TMenuItem *N3;
	TMenuItem *miSkinActive;
	TMenuItem *Other1;
	TMenuItem *N4;
	TMenuItem *About1;
	TsSpeedButton *btBreakpoint;
	TMenuItem *Breakpoint1;
	TMenuItem *File1;
	TMenuItem *N5;
	TMenuItem *miOpen;
	TMenuItem *Save1;
	TMenuItem *Saveas1;
	TMenuItem *N6;
	TMenuItem *Exit1;
	TMenuItem *N7;
	TMenuItem *Options1;
	TsTreeView *tvFiles;
	TsAlphaImageList *imgList16;
	TsSpeedButton *btRefreshDir;
	TsSplitter *sSplitter3;
	TsOpenDialog *dlgOpen;
	TsSaveDialog *dlgSave;
	TsSpeedButton *sbConnect;
	void __fastcall btConnectClick(TObject *Sender);
	void __fastcall tcpConnected(TObject *Sender);
	void __fastcall dirBeforeDialog(TObject *Sender, UnicodeString &Name, bool &Action);
	void __fastcall tcpWork(TObject *ASender, TWorkMode AWorkMode, __int64 AWorkCount);
	void __fastcall tcpDisconnected(TObject *Sender);
	void __fastcall tcpStatus(TObject *ASender, const TIdStatus AStatus, const UnicodeString AStatusText);
	void __fastcall tmTimer(TObject *Sender);
	void __fastcall OnDebugAction(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall mmDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall miSkinActiveClick(TObject *Sender);
	void __fastcall OnChooseSkin(TObject *Sender);
	void __fastcall About1Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall mmClick(TObject *Sender);
	void __fastcall dirAfterDialog(TObject *Sender, UnicodeString &Name, bool &Action);
	void __fastcall btRefreshDirClick(TObject *Sender);
	void __fastcall miOpenClick(TObject *Sender);
	void __fastcall Saveas1Click(TObject *Sender);
	void __fastcall Save1Click(TObject *Sender);
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall tvFilesDblClick(TObject *Sender);
	void __fastcall btBreakpointClick(TObject *Sender);
private:	// User declarations
	int skinIdx;
	UnicodeString cmd;

public:		// User declarations
	typedef std::vector<int> int_v;
	typedef std::map<UnicodeString, int_v> break_list;

	int line;
	UnicodeString file;
	break_list lists;
	int_v list;

	TColor clDefBack;
	TColor clDefFont;
	TColor clBreakBack;
	TColor clBreakFont;
	TColor clPosBack;
	TColor clPosFont;
	TColor clSelBack;
	TColor clSelFont;

	void GetData (int ms = 10);

	void UpdateColor ();

	void LoadProject (const UnicodeString &file);
	void SaveProject (const UnicodeString &FileName);
	void FindFiles (const UnicodeString &path, TTreeNode *node);

	void Break (IXMLNode *node);
	void ParseObjs (IXMLNode *node, TTreeNode *tree);
	void ParseCalls (IXMLNode *node);

	__fastcall TfmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmMain *fmMain;
//---------------------------------------------------------------------------
#endif
