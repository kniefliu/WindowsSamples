// RichListDemo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "RichListWnd.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	DuiLib::CDuiString strDuiResPath = CPaintManagerUI::GetInstancePath();
#ifdef DUI_RES_PATH
	char szAnisDuiResPath[MAX_PATH] = {0};
	strcpy_s(szAnisDuiResPath, DUI_RES_PATH);
	TCHAR szDuiResPath[MAX_PATH] = {0};
	MultiByteToWideChar(CP_ACP, 0, szAnisDuiResPath, strlen(szAnisDuiResPath), szDuiResPath, MAX_PATH-1);
	strDuiResPath = szDuiResPath;
#endif
	CPaintManagerUI::SetResourcePath(strDuiResPath + _T("skin"));
	CPaintManagerUI::SetResourceZip(_T("RichListRes.zip"));

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	CRichListWnd* pFrame = new CRichListWnd();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("DuiLib Demo from mbstudio.cn / meineson"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 500, 400);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	//::MessageBox(NULL, _T("alert!"), _T("Duilib Demo"), MB_OK);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}
