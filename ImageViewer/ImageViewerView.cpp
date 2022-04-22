
// ImageViewerView.cpp : CImageViewerView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "ImageViewer.h"
#endif

#include "ImageViewerDoc.h"
#include "ImageViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageViewerView

IMPLEMENT_DYNCREATE(CImageViewerView, CView)

BEGIN_MESSAGE_MAP(CImageViewerView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_DROPFILES()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_NEW, &CImageViewerView::OnFileNew)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_MOUSELEAVE()
	ON_WM_NCMOUSELEAVE()
END_MESSAGE_MAP()

// CImageViewerView 构造/析构

CImageViewerView::CImageViewerView()
{
	// TODO: 在此处添加构造代码
	m_nWindowWidth = 0;
	m_nWindowHeight = 0;

	m_iXScrollMinPos = 0;
	m_iXScrollPos = 0;
	m_iXScrollMaxPos = 0;

	m_iYScrollMinPos = 0;
	m_iYScrollPos = 0;
	m_iYScrollMaxPos = 0;

	m_nLBlank = 0;
	m_nTBlank = 0;

	m_BitmapInfo = { 0 };
}

CImageViewerView::~CImageViewerView()
{
}

BOOL CImageViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CImageViewerView 绘制

void CImageViewerView::OnDraw(CDC* pDC)
{
	CImageViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	RECT rect;
	GetClientRect(&rect);
	DWORD dwWindowWidth = rect.right - rect.left;
	DWORD dwWindowHeight = rect.bottom - rect.top;

	if (m_pImageData == NULL)
	{
		pDC->Rectangle(rect.left, rect.top, rect.right, rect.bottom);
		pDC->SetTextAlign(TA_CENTER | TA_BASELINE);
		LPCTSTR pszText = _T("请将图片文件拖到这里!");

		pDC->TextOut(
			(rect.left + rect.right) / 2,
			(rect.top + rect.bottom) / 2,
			pszText,
			lstrlen(pszText));
		return;
	}

	SetDIBitsToDevice(pDC->GetSafeHdc(),
		m_nLBlank - m_iXScrollPos,
		m_nTBlank - m_iYScrollPos,
		pDoc->m_dwImageWidth,
		pDoc->m_dwImageHeight,
		0, 0,
		0, pDoc->m_dwImageHeight,
		m_pImageData,
		&m_BitmapInfo,
		DIB_RGB_COLORS);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap tempBmp;
	tempBmp.CreateCompatibleBitmap(pDC, dwWindowWidth, dwWindowHeight);

	memDC.SelectObject(tempBmp);
	if (m_nLBlank > 0 && m_nTBlank > 0)
		memDC.Rectangle(rect.left, rect.top, rect.right, rect.bottom);
	else
		memDC.FillRect(&rect, NULL);

	memDC.BitBlt(m_nLBlank, m_nTBlank,
		dwWindowWidth - m_nLBlank * 2 - m_nLBlank % 2,
		dwWindowHeight - m_nTBlank * 2 - m_nTBlank % 2, pDC,
		m_nLBlank, m_nTBlank, SRCCOPY);

	pDC->BitBlt(0, 0, dwWindowWidth, dwWindowHeight, &memDC,
		0, 0, SRCCOPY);

	tempBmp.DeleteObject();
	memDC.DeleteDC();
}


// CImageViewerView 打印

BOOL CImageViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CImageViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CImageViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CImageViewerView 诊断

#ifdef _DEBUG
void CImageViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CImageViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageViewerDoc* CImageViewerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageViewerDoc)));
	return (CImageViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageViewerView 消息处理程序

int CImageViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	::DragAcceptFiles(m_hWnd, TRUE);
    
	SCROLLINFO hScrollInfo = { 0 };
	hScrollInfo.cbSize = sizeof(hScrollInfo);
	hScrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	hScrollInfo.nMin = 0;
	hScrollInfo.nMax = 100;
	hScrollInfo.nPage = 100;
	hScrollInfo.nPos = 0;

	::SetScrollInfo(m_hWnd, SB_HORZ, &hScrollInfo, FALSE);
	::ShowScrollBar(m_hWnd, SB_HORZ, TRUE);

	SCROLLINFO vScrollInfo = { 0 };
	vScrollInfo.cbSize = sizeof(vScrollInfo);
	vScrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	vScrollInfo.nMin = 0;
	vScrollInfo.nMax = 100;
	vScrollInfo.nPage = 100;
	vScrollInfo.nPos = 0;

	::SetScrollInfo(m_hWnd, SB_VERT, &vScrollInfo, FALSE);
	::ShowScrollBar(m_hWnd, SB_VERT, TRUE);

	::EnableScrollBar(m_hWnd, SB_HORZ, ESB_DISABLE_BOTH);
	::EnableScrollBar(m_hWnd, SB_VERT, ESB_DISABLE_BOTH);

	return 0;
}

void CImageViewerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int iXDelta = 0;
	int iYDelta = 0;
	int iXNewPos = 0;
	int iWidth = 0;
	int iHeight = 0;

	RECT rtClient;
	::GetClientRect(m_hWnd, &rtClient);

	iWidth = rtClient.right - rtClient.left;
	iHeight = rtClient.bottom - rtClient.top;

	switch (nSBCode)
	{
	case SB_PAGEUP:
		iXNewPos = m_iXScrollPos - 50;
		break;

	case SB_PAGEDOWN:
		iXNewPos = m_iXScrollPos + 50;
		break;

	case SB_LINEUP:
		iXNewPos = m_iXScrollPos - 5;
		break;

	case SB_LINEDOWN:
		iXNewPos = m_iXScrollPos + 5;
		break;

	case SB_THUMBTRACK:
		iXNewPos = nPos;
		break;

	default:
		iXNewPos = m_iXScrollPos;
		break;
	}

	if (0 > iXNewPos)
		iXNewPos = 0;

	if (m_iXScrollMaxPos - iWidth < iXNewPos)
		iXNewPos = m_iXScrollMaxPos - iWidth;

	if (iXNewPos == m_iXScrollPos)
		return;

	iXDelta = iXNewPos - m_iXScrollPos;
	m_iXScrollPos = iXNewPos;

	SCROLLINFO scrollInfo = { 0 };
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = m_iXScrollPos;

	::SetScrollInfo(m_hWnd, SB_HORZ, &scrollInfo, TRUE);
	::InvalidateRect(m_hWnd, NULL, FALSE);

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CImageViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{ 
	CImageViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return TRUE;

	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int   iXDelta = 0;
	int   iYDelta = 0;
	int   iYNewPos = 0;
	DWORD iWidth = 0;
	DWORD iHeight = 0;

	RECT  rtClient;
	::GetClientRect(m_hWnd, &rtClient);

	iWidth = rtClient.right - rtClient.left;
	iHeight = rtClient.bottom - rtClient.top;

	iYNewPos = m_iYScrollPos - zDelta;

	if (0 > iYNewPos)
		iYNewPos = 0;

	if (m_iYScrollMaxPos - (int)iHeight < iYNewPos)
		iYNewPos = m_iYScrollMaxPos - (int)iHeight;

	if (iYNewPos == m_iYScrollPos || iHeight >= pDoc->m_dwImageHeight)
		return 0;

	iYDelta = iYNewPos - m_iYScrollPos;
	m_iYScrollPos = iYNewPos;

	SCROLLINFO scrollInfo = { 0 };
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = m_iYScrollPos;

	::SetScrollInfo(m_hWnd, SB_VERT, &scrollInfo, TRUE);
	::InvalidateRect(m_hWnd, NULL, FALSE);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CImageViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码

	m_iXScrollPos = 0;
	m_iYScrollPos = 0;
	m_nWindowWidth = cx;
	m_nWindowHeight = cy;

	UpdateScrollBarInfo();
}

void CImageViewerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int iXDelta = 0;
	int iYDelta = 0;
	int iWidth = 0;
	int iHeight = 0;
	int iYNewPos = 0;

	RECT rtClient;
	::GetClientRect(m_hWnd, &rtClient);

	iWidth = rtClient.right - rtClient.left;
	iHeight = rtClient.bottom - rtClient.top;

	switch (nSBCode)
	{
	case SB_PAGEUP:
		iYNewPos = m_iYScrollPos - 50;
		break;

	case SB_PAGEDOWN:
		iYNewPos = m_iYScrollPos + 50;
		break;

	case SB_LINEUP:
		iYNewPos = m_iYScrollPos - 5;
		break;

	case SB_LINEDOWN:
		iYNewPos = m_iYScrollPos + 5;
		break;

	case SB_THUMBTRACK:
		iYNewPos = nPos;
		break;

	default:
		iYNewPos = m_iYScrollPos;
		break;
	}

	if (iYNewPos < 0)
		iYNewPos = 0;

	if (m_iYScrollMaxPos - iHeight < iYNewPos)
		iYNewPos = m_iYScrollMaxPos - iHeight;

	if (iYNewPos == m_iYScrollPos)
		return;

	iYDelta = iYNewPos - m_iYScrollPos;
	m_iYScrollPos = iYNewPos;

	SCROLLINFO scrollInfo = { 0 };
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = m_iYScrollPos;

	::SetScrollInfo(m_hWnd, SB_VERT, &scrollInfo, TRUE);
	::InvalidateRect(m_hWnd, NULL, FALSE);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CImageViewerView::OnDropFiles(HDROP hDropInfo)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CString strFile;
	UINT nFilesCount = DragQueryFile(hDropInfo, INFINITE, NULL, 0);
	for (UINT i = 0; i < nFilesCount; i++)
	{
		auto pathLen = DragQueryFile(hDropInfo, i, strFile.GetBuffer(MAX_PATH), MAX_PATH);
		strFile.ReleaseBuffer(pathLen);
	}

	auto pApp = (CImageViewerApp*)AfxGetApp(); 
	pApp->OpenDocumentFile(strFile);

	::SetFocus(m_hWnd);
}

VOID CImageViewerView::UpdateScrollBarInfo()
{
	CImageViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (m_pImageData)
	{
		RECT rt;
		::GetClientRect(m_hWnd, &rt);
		int iWidth = rt.right - rt.left;
		int iHeight = rt.bottom - rt.top;

		if (pDoc->m_dwImageWidth <= m_nWindowWidth)
		{
			::EnableScrollBar(m_hWnd, SB_HORZ, ESB_DISABLE_BOTH);

			m_nLBlank = (m_nWindowWidth - pDoc->m_dwImageWidth) / 2;
			m_iXScrollPos = 0;
		}
		else
		{
			m_iXScrollMaxPos = pDoc->m_dwImageWidth > m_nWindowWidth ? pDoc->m_dwImageWidth : 0;
			m_iXScrollPos = m_iXScrollPos < m_iXScrollMaxPos - (int)m_nWindowWidth ? m_iXScrollPos : m_iXScrollMaxPos - (int)m_nWindowWidth;
			SCROLLINFO hScrollInfo = { 0 };
			hScrollInfo.cbSize = sizeof(hScrollInfo);
			hScrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
			hScrollInfo.nMin = m_iXScrollMinPos;
			hScrollInfo.nMax = m_iXScrollMaxPos;
			hScrollInfo.nPage = m_nWindowWidth;
			hScrollInfo.nPos = m_iXScrollPos;

			::SetScrollInfo(m_hWnd, SB_HORZ, &hScrollInfo, TRUE);
			::ShowScrollBar(m_hWnd, SB_HORZ, TRUE);

			::EnableScrollBar(m_hWnd, SB_HORZ, ESB_ENABLE_BOTH);

			m_nLBlank = 0;
		}
		if (pDoc->m_dwImageHeight <= m_nWindowHeight)
		{
			::EnableScrollBar(m_hWnd, SB_VERT, ESB_DISABLE_BOTH);
			m_nTBlank = (m_nWindowHeight - pDoc->m_dwImageHeight) / 2;
			m_iYScrollPos = 0;
		}
		else
		{
			m_iYScrollMaxPos = pDoc->m_dwImageHeight > m_nWindowHeight ? pDoc->m_dwImageHeight : 0;
			m_iYScrollPos = m_iYScrollPos < m_iYScrollMaxPos - (int)m_nWindowHeight ? m_iYScrollPos : m_iYScrollMaxPos - (int)m_nWindowHeight;
			SCROLLINFO vScrollInfo = { 0 };
			vScrollInfo.cbSize = sizeof(vScrollInfo);
			vScrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
			vScrollInfo.nMin = m_iYScrollMinPos;
			vScrollInfo.nMax = m_iYScrollMaxPos;
			vScrollInfo.nPage = m_nWindowHeight;
			vScrollInfo.nPos = m_iYScrollPos;

			::SetScrollInfo(m_hWnd, SB_VERT, &vScrollInfo, TRUE);
			::ShowScrollBar(m_hWnd, SB_VERT, TRUE);

			::EnableScrollBar(m_hWnd, SB_VERT, ESB_ENABLE_BOTH);

			m_nTBlank = 0;
		}
	}

	::InvalidateRect(m_hWnd, NULL, TRUE);
}

BOOL CImageViewerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	return CView::OnEraseBkgnd(pDC);
}


void CImageViewerView::OnFileNew()
{
	// TODO: Add your command handler code here
}

void CImageViewerView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
	auto pDoc = GetDocument();


	if (pDoc->GetPathName().IsEmpty())
	{ 
		m_pImageData = NULL;
		UpdateScrollBarInfo();
		return;
	}

	if (pDoc->GetPathName() != m_FileName)
	{
		m_FileName = pDoc->GetPathName();
		m_pImageData = pDoc->m_pImageData.get();

		BITMAPINFOHEADER& bih = m_BitmapInfo.bmiHeader;
		ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));

		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = pDoc->m_dwImageWidth;
		bih.biHeight = pDoc->m_dwImageHeight;
		bih.biCompression = BI_RGB;
		bih.biPlanes = 1;
		bih.biBitCount = pDoc->m_dwImageBpp;

		m_iXScrollMinPos = 0;
		m_iXScrollPos = 0;
		m_iXScrollMaxPos = 0;

		m_iYScrollMinPos = 0;
		m_iYScrollPos = 0;
		m_iYScrollMaxPos = 0;

		m_nLBlank = 0;
		m_nTBlank = 0;

		UpdateScrollBarInfo();
	}
}


void CImageViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	CImageViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: Add your message handler code here and/or call default

	if (nFlags != MK_LBUTTON)
		return;

	if (!m_UseHand)
	{
		auto pApp = (CImageViewerApp*)AfxGetApp();
		m_UseHand = true;
		SetCursor(pApp->m_hCursorHand);
	}

	int   iXDelta = 0;
	int   iYDelta = 0;
	int   iYNewPos = 0;
	int   iXNewPos = 0;
	DWORD iWidth = 0;
	DWORD iHeight = 0;

	RECT  rtClient;
	::GetClientRect(m_hWnd, &rtClient);

	iWidth = rtClient.right - rtClient.left;
	iHeight = rtClient.bottom - rtClient.top;

	iYNewPos = m_iYScrollPos - (point.y - m_lastMousePos.y);
	iXNewPos = m_iXScrollPos - (point.x - m_lastMousePos.x);

	m_lastMousePos = point;

	if (m_iYScrollMaxPos - (int)iHeight < iYNewPos)
		iYNewPos = m_iYScrollMaxPos - (int)iHeight;

	if (m_iXScrollMaxPos - (int)iWidth < iXNewPos)
		iXNewPos = m_iXScrollMaxPos - (int)iWidth;

	if (0 > iYNewPos)
		iYNewPos = 0;

	if (0 > iXNewPos)
		iXNewPos = 0;

	bool xMove = true;
	bool yMove = true;

	if (iYNewPos == m_iYScrollPos || iHeight >= pDoc->m_dwImageHeight)
	{
		yMove = false;
	}
	if (iXNewPos == m_iXScrollPos || iWidth >= pDoc->m_dwImageWidth)
		xMove = false;

	if (!xMove && !yMove)
		return;

	iYDelta = iYNewPos - m_iYScrollPos;
	iXDelta = iXNewPos - m_iXScrollPos;
	m_iYScrollPos = iYNewPos;
	m_iXScrollPos = iXNewPos;

	if (yMove)
	{
		SCROLLINFO vScrollInfo = { 0 };
		vScrollInfo.cbSize = sizeof(vScrollInfo);
		vScrollInfo.fMask = SIF_POS;
		vScrollInfo.nPos = m_iYScrollPos;
		::SetScrollInfo(m_hWnd, SB_VERT, &vScrollInfo, TRUE);
	}

	if (xMove)
	{
		SCROLLINFO hScrollInfo = { 0 };
		hScrollInfo.cbSize = sizeof(hScrollInfo);
		hScrollInfo.fMask = SIF_POS;
		hScrollInfo.nPos = m_iXScrollPos;
		::SetScrollInfo(m_hWnd, SB_HORZ, &hScrollInfo, TRUE);
	}
	::InvalidateRect(m_hWnd, NULL, FALSE);

	CView::OnMouseMove(nFlags, point);
}


void CImageViewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_lastMousePos = point;
	CView::OnLButtonDown(nFlags, point);
	auto pApp = (CImageViewerApp*)AfxGetApp();
	SetCursor(pApp->m_hCursorHand); 
	m_UseHand = true;
}


void CImageViewerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnLButtonUp(nFlags, point);
	auto pApp = (CImageViewerApp*)AfxGetApp();
	m_UseHand = false;
	SetCursor(pApp->m_hCursorNormal);
}


BOOL CImageViewerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	if (m_UseHand)
	{
		auto pApp = (CImageViewerApp*)AfxGetApp();
		SetCursor(pApp->m_hCursorHand);
		return TRUE;
	}
	return CView::OnSetCursor(pWnd, nHitTest, message);
}


void CImageViewerView::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default

	auto pApp = (CImageViewerApp*)AfxGetApp();
	m_UseHand = false;
	SetCursor(pApp->m_hCursorNormal);

	CView::OnMouseLeave();
}


void CImageViewerView::OnNcMouseLeave()
{
	// This feature requires Windows 2000 or greater.
	// The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
	// TODO: Add your message handler code here and/or call default

	auto pApp = (CImageViewerApp*)AfxGetApp();
	m_UseHand = false;
	SetCursor(pApp->m_hCursorNormal);

	CView::OnNcMouseLeave();
}
