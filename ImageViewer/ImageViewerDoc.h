
// ImageViewerDoc.h : CImageViewerDoc 类的接口
//


#pragma once

#include <memory>

class CImageViewerDoc : public CDocument
{
protected: // 仅从序列化创建
	CImageViewerDoc();
	DECLARE_DYNCREATE(CImageViewerDoc)

// 特性
public: 
	std::shared_ptr<BYTE[]> m_pImageData;

	int m_dwImageWidth = 0;
	int m_dwImageHeight = 0;
	int m_dwImageBpp = 24;

	COLORREF m_bkColor = 0x8F8F8F; 

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
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
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
//	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};
