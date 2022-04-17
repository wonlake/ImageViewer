
// ImageViewerDoc.cpp : CImageViewerDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "ImageViewer.h"
#endif

#include "ImageViewerDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImageViewerDoc

IMPLEMENT_DYNCREATE(CImageViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageViewerDoc, CDocument)
END_MESSAGE_MAP()


// CImageViewerDoc 构造/析构

CImageViewerDoc::CImageViewerDoc()
{
	// TODO: 在此添加一次性构造代码

}

CImageViewerDoc::~CImageViewerDoc()
{
}

BOOL CImageViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CImageViewerDoc 序列化

void CImageViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
		int len = 0;
		auto data = SaveImageToFile(m_strPathName, ar.m_strFileName, len);
		if (data != nullptr)
			ar.Write(data.get(), len); 
	}
	else
	{
		// TODO: 在此添加加载代码
		LoadImageFromFile(ar.m_strFileName);
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CImageViewerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CImageViewerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CImageViewerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImageViewerDoc 诊断

#ifdef _DEBUG
void CImageViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageViewerDoc 命令

BOOL CImageViewerDoc::LoadImageFromMemory(BYTE* pData, LONG iDataSize)
{
	// TODO: 在此添加实现代码
	FIBITMAP* pFiBitmap = NULL;
	FIMULTIBITMAP* pFiMultiBitmap = NULL;
	DWORD             dwIndex = 0;
	BOOL              bMultiBitmap = FALSE;

	FIMEMORY* fi_mem = FreeImage_OpenMemory(pData, iDataSize);
	FREE_IMAGE_FORMAT FiFormat = FreeImage_GetFileTypeFromMemory(fi_mem, iDataSize);

	if (FiFormat == FIF_UNKNOWN)
	{
		FreeImage_CloseMemory(fi_mem);
		return FALSE;
	}

	if (FiFormat == FIF_JPEG)
		pFiBitmap = FreeImage_LoadFromMemory(FiFormat, fi_mem, JPEG_CMYK);
	else if (FiFormat == FIF_GIF)
		pFiBitmap = FreeImage_LoadFromMemory(FiFormat, fi_mem, GIF_PLAYBACK);
	else if (FiFormat == FIF_ICO)
		pFiBitmap = FreeImage_LoadFromMemory(FiFormat, fi_mem, ICO_MAKEALPHA);
	else
		pFiBitmap = FreeImage_LoadFromMemory(FiFormat, fi_mem, 0);

	int bpp = FreeImage_GetBPP(pFiBitmap);
	int iWidth = FreeImage_GetWidth(pFiBitmap);
	int iHeight = FreeImage_GetHeight(pFiBitmap);

	int iRowPitch = iWidth * bpp / 8;
	while (iRowPitch % 4)
		++iRowPitch;
	int imageSize = iRowPitch * iHeight;

	m_dwImageWidth = iWidth;
	m_dwImageHeight = iHeight;
	m_dwImageBpp = bpp;

	m_pImageData.reset(new BYTE[imageSize]);

	BYTE* pImageData = FreeImage_GetBits(pFiBitmap);
	memcpy(m_pImageData.get(), pImageData, imageSize);

	if (bMultiBitmap)
	{
		FreeImage_UnlockPage(pFiMultiBitmap, pFiBitmap, FALSE);
		FreeImage_CloseMultiBitmap(pFiMultiBitmap, 0);
	}
	else
	{
		FreeImage_Unload(pFiBitmap);
	}
	FreeImage_CloseMemory(fi_mem);

	if (bpp == 32)
	{
		auto pBuffer = m_pImageData.get();
		for (auto m = 0; m < m_dwImageHeight; m++)
		{
			for (auto n = 0; n < m_dwImageWidth; n++)
			{
				auto dwDataIndex = iRowPitch * m + n * 4;

				pBuffer[dwDataIndex] = pBuffer[dwDataIndex] * pBuffer[dwDataIndex + 3] / 255 +
					(255 - pBuffer[dwDataIndex + 3]) * GetBValue(m_bkColor) / 255;
				pBuffer[dwDataIndex + 1] = pBuffer[dwDataIndex + 1] * pBuffer[dwDataIndex + 3] / 255 +
					(255 - pBuffer[dwDataIndex + 3]) * GetGValue(m_bkColor) / 255;
				pBuffer[dwDataIndex + 2] = pBuffer[dwDataIndex + 2] * pBuffer[dwDataIndex + 3] / 255 +
					(255 - pBuffer[dwDataIndex + 3]) * GetRValue(m_bkColor) / 255;
			}
		}
	}

	return TRUE;
}

BOOL CImageViewerDoc::LoadImageFromFile(const wchar_t* lpFileName)
{
	FIBITMAP* pFiBitmap = NULL;
	FIMULTIBITMAP* pFiMultiBitmap = NULL;
	DWORD             dwIndex = 0;
	BOOL              bMultiBitmap = FALSE;

	FREE_IMAGE_FORMAT FiFormat = FreeImage_GetFileTypeU(lpFileName, 0);

	if (FiFormat == FIF_UNKNOWN)
		return FALSE;

	if (FiFormat == FIF_JPEG)
		pFiBitmap = FreeImage_LoadU(FiFormat, lpFileName, JPEG_CMYK);
	else if (FiFormat == FIF_ICO)
	{
		bMultiBitmap = TRUE;

		int len = WideCharToMultiByte(CP_ACP, 0, lpFileName, -1, NULL, 0, NULL, NULL);
		char* pFileName = new char[len];
		WideCharToMultiByte(CP_ACP, 0, lpFileName, -1, pFileName, len, NULL, NULL);

		pFiMultiBitmap = FreeImage_OpenMultiBitmap(FiFormat,
			pFileName, FALSE, TRUE, TRUE, ICO_DEFAULT);

		pFiBitmap = FreeImage_LockPage(pFiMultiBitmap, dwIndex);
		for (int i = 0; i < FreeImage_GetPageCount(pFiMultiBitmap); ++i)
		{
			FIBITMAP* pTempBitamp = FreeImage_LockPage(pFiMultiBitmap, i);
			if ((FreeImage_GetWidth(pTempBitamp) > FreeImage_GetWidth(pFiBitmap)) ||
				(FreeImage_GetBPP(pTempBitamp) > FreeImage_GetBPP(pFiBitmap)))
			{
				FreeImage_UnlockPage(pFiMultiBitmap, pFiBitmap, FALSE);
				pFiBitmap = pTempBitamp;
				dwIndex = i;
				continue;
			}
			FreeImage_UnlockPage(pFiMultiBitmap, pTempBitamp, FALSE);
		}
		FreeImage_CloseMultiBitmap(pFiMultiBitmap, 0);

		pFiMultiBitmap = FreeImage_OpenMultiBitmap(FiFormat,
			pFileName, FALSE, TRUE, TRUE, ICO_MAKEALPHA);
		pFiBitmap = FreeImage_LockPage(pFiMultiBitmap, dwIndex);
		delete[] pFileName;
	}
	else if (FiFormat == FIF_GIF)
	{
		bMultiBitmap = TRUE;

		int len = WideCharToMultiByte(CP_ACP, 0, lpFileName, -1, NULL, 0, NULL, NULL);
		char* pFileName = new char[len];
		WideCharToMultiByte(CP_ACP, 0, lpFileName, -1, pFileName, len, NULL, NULL);

		pFiMultiBitmap = FreeImage_OpenMultiBitmap(FiFormat,
			pFileName, FALSE, TRUE, TRUE, GIF_PLAYBACK);
		pFiBitmap = FreeImage_LockPage(pFiMultiBitmap, 0);
		delete[] pFileName;
	}
	else
		pFiBitmap = FreeImage_LoadU(FiFormat, (LPCTSTR)lpFileName, 0);

	int bpp = FreeImage_GetBPP(pFiBitmap);
	int iWidth = FreeImage_GetWidth(pFiBitmap);
	int iHeight = FreeImage_GetHeight(pFiBitmap);

	int iRowPitch = iWidth * bpp / 8;
	while (iRowPitch % 4)
		++iRowPitch;
	int imageSize = iRowPitch * iHeight;

	m_dwImageWidth = iWidth;
	m_dwImageHeight = iHeight;
	m_dwImageBpp = bpp;

	m_pImageData.reset(new BYTE[imageSize]);

	BYTE* pImageData = FreeImage_GetBits(pFiBitmap);
	memcpy(m_pImageData.get(), pImageData, imageSize);

	if (bMultiBitmap)
	{
		FreeImage_UnlockPage(pFiMultiBitmap, pFiBitmap, FALSE);
		FreeImage_CloseMultiBitmap(pFiMultiBitmap, 0);
	}
	else
	{
		FreeImage_Unload(pFiBitmap);
	}

	if (bpp == 32)
	{
		COLORREF BgColor = m_bkColor;

		auto pBuffer = m_pImageData.get();
		for (auto m = 0; m < m_dwImageHeight; m++)
		{
			for (auto n = 0; n < m_dwImageWidth; n++)
			{
				auto dwDataIndex = iRowPitch * m + n * 4;

				pBuffer[dwDataIndex] = pBuffer[dwDataIndex] * pBuffer[dwDataIndex + 3] / 255 +
					(255 - pBuffer[dwDataIndex + 3]) * GetBValue(BgColor) / 255;
				pBuffer[dwDataIndex + 1] = pBuffer[dwDataIndex + 1] * pBuffer[dwDataIndex + 3] / 255 +
					(255 - pBuffer[dwDataIndex + 3]) * GetGValue(BgColor) / 255;
				pBuffer[dwDataIndex + 2] = pBuffer[dwDataIndex + 2] * pBuffer[dwDataIndex + 3] / 255 +
					(255 - pBuffer[dwDataIndex + 3]) * GetRValue(BgColor) / 255;
			}
		}
	} 

	return TRUE;
}

std::shared_ptr<BYTE[]> CImageViewerDoc::SaveImageToFile(const wchar_t* lpFileName, const wchar_t* lpDstFileName, int &len)
{
	FIBITMAP* pFiBitmap = NULL;
	FIMULTIBITMAP* pFiMultiBitmap = NULL;
	DWORD             dwIndex = 0;
	BOOL              bMultiBitmap = FALSE;

	FREE_IMAGE_FORMAT FiFormat = FreeImage_GetFileTypeU(lpFileName, 0);

	if (FiFormat == FIF_UNKNOWN)
		return NULL;

	if (FiFormat == FIF_JPEG)
		pFiBitmap = FreeImage_LoadU(FiFormat, lpFileName, JPEG_CMYK);
	else if (FiFormat == FIF_ICO)
	{
		bMultiBitmap = TRUE;

		int len = WideCharToMultiByte(CP_ACP, 0, lpFileName, -1, NULL, 0, NULL, NULL);
		char* pFileName = new char[len];
		WideCharToMultiByte(CP_ACP, 0, lpFileName, -1, pFileName, len, NULL, NULL);

		pFiMultiBitmap = FreeImage_OpenMultiBitmap(FiFormat,
			pFileName, FALSE, TRUE, TRUE, ICO_DEFAULT);

		pFiBitmap = FreeImage_LockPage(pFiMultiBitmap, dwIndex);
		for (int i = 0; i < FreeImage_GetPageCount(pFiMultiBitmap); ++i)
		{
			FIBITMAP* pTempBitamp = FreeImage_LockPage(pFiMultiBitmap, i);
			if ((FreeImage_GetWidth(pTempBitamp) > FreeImage_GetWidth(pFiBitmap)) ||
				(FreeImage_GetBPP(pTempBitamp) > FreeImage_GetBPP(pFiBitmap)))
			{
				FreeImage_UnlockPage(pFiMultiBitmap, pFiBitmap, FALSE);
				pFiBitmap = pTempBitamp;
				dwIndex = i;
				continue;
			}
			FreeImage_UnlockPage(pFiMultiBitmap, pTempBitamp, FALSE);
		}
		FreeImage_CloseMultiBitmap(pFiMultiBitmap, 0);

		pFiMultiBitmap = FreeImage_OpenMultiBitmap(FiFormat,
			pFileName, FALSE, TRUE, TRUE, ICO_MAKEALPHA);
		pFiBitmap = FreeImage_LockPage(pFiMultiBitmap, dwIndex);
		delete[] pFileName;
	}
	else if (FiFormat == FIF_GIF)
	{
		bMultiBitmap = TRUE;

		int len = WideCharToMultiByte(CP_ACP, 0, lpFileName, -1, NULL, 0, NULL, NULL);
		char* pFileName = new char[len];
		WideCharToMultiByte(CP_ACP, 0, lpFileName, -1, pFileName, len, NULL, NULL);

		pFiMultiBitmap = FreeImage_OpenMultiBitmap(FiFormat,
			pFileName, FALSE, TRUE, TRUE, GIF_PLAYBACK);
		pFiBitmap = FreeImage_LockPage(pFiMultiBitmap, 0);
		delete[] pFileName;
	}
	else
		pFiBitmap = FreeImage_LoadU(FiFormat, (LPCTSTR)lpFileName, 0);

	auto fifDst = FreeImage_GetFIFFromFilenameU(lpDstFileName);

	std::shared_ptr<BYTE[]> data = NULL;

	if (fifDst != FIF_UNKNOWN)
	{ 
		auto dst = FreeImage_OpenMemory();
		auto ok = FreeImage_SaveToMemory(fifDst, pFiBitmap, dst);
		BYTE* pData = NULL; 
		FreeImage_AcquireMemory(dst, &pData, (DWORD*)&len);
		auto pBuffer = new BYTE[len];
		memcpy(pBuffer, pData, len);
		FreeImage_CloseMemory(dst); 
		data.reset(pBuffer);
	}

	if (bMultiBitmap)
	{
		FreeImage_UnlockPage(pFiMultiBitmap, pFiBitmap, FALSE);
		FreeImage_CloseMultiBitmap(pFiMultiBitmap, 0);
	}
	else
	{
		FreeImage_Unload(pFiBitmap);
	}

	return data;
}


BOOL CImageViewerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class

	if (lpszPathName == m_strPathName)
		return FALSE;

	auto fifDst = FreeImage_GetFIFFromFilenameU(lpszPathName);
	if (!FreeImage_FIFSupportsWriting(fifDst))
	{
		AfxGetMainWnd()->MessageBox(TEXT("格式不支持"));
		return FALSE;
	}

	return CDocument::OnSaveDocument(lpszPathName);
}


BOOL CImageViewerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  Add your specialized creation code here
	auto fif = FreeImage_GetFIFFromFilenameU(lpszPathName);
	return FreeImage_FIFSupportsReading(fif); 
}
