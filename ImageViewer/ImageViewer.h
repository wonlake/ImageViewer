
// ImageViewer.h : ImageViewer Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CImageViewerApp:
// �йش����ʵ�֣������ ImageViewer.cpp
//

class CImageViewerApp : public CWinAppEx
{
public:
	CImageViewerApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CImageViewerApp theApp;
