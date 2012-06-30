//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "IdGlobal.hpp"
#include "About.h"
#include "Path.h"

#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdTCPClient"
#pragma link "IdTCPConnection"
#pragma link "sButton"
#pragma link "sCustomComboEdit"
#pragma link "sMaskEdit"
#pragma link "sMemo"
#pragma link "sTooledit"
#pragma link "sStatusBar"
#pragma link "sTreeView"
#pragma link "sSpeedButton"
#pragma link "sEdit"
#pragma link "sListBox"
#pragma link "sPanel"
#pragma link "sSplitter"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma link "acAlphaImageList"
#pragma link "sDialogs"
#pragma resource "*.dfm"
TfmMain *fmMain;
//---------------------------------------------------------------------------
__fastcall TfmMain::TfmMain(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btConnectClick(TObject *Sender)
{
	if (tcp->Connected()) {
		tcp->DisconnectNotifyPeer();
		btConnect->Caption = "Connect";
		btConnect->ImageIndex = 10;
		sbConnect->ImageIndex = 10;
	}
	else {
		tcp->Connect();
    	btConnect->Caption = "Disconnect";
		btConnect->ImageIndex = 11;
		sbConnect->ImageIndex = 11;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::tcpConnected(TObject *Sender)
{
	tcp->Socket->WriteLn("rd\nsp",enUTF8);

	GetData();
	tm->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::dirBeforeDialog(TObject *Sender, UnicodeString &Name, bool &Action)
{
	if (dir->Text=="") {
		dir->InitialDir = ExtractFileDir(Application->ExeName);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::dirAfterDialog(TObject *Sender, UnicodeString &Name, bool &Action)
{
	dir->Text = Name;
	btRefreshDirClick(Sender);
}
//---------------------------------------------------------------------------
int CALLBACK FileSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	TTreeNode *Node1 = reinterpret_cast<TTreeNode *>(lParam1);
	TTreeNode *Node2 = reinterpret_cast<TTreeNode *>(lParam2);
	if ((Node1 == NULL) || (Node2 == NULL)) return 0;
	if (Node1->ImageIndex==0 && Node2->ImageIndex!=0) return -1;
	if (Node1->ImageIndex!=0 && Node2->ImageIndex==0) return 1;
	return AnsiStrIComp(Node1->Text.t_str(), Node2->Text.t_str());
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btRefreshDirClick(TObject *Sender)
{
	try
	{
		tvFiles->Items->BeginUpdate();
		tvFiles->Items->Clear();

		Path.Scripts = dir->Text;
		if (!Path.Scripts.Length())
			return;

		if (*Path.Scripts.LastChar()!='\\')
			Path.Scripts += '\\';

		if (DirectoryExists(Path.Scripts)) {
			FindFiles(Path.Scripts,NULL);
			tvFiles->CustomSort(&FileSort, 0, true);
		}
	}
	__finally
	{
		tvFiles->Items->EndUpdate();
	}
}
//---------------------------------------------------------------------------
void TfmMain::FindFiles (const UnicodeString &path, TTreeNode *node)
{
	TSearchRec F;

	try
	{
		if (FindFirst(path+"\\*.*", faAnyFile, F)) return;

		UnicodeString file, ext;
		do {
			if (F.Name=="." || F.Name=="..")
				continue;

			file = path+"\\"+F.Name;
			// Пропуск специфических файлов
			if (F.Attr & faDirectory) {
				TTreeNode *dir = tvFiles->Items->AddChild(node, F.Name);
				FindFiles(file, dir);
				if (!dir->HasChildren)
					dir->Delete();

				continue;
			};


			ext = AnsiLowerCase(ExtractFileExt(F.Name));
			if (ext==".nut") {
				TTreeNode *file = tvFiles->Items->AddChild(node, F.Name);
				file->ImageIndex = 1;
				file->SelectedIndex = 1;
			}
		} while (FindNext(F)==0);
	}
	__finally
	{
    	FindClose(F);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::tcpWork(TObject *ASender, TWorkMode AWorkMode, __int64 AWorkCount)
{
	Beep();
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::tcpDisconnected(TObject *Sender)
{
	tm->Enabled = false;

	mm->Clear();
	mm->AddItem("NO CONNECTION",NULL);
	line = -1;
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::tcpStatus(TObject *ASender, const TIdStatus AStatus, const UnicodeString AStatusText)
{
	(*sb->Panels)[1]->Text = AStatusText;
}
//---------------------------------------------------------------------------
void TfmMain::GetData (int ms)
{
	if (!tcp->Connected()) {
		tm->Enabled = false;
		return;
    }

	UnicodeString us;

	us = "";
	while (tcp->Socket->Readable(ms)) {
		us = tcp->Socket->ReadLn(enUTF8);
		if (us!="") {
			_di_IXMLDocument xml = NewXMLDocument();
			try
			{
				xml->XML->Text = us;
				xml->Active = true;
			}
			catch (...)
			{
				TStringStream *ss = new TStringStream(us);
				ss->SaveToFile(Path.Program+"\\unrecognized.xml");

				MessageDlg("Unrecognized response", mtError, TMsgDlgButtons()<<mbOK, 0);
				return;
			}

			IXMLNode *n = xml->ChildNodes->FindNode("break");
			if (n) {
				Break(n);
			}
			else if (n=xml->ChildNodes->FindNode("resume")) {

			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::tvFilesDblClick(TObject *Sender)
{
	if (tvFiles->Selected) {
		UnicodeString us = tvFiles->Selected->Text;
		for (TTreeNode *tr=tvFiles->Selected->Parent; tr; tr=tr->Parent) {
			us = tr->Text + "\\" + us;
		}

		us = Path.Scripts+us;
		if (FileExists(us)) {
        	if (file!="")
				lists[file] = list;

			list.clear();
			file = us;
			list = lists[file];

			mm->Items->LoadFromFile(file);
			mm->BoundLabel->Caption = file;
			line = -1;
		}
	}
}
//---------------------------------------------------------------------------
void TfmMain::Break (IXMLNode *node)
{
	UnicodeString thread = node->GetAttribute("thread");
	line = node->GetAttribute("line");

	if (file!="")
		lists[file] = list;

	list.clear();

	file = Path.Scripts+StringReplace(node->GetAttribute("src"),"/","\\", TReplaceFlags()<<rfReplaceAll);
	UnicodeString type = node->GetAttribute("type");
	if (node->HasAttribute("error")) {
		MessageDlg(node->GetAttribute("error"), mtError, TMsgDlgButtons()<<mbOK, 0);
	}

	(*sb->Panels)[1]->Text = thread+"> "+file+":"+IntToStr(line)+" ("+type+")";

	if (FileExists(file)) {
		list = lists[file];
		mm->Items->LoadFromFile(file);
		mm->ItemIndex = Min(line-1,mm->Count-1);
		mm->BoundLabel->Caption = file;
	}
	else {
		mm->Clear();
		mm->AddItem("File not found: "+file,NULL);
		mm->BoundLabel->Caption = "Code";
		line = -1;
    }

	IXMLNode *n = node->ChildNodes->FindNode("objs");
	if (n) {
		try
		{
			var->Items->BeginUpdate();
			var->Items->Clear();

			ParseObjs(n,NULL);
		}
		__finally
		{
			var->Items->EndUpdate();
		}
	}

	n = node->ChildNodes->FindNode("calls");
	if (n) {
    	try
		{
			stack->Items->BeginUpdate();
			stack->Items->Clear();

			ParseCalls(n);
		}
		__finally
		{
			stack->Items->EndUpdate();
		}
	}


}
//---------------------------------------------------------------------------
void TfmMain::ParseObjs (IXMLNode *node, TTreeNode *tree)
{
	for (int i=0; i<node->ChildNodes->Count; i++) {
		IXMLNode *child = node->ChildNodes->Get(i);
		TTreeNode *nT;
		if (child->NodeName=="o") {
			nT = var->Items->AddChild(tree,
				child->GetAttribute("type").operator AnsiString()+" ["+child->GetAttribute("ref")+"] ");
			nT->ImageIndex = 24;
			nT->SelectedIndex = 24;
		}
		else {
			UnicodeString v = child->GetAttribute("vt");
			UnicodeString kt = child->GetAttribute("kt");
			UnicodeString res = child->GetAttribute("kv");
			if (kt.Length()) switch (kt[1]) {
			case 's': res = "\""+res+"\""; break;
			case 'i':
			case 'f': break;
			case 't': res = "{"+res+"}"; break;
			case 'a': res = "["+res+"]"; break;
			default: res += " ("+kt+")";
			}

			res += " = "+child->GetAttribute("v");
			nT = var->Items->AddChild(tree,res);

			int i = -1;

			if (v.Length()) switch (v[1]) {
			case 'n': i = 12; break;
			case 's': i = 13; break;
			case 'i': i = 14; break;
			case 'f':
				if (v=="fn") i = 17;
				else i = 15;
			break;
			case 'u': i = 16; break;
			case 't': i = 18; break;
			case 'a': i = 19; break;
			case 'g': i = 20; break;
			case 'h': i = 21; break;
			case 'x': i = 22; break;
			case 'y': i = 23; break;
			case 'b': i = 25; break;
			case 'w': i = 26; break;
			}

			nT->ImageIndex = i;
			nT->SelectedIndex = i;
		}

		if (child->HasChildNodes) {
			ParseObjs (child, nT);
		}

		if (nT) {
            nT->Expand(false);
		}
	}
}
//---------------------------------------------------------------------------
void TfmMain::ParseCalls (IXMLNode *node)
{
	UnicodeString n, t, v;
	for (int i=0; i<node->ChildNodes->Count; i++) {
		IXMLNode *child = node->ChildNodes->Get(i);
		TTreeNode *nT = stack->Items->Add(NULL,
			child->GetAttribute("fnc").operator AnsiString()+" ["+child->GetAttribute("src")
			+":"+child->GetAttribute("line")+"] ");
		//nT->ImageIndex = 1;
		//nT->SelectedIndex = 1;

		for (int j = 0; j < child->ChildNodes->Count; j++) {
			IXMLNode *c = child->ChildNodes->Get(j);
			if (c->NodeName=="l") {
				if (c->HasAttribute("name"))
					n = c->GetAttribute("name");
				if (c->HasAttribute("type"))
					t = c->GetAttribute("type");
				if (c->HasAttribute("val"))
					v = c->GetAttribute("val");

				TTreeNode *rec = stack->Items->AddChild(nT, n+" = "+v);
				int i = -1;

				if (t.Length()) switch (t[1]) {
				case 'n': i = 12; break;
				case 's': i = 13; break;
				case 'i': i = 14; break;
				case 'f':
					if (t=="fn") i = 17;
					else i = 15;
				break;
				case 'u': i = 16; break;
				case 't': i = 18; break;
				case 'a': i = 19; break;
				case 'g': i = 20; break;
				case 'h': i = 21; break;
				case 'x': i = 22; break;
				case 'y': i = 23; break;
				case 'b': i = 25; break;
				case 'w': i = 26; break;
				}

				rec->ImageIndex = i;
				rec->SelectedIndex = i;
			}
		}

		if (nT) {
            nT->Expand(false);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::tmTimer(TObject *Sender)
{
	GetData();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::OnDebugAction(TObject *Sender)
{
	if (!tcp->Connected())
		return;

	UnicodeString us;

	TComponent *sp = (TComponent*) Sender;
	switch (sp->Tag) {
	case 0: us = "si"; break;
	case 1: us = "so"; break;
	case 2: us = "sr"; break;
	case 3: us = "go";
		mm->Clear();
		mm->AddItem("RESUME",NULL);
		line = -1;
	break;
	case 4: us = "sp"; break;
	case 5: us = "tr"; tm->Enabled=false;
    	mm->Clear();
		mm->AddItem("TERMINATE",NULL);
		line = -1;
	break;

	case 10: {
    	us = cmd+IntToHex(mm->ItemIndex+1,4)+":"+Path.RelativePath(file);
	} break;

	default:
		;
	}

	tcp->Socket->WriteLn(us,enUTF8);
	GetData();

	if (sp->Tag==5) {
		tcp->DisconnectNotifyPeer();
	}
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btBreakpointClick(TObject *Sender)
{
	if (mm->ItemIndex<0)
		return;

	int_v::iterator it = std::find(list.begin(), list.end(), mm->ItemIndex);
	if (it==list.end()) {
		list.push_back(mm->ItemIndex);
		cmd = "ab:";
	}
	else {
		list.erase(it);
		cmd = "rb:";
	}

	if (tcp->Connected())
		OnDebugAction(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mmDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State)
{
	TCanvas *c = mm->Canvas;
	c->Font = mm->Font;

	if (Index==mm->ItemIndex) {
		c->Brush->Color = clSelBack;
		c->Font->Color = clSelFont;
	}
	else if (line-1==Index) {
		c->Brush->Color = clPosBack;
		c->Font->Color = clPosFont;
	}
	else {
		int_v::iterator it = std::find(list.begin(), list.end(), Index);
		if (it!=list.end()) {
			c->Brush->Color = clBreakBack;
			c->Font->Color = clBreakFont;
		}
		else {
			c->Brush->Color = clDefBack;
			c->Font->Color = clDefFont;
		}
	}

	c->FillRect(Rect);
	if (Index>=0) {
		c->TextOut(Rect.Left, Rect.Top,
			StringReplace(mm->Items->Strings[Index],"\t","  ",TReplaceFlags()<<rfReplaceAll));
	}
}
//---------------------------------------------------------------------------
void TfmMain::UpdateColor ()
{
	if (skin->Active) {
		clDefBack = skin->GetActiveEditColor();
		clDefFont = skin->GetActiveEditFontColor();
		clBreakBack = clMaroon;
		clBreakFont = clWhite;
		clPosBack = clBlue;
		clPosFont = clWhite;
		clSelBack = skin->GetHighLightColor();
		clSelFont = skin->GetHighLightFontColor();
	}
	else {
		clDefBack = clWindow;
		clDefFont = clWindowText;
		clBreakBack = clMaroon;
		clBreakFont = clWhite;
		clPosBack = clBlue;
		clPosFont = clWhite;
		clSelBack = clHighlight;
		clSelFont = clHighlightText;
	}

	mm->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::miSkinActiveClick(TObject *Sender)
{
	skin->Active = miSkinActive->Checked;
	UpdateColor();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::OnChooseSkin(TObject *Sender)
{
	TComponent *cp = (TComponent*) Sender;
	skin->SkinName = (*skin->InternalSkins)[cp->Tag]->Name;
	UpdateColor();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::About1Click(TObject *Sender)
{
	//
	fmAbout->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::mmClick(TObject *Sender)
{
	mm->Repaint();
	(*sb->Panels)[0]->Text = IntToStr(mm->ItemIndex+1) + "/" + IntToStr(mm->Count);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::FormCreate(TObject *Sender)
{
	dir->Text = ExtractFileDir(Application->ExeName);

	if (FileExists(Path.FileConfig)) {
		xml->FileName = Path.FileConfig;
		xml->Active = true;

		IXMLNode *all = xml->ChildNodes->FindNode("Config");
		if (all) {
			IXMLNode *child = all->ChildNodes->FindNode("View");
			if (child) {
				skin->SkinName = child->GetAttribute("skin");
				miSkinActive->Checked = child->GetAttribute("active");
				skin->Active = miSkinActive->Checked;
			}

			child = all->ChildNodes->FindNode("Project");
			if (child) {
				LoadProject (Path.RestorePath(child->GetText(), Path.Program));
			}
		}
	}

	UpdateColor();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::FormDestroy(TObject *Sender)
{
	xml->Active = false;
	xml->XML->Clear();
	xml->FileName = "";
	xml->Active = true;

	IXMLNode *all = xml->AddChild("Config");

	IXMLNode *child = all->AddChild("View");
	child->SetAttribute("active",miSkinActive->Checked);
	child->SetAttribute("skin",skin->SkinName);

	if (FileExists(Path.FileProject)) {
		SaveProject(Path.FileProject);

		child = all->AddChild("Project");
		child->SetText(Path.RelativePath(Path.FileProject,Path.Program));
	}

	xml->SaveToFile(Path.FileConfig);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::miOpenClick(TObject *Sender)
{
	if (!dlgOpen->Execute())
		return;

	LoadProject(dlgOpen->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::Saveas1Click(TObject *Sender)
{
	if (!dlgSave->Execute())
		return;

	SaveProject(dlgSave->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::Save1Click(TObject *Sender)
{
	if (FileExists(Path.FileProject)) {
		SaveProject(Path.FileProject);
	}
	else {
    	Saveas1Click(Sender);
    }
}
//---------------------------------------------------------------------------
void TfmMain::LoadProject (const UnicodeString &file)
{
	if (!FileExists(file))
		return;

	xml->Active = false;
	xml->XML->Clear();
	xml->FileName = file;
	xml->Active = true;

	IXMLNode *all = xml->ChildNodes->FindNode("SqDbgrProject");
	if (!all)
		return;

	Path.FileProject = file;
	lists.clear();

	IXMLNode *child = all->ChildNodes->FindNode("Scripts");
	if (child)
		Path.Scripts = Path.RestorePath(child->GetText(),ExtractFilePath(Path.FileProject));

	if (!Path.Scripts.Length())
		Path.Scripts = ExtractFilePath(Path.FileProject);

	dir->Text = Path.Scripts;

	child = all->ChildNodes->FindNode("Breakpoints");
	if (child) for (IXMLNode *fl = child->ChildNodes->First(); fl; fl=fl->NextSibling()) {
		UnicodeString file = fl->GetAttribute("name");
		file = Path.RestorePath(file);

		list.clear();
		for (IXMLNode *br = fl->ChildNodes->First(); br; br=br->NextSibling()) {
			list.push_back(br->GetAttribute("idx"));
		}

		lists[file] = list;
	}

	btRefreshDirClick(NULL);
}
//---------------------------------------------------------------------------
void TfmMain::SaveProject (const UnicodeString &FileName)
{
	xml->Active = false;
	xml->XML->Clear();
	xml->FileName = "";
	xml->Active = true;

	Path.FileProject = FileName;
	IXMLNode *all = xml->AddChild("SqDbgrProject");

	IXMLNode *child = all->AddChild("Scripts");
	child->SetText(Path.RelativePath(Path.Scripts,ExtractFilePath(Path.FileProject)));

	if (file.Length() && list.size())
		lists[file] = list;

	child = all->AddChild("Breakpoints");
	for (break_list::iterator it = lists.begin(); it != lists.end(); it++) {
		if (!it->second.size())
			continue;

		IXMLNode *fl = child->AddChild("File");
		fl->SetAttribute("name", Path.RelativePath(it->first));

		for (size_t i=0; i<it->second.size(); i++) {
			IXMLNode *br = fl->AddChild("Line");
			br->SetAttribute("idx", it->second[i]);
		}
	}

	xml->SaveToFile(FileName);
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::Exit1Click(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------


