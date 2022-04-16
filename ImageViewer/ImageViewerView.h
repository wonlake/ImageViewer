
// ImageViewerView.h : CImageViewerView 类的接口
//

#pragma once
#include <memory>

class CImageViewerDoc;
class CImageViewerView : public CView
{
private:
	CString m_FileName = TEXT("");
	std::shared_ptr<BYTE[]> m_pImageData;
	
	DWORD		m_dwImageWidth;
	DWORD		m_dwImageHeight;

	DWORD		m_nWindowWidth;
	DWORD		m_nWindowHeight;

	int			m_iXScrollMinPos;
	int			m_iXScrollPos;
	int			m_iXScrollMaxPos;

	int			m_iYScrollMinPos;
	int			m_iYScrollPos;
	int			m_iYScrollMaxPos;

	DWORD		m_nLBlank;
	DWORD		m_nTBlank;

	BITMAPINFO	m_BitmapInfo;

	COLORREF	m_bkColor;

	CPoint      m_lastMousePos;
	bool		m_UseHand = false;

protected: // 仅从序列化创建
	CImageViewerView();
	DECLARE_DYNCREATE(CImageViewerView)

// 特性
public:
	CImageViewerDoc* GetDocument() const;

// 操作
public:
	BOOL LoadImageFromMemory(BYTE* pData, LONG iDataSize);
	BOOL LoadImageFromFile(const TCHAR* pszFileName);
	VOID UpdateScrollBarInfo();

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CImageViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFileNew();
//	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
//	virtual void OnActivateFrame(UINT nState, CFrameWnd* pDeactivateFrame);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseLeave();
	afx_msg void OnNcMouseLeave();
};

#ifndef _DEBUG  // ImageViewerView.cpp 中的调试版本
inline CImageViewerDoc* CImageViewerView::GetDocument() const
   { return reinterpret_cast<CImageViewerDoc*>(m_pDocument); }
#endif

