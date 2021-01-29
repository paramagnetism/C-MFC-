
// LvXiaoleView.h: CLvXiaoleView 类的接口
//

#pragma once

#define NUMBER 9

class CLvXiaoleView : public CView
{
protected: // 仅从序列化创建
	CLvXiaoleView() noexcept;
	DECLARE_DYNCREATE(CLvXiaoleView)
	
// 特性
public:
	CLvXiaoleDoc* GetDocument() const;
// 操作
public:
	CBitmap *cb[NUMBER+1];
	CDC *mdc;
	int size;
	int random[20][20];
	CPoint click,temp;
	CPoint linepoints[4];
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
	virtual ~CLvXiaoleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	// 单点判断
	bool Single_line(CPoint point1, CPoint point2);
	// 双折判断
	bool Double_line(CPoint point1, CPoint point2);
	// 三折判断
	bool Trible_line(CPoint point1, CPoint point2);
};

#ifndef _DEBUG  // LvXiaoleView.cpp 中的调试版本
inline CLvXiaoleDoc* CLvXiaoleView::GetDocument() const
   { return reinterpret_cast<CLvXiaoleDoc*>(m_pDocument); }
#endif