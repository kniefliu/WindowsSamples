#include "stdafx.h"

#include <Windows.h>
#include "resource.h"
#include "EmotionButton.h"


CEmotionButton::CEmotionButton(void)
{
	m_CxImages=NULL;
	  //���ڴ�С
	QQFACEDLG_WIDTH = 446;
	QQFACEDLG_HEIGHT = 239;  

		//�������Ͻ�����
	CELLS_LEFT = 6;
	CELLS_TOP =3;
	CELLS_RIGHT = 441;	//(6  + 29*15)
	CELLS_BOTTOM = 259;	//(28 + 29*8)

	CELLSIZE = 29; //ÿ������= 29*30����ͼƬ�ߴ���24*24��

	CELLCOUNT_LINE = 15;	//ÿ��
	CELLCOUNT_COLUMN = 8;	//ÿ��
	CELLCOUNT_PAGE = 120; //ÿҳ120������


	rcLeft.left = CELLS_LEFT+1;
	rcLeft.top = CELLS_TOP+1;
	rcLeft.right = rcLeft.left + (CELLSIZE*3-1);
	rcLeft.bottom = rcLeft.top + (CELLSIZE*3-1);

	rcRight.left = CELLS_LEFT+(CELLSIZE*12)+1;
	rcRight.top = CELLS_TOP+1;
	rcRight.right = rcRight.left + (CELLSIZE*3-1);
	rcRight.bottom = rcRight.top + (CELLSIZE*3-1);
	//��ʼʱ���ض�������
	pvstatus = Hide;

	TCHAR facepath[MAX_PATH];
	GetFaceFolderPath(facepath, _T("Face"));

		

	//�������б��飡
	m_ImageCount = LoadImages(facepath);

	//����ҳ����
	m_PageCount = (m_ImageCount + CELLCOUNT_PAGE - 1) / CELLCOUNT_PAGE;

	curPage=0;



	//�����ڴ�DC
	HDC hdc = GetDC(NULL);
	m_hMemDC = CreateCompatibleDC(hdc);
	m_hMemDCBkGnd = CreateCompatibleDC(hdc);	
	m_hMemBitmap = CreateCompatibleBitmap(hdc, QQFACEDLG_WIDTH, QQFACEDLG_HEIGHT);
	m_hMemBitmapBkGnd = CreateCompatibleBitmap(hdc, QQFACEDLG_WIDTH, QQFACEDLG_HEIGHT);
	ReleaseDC(NULL, hdc);

	SelectObject(m_hMemDC, m_hMemBitmap);
	SelectObject(m_hMemDCBkGnd, m_hMemBitmapBkGnd);

	TCHAR szPageInfo[16];
		//����ҳ��Ϣ
	wsprintf(szPageInfo, _T("%d/%d"), curPage+1, m_PageCount);

	SwitchPage(curPage);
}

CEmotionButton::~CEmotionButton(void)
{
	FreeImages();
}


int CEmotionButton::GetPageCount()
{
    return m_PageCount;
}

//�ͷ�ͼƬ��Դ
void CEmotionButton::FreeImages()
{
	//if(m_Images != NULL)
	//{
	//	delete[] m_Images;
	//	m_Images = NULL;		
	//}

	if(m_CxImages != NULL)
	{
		delete[] m_CxImages;
		m_CxImages = NULL;
	}	
	m_ImageCount = 0;
}

//��ȡ�����ļ�������·����
void CEmotionButton::GetFaceFolderPath(TCHAR* path, TCHAR* folderName)
{
	TCHAR *pChar;
	TCHAR temp[MAX_PATH] = {0};
#ifdef DUI_RES_PATH
	char szAnisDuiResPath[MAX_PATH] = {0};
	strcpy_s(szAnisDuiResPath, DUI_RES_PATH);
	TCHAR szDuiResPath[MAX_PATH] = {0};
	MultiByteToWideChar(CP_ACP, 0, szAnisDuiResPath, strlen(szAnisDuiResPath), temp, MAX_PATH-1);
#else
	GetModuleFileName(NULL, temp, MAX_PATH);

	//���ҵ�����һ����б��λ��
	
	pChar=_tcsrchr(temp,'\\');
	if(pChar != NULL)
		*pChar = 0;//�ڷ�б�ܴ��ض��ַ���	
#endif
	wsprintf(path, _T("%s\\%s"), temp, folderName);
}

bool CEmotionButton::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{ 
	 
	::BitBlt(hDC, 0, 0, QQFACEDLG_WIDTH, QQFACEDLG_HEIGHT, m_hMemDC, 0, 0, SRCCOPY);
	
	//RECT rc = rcPaint;
	//CRenderEngine::DrawText(hDC, m_pManager, rc, _T("����һ���ؼ���"), RGB(255,0,0),  -1, DT_SINGLELINE );

	return true;
}

int CEmotionButton::GetCurrentPage()
{
    return curPage;
}

void CEmotionButton::SetPage(int page)
{
     curPage = page;

	 
	 SwitchPage(curPage);
}

void CEmotionButton::DoEvent(TEventUI& event)
{
     switch( event.Type)
	 {
		 case UIEVENT_MOUSEMOVE:
		 {
				RECT rc1, rc2, invalidRect;
				int lastCol = -1, curCol = -1;

				unsigned char changetype = NoChange; //Ԥ�������Ƿ���Ҫ�ı�

				POINT pt = event.ptMouse;
				int tmpSel = GetCellIndex(pt.x, pt.y);

				//�Ƿ񳬳���ͼƬ����
				if((tmpSel + curPage * CELLCOUNT_PAGE) >= m_ImageCount)
					tmpSel = -1;

				if(tmpSel != curSel)
				{
					//����cell �����н�
					lastSel = curSel;
					curSel = tmpSel;
					curFrame = 0;

					//Ԥ��������Ҫ�ƶ���
					lastCol = (lastSel % 15);
					curCol = (curSel % 15); //ÿ��15��

					GetBlueRect(lastSel, &rc1);
					GetBlueRect(curSel, &rc2);
					UnionRect(&invalidRect, &rc1, &rc2);
					invalidRect.right++;
					invalidRect.bottom++;

					//�ж����Ծ����Ƿ���λ�øı䣿
					if(pvstatus == Hide)
					{
						if(curSel >= 0)
						{
							if((curSel%15) > 7)
							{
								pvstatus = Left;
								changetype = HideToLeft;
							}
							else
							{
								pvstatus = Right;
								changetype = HideToRight;
							}
						}
					}
					else if(pvstatus == Left)
					{					
						if(curSel < 0)
						{
							pvstatus = Hide;
							changetype = LeftToHide;
						}
						else if(curCol < 4 || (curCol == 4 && lastCol == 4))
						{
							pvstatus = Right;
							changetype = LeftToRight;
						}
					}
					else if(pvstatus == Right)
					{
						if(curSel < 0)
						{
							pvstatus = Hide;
							changetype = RightToHide;
						}
						else if(curCol > 10 || (curCol == 10 && lastCol == 10))
						{
							pvstatus = Left;
							changetype = RightToLeft;
						}
					}

					//ˢ����ɫ���ο�
					UpdateSelectedFace(curPage, curSel, curFrame, pvstatus);
					this->Invalidate();
					 
					
					if(changetype == NoChange)
					{
						//ˢ��Ԥ��ͼƬ
						if(pvstatus == Left)
						{
							this->Invalidate();
							//InvalidateRect(hDlg, &rcLeft, FALSE);
						}
						else if(pvstatus == Right)
						{
							this->Invalidate();
							//InvalidateRect(hDlg, &rcRight, FALSE);
						}

					}
					else
					{
						CopyRect(&rc1, &rcLeft);
						CopyRect(&rc2, &rcRight);
						rc1.right++;
						rc1.bottom++;
						rc2.right++;
						rc2.bottom++; 

						//����ԭλ�õ�����ͼ
						switch(changetype & 0xf0)
						{
						case 0x00:
							break;
						case 0x10:
							this->Invalidate();
							//InvalidateRect(hDlg, &rc1, FALSE);
							break;
						case 0x20:
							this->Invalidate();
							//InvalidateRect(hDlg, &rc2, FALSE);
							break;
						}
						//��ʾ��λ���ϵ�����ͼ
						switch(changetype & 0x0f)
						{
						case 0x00:
							pvstatus = Hide;
							break;
						case 0x01:
							this->Invalidate();
							//InvalidateRect(hDlg, &rc1, FALSE);
							break;
						case 0x02:
							this->Invalidate();
							//InvalidateRect(hDlg, &rc2, FALSE);
							break;
						}
					}

					//��װ��ʱ������ʾ������
					if(curSel >= 0)
					{
						UINT frameDelay = 200;
						CxImage* pImg = GetSelectedImage(curPage, curSel);
						CxImage* pFrame = pImg->GetFrame(curFrame);

						//QQ�����֡��ʱͨ����10���룬��ʾ�ٶȹ��죬�������50����
						if(pFrame != NULL) frameDelay = max(frameDelay, pFrame->GetFrameDelay());

						frameCount = pImg->GetNumFrames();
 
						this->m_pManager->SetTimer(this, TIMER_SHOWGIF, frameDelay);
					}
					else
					{ 
						this->m_pManager->KillTimer(this, TIMER_SHOWGIF);
					}
				}
				
				//ʹϵͳ֪ͨ���� WM_MOUSELEAVE ;
				TrackMouseEvent(&m_tme); //ע��汾����_WIN32_WINNT 0x0510
		 }
		 break;
		 case UIEVENT_MOUSELEAVE:
		 {
				if(curSel >= 0)
				{
					RECT rc;
					GetBlueRect(curSel, &rc);
					lastSel = curSel;
					curSel = -1;
					curFrame = 0;
					pvstatus = Hide;
					UpdateSelectedFace(curPage, curSel, curFrame, pvstatus);
					this->Invalidate();
					//InvalidateRect(hDlg, &rc, FALSE);
				}
		 }
		 break;
		 case UIEVENT_TIMER:
		 {
			  this->m_pManager->KillTimer(this, TIMER_SHOWGIF);

			if(curSel >= 0 && frameCount > 1)
			{
				CxImage *pImg = GetSelectedImage(curPage, curSel);
				CxImage *pFrame = NULL;
				UINT frameDelay = 200;

				LPRECT lpRect = (pvstatus == Left)? &rcLeft:&rcRight;
				
				//�ƶ�����һ֡��
				curFrame = (curFrame + 1) % frameCount;

				//QQ�����֡��ʱͨ����10���룬��ʾ�ٶȹ��죬�������100����
				pFrame = pImg->GetFrame(curFrame);
				
				if(pFrame) frameDelay = max(frameDelay, pFrame->GetFrameDelay());
 
				UpdateSelectedFace(curPage, curSel, curFrame, pvstatus);
				this->Invalidate();
				//InvalidateRect(hDlg, lpRect, FALSE);

				//��һ֡�Ķ�ʱʱ��
				this->m_pManager->SetTimer(this, TIMER_SHOWGIF, frameDelay);
			} 
		 }
		 break;
	 }
}

void CEmotionButton::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("timer")) == 0)
	{
	}
}

//�л���ǰҳʱ��Ҫ���еĸ���
void CEmotionButton::SwitchPage(int curPage)
{
	if(NULL == m_CxImages)
		return ;
	int index, count, left, top, width, height;
	RECT rc; 

    this->Invalidate();	
	//�Ȼ�����
	TCHAR szBKImg[MAX_PATH] = {0};
	_sntprintf(szBKImg, MAX_PATH - 1, _T("file='bg.png' corner='4,4,2,2' fade='100'"));
	  
    //CRenderEngine::DrawImageString(m_hMemDCBkGnd, m_pManager, m_rcItem, m_rcItem, szBKImg, NULL); 

	HINSTANCE hInst =m_pManager->GetInstance();
	HBITMAP hBlankBkGnd = ::LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BKGND));
	HDC hTempDC = CreateCompatibleDC(m_hMemDCBkGnd);
	SelectObject(hTempDC, hBlankBkGnd);
	
	

	BitBlt(m_hMemDCBkGnd, 0, 0, QQFACEDLG_WIDTH, QQFACEDLG_HEIGHT, hTempDC, 0, 0, SRCCOPY);
	
	DeleteDC(hTempDC);
	DeleteObject(hBlankBkGnd);
	 
	//���Ƶ�ǰҳ������б���
	//���Ʊ�ҳ������ͼƬ
	for(index = (curPage * CELLCOUNT_PAGE), count = 0; (index< m_ImageCount && count < CELLCOUNT_PAGE); index++, count++)
	{
		//���л���
		GetBlueRect(count, &rc);

		left = max(rc.left, (rc.left + rc.right - m_CxImages[index].GetWidth())/2);
		top = max(rc.top, (rc.top + rc.bottom - m_CxImages[index].GetHeight())/2);
		width = min(m_CxImages[index].GetWidth(), CELLSIZE);
		height = min(m_CxImages[index].GetHeight(), CELLSIZE);

		m_CxImages[index].GetFrame(0)->Draw(m_hMemDCBkGnd, left, top, width, height);
		//m_CxImages[index].Draw(m_hMemDCBkGnd, left, top, width, height);
	}

	//���͵��ڴ�λͼ
	
	BitBlt(m_hMemDC, 0, 0, QQFACEDLG_WIDTH, QQFACEDLG_HEIGHT, m_hMemDCBkGnd, 0, 0, SRCCOPY);
	
 
}

//����ڴ������ƶ�ʱ��Ҫ���еĸ���
void CEmotionButton::UpdateSelectedFace(int curPage, int curSel, int curFrame, int pvstatus)
{
	HGDIOBJ hOldBrush = NULL, hOldPen = NULL, hTempPen, hTempBrush;
	HPEN hPen1 = NULL;
	HPEN hPen2 = NULL;
	LPRECT lpRect = NULL;
	RECT rc;
	int index;

	//������
	BitBlt(m_hMemDC, 0, 0, QQFACEDLG_WIDTH, QQFACEDLG_HEIGHT, m_hMemDCBkGnd, 0, 0, SRCCOPY);

	//������ɫѡ�п�
	if(curSel >= 0)
	{
		hPen1 = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		hTempPen = SelectObject(m_hMemDC, hPen1);
		hTempBrush = SelectObject(m_hMemDC, GetStockObject(NULL_BRUSH));
		GetBlueRect(curSel, &rc);		
		Rectangle(m_hMemDC, rc.left, rc.top, rc.right, rc.bottom);

		if(hOldPen == NULL) hOldPen = hTempPen;
		if(hOldBrush == NULL) hOldBrush = hTempBrush;
	}

	//�����������Ҳ��Ԥ��ͼ
	if(pvstatus == Left) lpRect = &rcLeft;
	else if(pvstatus == Right) lpRect = &rcRight;

	if(lpRect != NULL)
	{
		index = curPage * CELLCOUNT_PAGE + curSel; //��ҳ������������������������		
		hPen2 = CreatePen(PS_SOLID, 1, RGB(0, 138, 255)); //����ɫ����		
		hTempPen = SelectObject(m_hMemDC, hPen2);
		hTempBrush = SelectObject(m_hMemDC, GetStockObject(WHITE_BRUSH)); //��ɫ��ˢ
		Rectangle(m_hMemDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
		//���л���
		int left = (lpRect->left + lpRect->right - m_CxImages[index].GetWidth())/2;
		int top = (lpRect->top + lpRect->bottom - m_CxImages[index].GetHeight())/2;

		//����֡
		if(curFrame < m_CxImages[index].GetNumFrames())
		{
			CxImage* pFrame = m_CxImages[index].GetFrame(curFrame);
			if(pFrame) pFrame->Draw(m_hMemDC, left, top);
			//m_CxImages[index].Draw(m_hMemDC, left, top);
		}
		if(hOldBrush == NULL) hOldBrush = hTempBrush;
		if(hOldPen == NULL) hOldPen = hTempPen;
	}
	
	//�ָ���ˢ������
	if(hOldBrush != NULL) SelectObject(m_hMemDC, hOldBrush);
	if(hOldPen != NULL)	SelectObject(m_hMemDC, hOldPen);

	if(hPen1 != NULL) DeleteObject(hPen1);
	if(hPen2 != NULL) DeleteObject(hPen2);
}



//�ṩ�ͻ������꣬��ȡ��ǰ����·�����������
int  CEmotionButton::GetCellIndex(int x, int y)
{
	//���Ƿ�������Χ�ڣ�

	//ע��x >= CELLS_RIGHT (��������)
	if(x < CELLS_LEFT || x >= CELLS_RIGHT || y < CELLS_TOP || y >= CELLS_BOTTOM)
		return -1;

	//ÿ�� 15  �� CELL

	return (y - CELLS_TOP) / CELLSIZE * 15 + (x - CELLS_LEFT) / CELLSIZE;
}

//��ȡĳ��Cell����ɫ���ο�ѡ��ʱ���ƣ�
void  CEmotionButton::GetBlueRect(int cellIndex, LPRECT pRect)
{
	pRect->left = CELLS_LEFT + CELLSIZE * (cellIndex % 15) + 1;
	pRect->top = CELLS_TOP + CELLSIZE * (cellIndex / 15) + 1;
	pRect->right = pRect->left + (CELLSIZE - 2);
	pRect->bottom = pRect->top + (CELLSIZE - 1);
}



//��ָ�����ļ��м���ͼƬ
int CEmotionButton::LoadImages(LPTSTR folder)
{
	TCHAR filter[MAX_PATH], filename[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	int index = 0, count = 0;

	//������ȡ���ļ���Ŀ
	wsprintf(filter, _T("%s\\*.gif"), folder);
	hFind = FindFirstFile(filter, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return 0;
	} 
	else 
	{
		count++;
		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			count++;
		}
	}
	FindClose(hFind);

	if(count>0)
	{
		    m_CxImages = new CxImage[count];

			hFind = FindFirstFile(filter, &FindFileData);
			wsprintf(filename, _T("%s\\%s"), folder, FindFileData.cFileName);
			//m_Images[index++].Load(filename);
			m_CxImages[index].SetRetreiveAllFrames(true);
			m_CxImages[index++].Load(filename, CXIMAGE_FORMAT_UNKNOWN);


			while (FindNextFile(hFind, &FindFileData) != 0) 
			{
				wsprintf(filename, _T("%s\\%s"), folder, FindFileData.cFileName);
				//m_Images[index++].Load(filename);
				m_CxImages[index].SetRetreiveAllFrames(true);
				m_CxImages[index++].Load(filename, CXIMAGE_FORMAT_UNKNOWN);
			}
			FindClose(hFind);
	}
	
	return count;
}


CxImage *CEmotionButton::GetSelectedImage(int curPage, int curSel)
{
	return m_CxImages + (curPage * CELLCOUNT_PAGE + curSel);
} 