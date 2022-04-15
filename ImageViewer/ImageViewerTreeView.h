#pragma once

#include <map>
#include <string>
// CImageViewerMJTreeView 视图

class CImageViewerTreeView : public CTreeView
{
	DECLARE_DYNCREATE(CImageViewerTreeView)

protected:
	CImageViewerTreeView();           // 动态创建所使用的受保护的构造函数
	virtual ~CImageViewerTreeView();

public:
	enum FileType
	{
		FT_UNKNOWN = 0x00000000,
		FT_JPG = 0x00000001,
		FT_PNG = 0x00000002,
		FT_GIF = 0x00000003,
		FT_BMP = 0x00000004,
		FT_ICO = 0x00000005,
	};

	struct FILE_DESC
	{
		unsigned int uiSizeLow;
		unsigned int uiSizeHigh;
		unsigned int uiOffsetLow;
		unsigned int uiOffsetHigh;
		unsigned int uiFileFormatFlags;
	};

	struct MI_HEADER
	{
		char szFileFlag[16];
		unsigned int uiFileHeaderSize;
		unsigned int uiDataSizeLow;
		unsigned int uiDataSizeHigh;
		unsigned int uiDataOffsetLow;
		unsigned int uiDataOffsetHigh;
		unsigned int uiNumFiles;
	};

	struct BUFFER_DESC
	{
		BYTE* pData;
		unsigned int iDataSize;
	};

	std::map<std::wstring, BUFFER_DESC> m_mapFiles;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};


