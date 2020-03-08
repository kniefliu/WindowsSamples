// DuilibTest.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DuilibTest.h"
#include "MainFrameWnd.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (GlobalSet::ForegroundPrevInstance())
		return FALSE;

	if(FAILED(::OleInitialize(NULL)))
		return FALSE;

	if (!GlobalSet::InitGlobal())
		return FALSE;


	{
		DuiLib::CPaintManagerUI::SetInstance(hInstance);
		DuiLib::CDuiString strDuiResPath = DuiLib::CPaintManagerUI::GetInstancePath();
#ifdef DUI_RES_PATH
		char szAnisDuiResPath[MAX_PATH] = {0};
		strcpy_s(szAnisDuiResPath, DUI_RES_PATH);
		TCHAR szDuiResPath[MAX_PATH] = {0};
		MultiByteToWideChar(CP_ACP, 0, szAnisDuiResPath, strlen(szAnisDuiResPath), szDuiResPath, MAX_PATH-1);
		strDuiResPath = szDuiResPath;
#endif

		DuiLib::CPaintManagerUI::SetResourcePath(strDuiResPath + _T("skins\\skin1"));
		//CPaintManagerUI::SetResourceZip(CPaintManagerUI::GetInstancePath() + _T("skin\\skin1.zip"));

		CMainFrameWnd mainFrame;
		mainFrame.CreateX(NULL, UI_WNDSTYLE_FRAME, _T("mainframe.xml"));
		mainFrame.CenterWindow();
		::ShowWindow(mainFrame, SW_SHOW);

		DuiLib::CPaintManagerUI::MessageLoop();
	}


	GlobalSet::UninitGlobal();
	::OleUninitialize();

	return 0;
}

