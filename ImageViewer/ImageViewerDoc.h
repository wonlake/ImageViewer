
// ImageViewerDoc.h : CImageViewerDoc ��Ľӿ�
//


#pragma once

#include <memory>

class CImageViewerDoc : public CDocument
{
protected: // �������л�����
	CImageViewerDoc();
	DECLARE_DYNCREATE(CImageViewerDoc)

// ����
public: 
	std::shared_ptr<BYTE[]> m_pImageData;

	int m_dwImageWidth = 0;
	int m_dwImageHeight = 0;
	int m_dwImageBpp = 24;

	COLORREF m_bkColor = 0x8F8F8F; 

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CImageViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL LoadImageFromMemory(BYTE* pData, LONG iDataSize);
	BOOL LoadImageFromFile(const wchar_t* pszFileName);
	std::shared_ptr<BYTE[]> CImageViewerDoc::SaveImageToFile(const wchar_t* lpFileName, const wchar_t* lpDstFileName, int& len);
protected:
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
//	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};
