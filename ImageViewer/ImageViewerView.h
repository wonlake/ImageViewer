
// ImageViewerView.h : CImageViewerView ��Ľӿ�
//

#pragma once

class CImageViewerDoc;
class CImageViewerView : public CView
{
private:
	BYTE*		m_pImageData;
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
	SCROLLINFO	m_ScrollInfo;

	COLORREF	m_bkColor;

protected: // �������л�����
	CImageViewerView();
	DECLARE_DYNCREATE(CImageViewerView)

// ����
public:
	CImageViewerDoc* GetDocument() const;

// ����
public:
	BOOL LoadImageFromMemory(BYTE* pData, LONG iDataSize);
	BOOL LoadImageFromFile(TCHAR* pszFileName);
	VOID UpdateScrollBarInfo();

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CImageViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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
};

#ifndef _DEBUG  // ImageViewerView.cpp �еĵ��԰汾
inline CImageViewerDoc* CImageViewerView::GetDocument() const
   { return reinterpret_cast<CImageViewerDoc*>(m_pDocument); }
#endif
