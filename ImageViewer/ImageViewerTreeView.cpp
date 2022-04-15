// ImageViewerMJTreeView.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageViewer.h"
#include "ImageViewerTreeView.h"
#include "ImageViewerView.h"
#include "MainFrm.h"
#include <string>
#include <tchar.h>

// CImageViewerMJTreeView

IMPLEMENT_DYNCREATE(CImageViewerTreeView, CTreeView)

CImageViewerTreeView::CImageViewerTreeView()
{

}

CImageViewerTreeView::~CImageViewerTreeView()
{
	std::map<std::wstring, BUFFER_DESC>::iterator iter =
		m_mapFiles.begin();
	while (iter != m_mapFiles.end())
	{
		delete[] iter->second.pData;
		++iter;
	}
	m_mapFiles.clear();
}

BEGIN_MESSAGE_MAP(CImageViewerTreeView, CTreeView)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CImageViewerTreeView::OnNMDblclk)
END_MESSAGE_MAP()


// CImageViewerMJTreeView 诊断

#ifdef _DEBUG
void CImageViewerTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CImageViewerTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG


// CImageViewerTreeView 消息处理程序


void CImageViewerTreeView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO:  在此添加专用代码和/或调用基类	
	CTreeCtrl * ptheTree;//实例对对象
	ptheTree = &GetTreeCtrl();
	ptheTree->ModifyStyle(0, TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS);
}


int CImageViewerTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	::DragAcceptFiles(m_hWnd, TRUE);

	return 0;
}


void CImageViewerTreeView::OnDropFiles(HDROP hDropInfo)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CString strFile;
	UINT nFilesCount = DragQueryFile(hDropInfo, INFINITE, NULL, 0);
	for (UINT i = 0; i < nFilesCount; i++)
	{ 
		auto pathLen = DragQueryFile(hDropInfo, i, strFile.GetBuffer(MAX_PATH), MAX_PATH);
		strFile.ReleaseBuffer(pathLen);
	}

	CFile File(strFile, CFile::modeRead);

	auto lFileSize = File.GetLength();
	auto file_size = lFileSize;
	BYTE* pData = new BYTE[file_size];
	File.Read(pData, (UINT)file_size);

	auto name_size = strFile.GetLength();
	long iNewDataSize = 0;

	CTreeCtrl& fileListTree = GetTreeCtrl();

	if (lFileSize > sizeof(MI_HEADER))
	{
		TCHAR strFormat[MAX_PATH] = { 0 };
		TCHAR strFormat2[32] = { 0 };

		BUFFER_DESC desc;
		MI_HEADER header;
		memcpy(&header, pData, sizeof(header));
		
		CImageViewerView* pView = 
			(CImageViewerView*)(((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.GetPane(0, 1));


		if (strcmp(header.szFileFlag, "MAGIC IMAGE") == 0)
		{
			fileListTree.DeleteAllItems();

			std::map<std::wstring, BUFFER_DESC>::iterator iter =
				m_mapFiles.begin();
			while (iter != m_mapFiles.end())
			{
				delete[] iter->second.pData;
				++iter;
			}
			m_mapFiles.clear();

			int numFiles = header.uiNumFiles;

			FILE_DESC* pFileDesc = (FILE_DESC*)(pData + sizeof(MI_HEADER));
			int* pBufferSize = (int*)(pData + header.uiDataOffsetLow + header.uiDataSizeLow);
			char* pBuffer = new char[*pBufferSize];
			memcpy(pBuffer, pBufferSize + 1, *pBufferSize);
			WCHAR* pTitle = (WCHAR*)pBuffer;

			BOOL bHasFilenameDetail = FALSE;
			int iFileNameDetailOffset =
				*pBufferSize + 4 + header.uiDataOffsetLow + header.uiDataSizeLow;

			if (iFileNameDetailOffset < lFileSize)
			{
				bHasFilenameDetail = TRUE;
			}
			int iNumZero = 1;
			int iFiles = numFiles;

			while (iFiles / 10)
			{
				iFiles = iFiles / 10;
				++iNumZero;
			}
			_stprintf_s(strFormat2, _T("%%0%dd"), iNumZero);

			for (int i = 0; i < numFiles; ++i)
			{
				int iFileSize = pFileDesc->uiSizeLow;
				desc.iDataSize = iFileSize;
				desc.pData = new BYTE[iFileSize];
				memcpy(desc.pData,
					pData + pFileDesc->uiOffsetLow, iFileSize);
				if (bHasFilenameDetail)
				{
					int* pFileNameSize = (int*)(pData + iFileNameDetailOffset);
					char* pFileNameBuffer = new char[*pFileNameSize];
					memcpy(pFileNameBuffer, pFileNameSize + 1, *pFileNameSize);
					WCHAR* pwFileName = (WCHAR*)pFileNameBuffer;
					m_mapFiles[pwFileName] = desc;
					iFileNameDetailOffset += *pFileNameSize + 4;
					delete[] pFileNameBuffer;
				}
				else
				{
					_stprintf_s(strFormat, strFormat2, i + 1);
					m_mapFiles[strFormat] = desc;
				}
				++pFileDesc;
			}

			iter = m_mapFiles.begin();
			if (iter != m_mapFiles.end())
			{
				pView->LoadImageFromMemory(iter->second.pData, iter->second.iDataSize);
			}

			HTREEITEM pRoot = fileListTree.InsertItem(pTitle, NULL);
			delete[] pBuffer;

			while (iter != m_mapFiles.end())
			{
				fileListTree.InsertItem(iter->first.c_str(), pRoot);
				++iter;
			}

			if (m_mapFiles.size() > 0)
			{
				CString strTitle = _T(" - ImageViewerMJ");
				strTitle = strFile + strTitle;
				SetWindowText(strTitle);

				if (!fileListTree.IsWindowVisible())
				{
					fileListTree.ShowWindow(TRUE);

					RECT rcFileList;
					fileListTree.GetClientRect(&rcFileList);

					int width = rcFileList.right - rcFileList.left;
					int height = rcFileList.bottom - rcFileList.top;

					RECT rcWindow;
					GetClientRect(&rcWindow);
					//m_ImageViewer.MoveWindow(rcWindow.left + width + 25, rcWindow.top + 10,
					//	rcWindow.right - rcWindow.left - width - 30,
					//	rcWindow.bottom - rcWindow.top - 20,
					//	TRUE);
				}
			}
		}
		else
		{
			File.Close();
			static TCHAR szFileName[1024] = { 0 };
			::DragQueryFile(hDropInfo, 0, szFileName, sizeof(szFileName));
			pView->LoadImageFromFile(szFileName);
		}
	}

	delete[] pData;

	CTreeView::OnDropFiles(hDropInfo);
}


void CImageViewerTreeView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;

	TCHAR t[MAX_PATH] = { 0 };
	POINT cp;
	TVHITTESTINFO tvhtInfo;
	ZeroMemory(&tvhtInfo, sizeof(tvhtInfo));
	GetCursorPos(&cp);

	CImageViewerView* pView =
		(CImageViewerView*)(((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.GetPane(0, 1));

	CTreeCtrl& treeFileList = GetTreeCtrl();
	treeFileList.ScreenToClient(&cp);  //m_TREE是CTreeCtrl控件变量。
	tvhtInfo.pt = cp;
	treeFileList.HitTest(&tvhtInfo);
	HTREEITEM hItem = tvhtInfo.hItem; //获取当前鼠标右键单击的位置下的item
	TVITEM tvItem;
	ZeroMemory(&tvItem, sizeof(tvItem));
	if (hItem)
	{
		tvItem.mask = TVIF_TEXT;
		tvItem.hItem = hItem;
		tvItem.pszText = t;
		tvItem.cchTextMax = MAX_PATH;
		treeFileList.GetItem(&tvItem);
		std::wstring FullPath = t;
		HTREEITEM hRoot = treeFileList.GetRootItem();
		if (treeFileList.GetChildItem(hItem) == NULL)
		{
			CString strItemText = treeFileList.GetItemText(hItem);
			std::map<std::wstring, BUFFER_DESC>::iterator iter =
				m_mapFiles.find((LPCTSTR)strItemText);
			if (iter != m_mapFiles.end())
			{
				pView->LoadImageFromMemory(iter->second.pData,
					iter->second.iDataSize);
			}
		}
	}
}
