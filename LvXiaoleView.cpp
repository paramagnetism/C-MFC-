
// LvXiaoleView.cpp: CLvXiaoleView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "LvXiaole.h"
#endif

#include "LvXiaoleDoc.h"
#include "LvXiaoleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//使用生成随机数的头文件
#include <cstdlib>
///使用标准输入输出流
#include <iostream>
//使用字符串操作的头文件
#include <string>
//使用容器头文件
#include <vector>


//定义有多少长图片（9）
#define NUMBER 9 
//定义宽度与高度
#define SET_WIDTH 12
#define SET_HEIGHT 7
//定义页边距
#define MARGIN 50

// CLvXiaoleView

IMPLEMENT_DYNCREATE(CLvXiaoleView, CView)

BEGIN_MESSAGE_MAP(CLvXiaoleView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CLvXiaoleView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
//	ON_WM_RBUTTONUP()
//	ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CLvXiaoleView 构造/析构

CLvXiaoleView::CLvXiaoleView() noexcept
{
	size = 50;//图片尺寸大小
	srand((int)time(NULL));//生成不同的随机数种子
	mdc = new CDC;//新建句柄
	char names[6];
	int num;
	//初始化句柄数组
	for (int i = 0; i <= NUMBER; i++) {
		//运用string头文件相关函数生成字符数组名
		std::string name = std::to_string(i) + ".bmp";
		strcpy_s(names, name.c_str());
		cb[i] = new CBitmap;
		cb[i]->Attach((HBITMAP)::LoadImage(NULL, names, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
	}
	//初始化坐标位置数组
	//因为边上也要留空能消除所以从1开始
	std::vector<CPoint> coordinates;
	for (int i = 1; i < SET_WIDTH+1; i++) {
		for (int j = 1; j < SET_HEIGHT+1; j++) {
			coordinates.push_back(CPoint(i,j));
		}
	}
	//初始化边缘点为空，流出预备选择路径
	for (int i = 0; i < SET_WIDTH + 2; i++) {
		for (int j = 0; j < SET_HEIGHT + 2; j++) {
			if (i == 0 || j == 0 || i == SET_WIDTH + 1 || j == SET_HEIGHT + 1)
				random[i][j] = 0;
		}
	}
	//配对生成随机数组，每配对一次从容器中去除一个坐标，直到坐标容器为空
	while(!coordinates.empty()) {
		int k = rand() % NUMBER;//随机图片标号
        //随机生成一个区域，并从容器中删除
		int ran1 = rand() % coordinates.size();
		random[coordinates[ran1].x][coordinates[ran1].y] = k + 1;
		auto it = coordinates.begin()+ ran1;
		coordinates.erase(it);
		//随机再生成一个区域，并从容器中删除
		//对每一个循环中执行用同一张图片该过程两次，以确保图片成双成对出现
		int ran2 = rand() % coordinates.size();
		random[coordinates[ran2].x][coordinates[ran2].y] = k + 1;
		it = coordinates.begin() + ran2;
		coordinates.erase(it);
	}
	//初始化点击点与临时存储（反色）点
	click = CPoint(-1, -1);
	temp = CPoint(-1, -1);
	for (int i = 0; i < 4; i++) {
		linepoints[i] = CPoint(-1,-1);
	}
}

//析构函数
CLvXiaoleView::~CLvXiaoleView(){
}

void CLvXiaoleView::OnDraw(CDC* pDC)
{
	CLvXiaoleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	mdc->CreateCompatibleDC(pDC);//创建框架对象
	//遍历random数组显示图片
	for (int i = 0; i < SET_HEIGHT + 2; i++) {
		for (int j = 0; j < SET_WIDTH + 2; j++) {
			//用句柄加载生成的随机位置的随机图片
			mdc->SelectObject(cb[random[j][i]]);
			if (temp == CPoint(j, i) && random[j][i] != 0)
				pDC->StretchBlt(size * j + MARGIN, size * i + MARGIN, size, size, mdc, 0, 0, 300, 300, SRCINVERT);
			//如果图片是点击过的临时存储点temp则反色显示图片。
			else pDC->StretchBlt(size * j + MARGIN, size * i + MARGIN, size, size, mdc, 0, 0, 300, 300, SRCCOPY);
		}
	}
	//循环画线
	for (int i = 0; i < 3; i++) {
		if (linepoints[i + 1] != CPoint(-1, -1)) {//如果第二个点非空
			pDC->MoveTo(CPoint((linepoints[i].x + 0.5) * size + MARGIN, (linepoints[i].y + 0.5) * size + MARGIN));
			pDC->LineTo(CPoint((linepoints[i + 1].x+0.5) * size + MARGIN, (linepoints[i + 1].y+0.5) * size + MARGIN));
		}
	}
	//画完线以后将点集清零
	for (int i = 0; i < 4; i++) {
		linepoints[i] = CPoint(-1, -1);
	}
	//刷新时销毁框架
	mdc->DeleteDC();
}

void CLvXiaoleView::OnLButtonDown(UINT nFlags, CPoint point)
{   //计算点击坐标
	click = CPoint((point.x-MARGIN)/size,(point.y-MARGIN)/size);
	//判断是否在生成的数组范围内   
	if (click.x > 0 && click.x < SET_WIDTH + 1 && click.y > 0 && click.y < SET_HEIGHT + 1) {    
		//如果之前没有，则反色显示
		if (temp == CPoint(-1, -1)) temp = click;
		//如果之前有点击，判断是否符合匹配条件
		else if (random[temp.x][temp.y] == random[click.x][click.y] && (temp != click) && random[temp.x][temp.y] != 0
			&& ((Single_line(temp, click) || Double_line(temp, click) || Trible_line(temp, click)))) {	
			random[temp.x][temp.y] = 0;
			random[click.x][click.y] = 0;
			click = CPoint(-1, -1);
			temp = CPoint(-1, -1);
		}
		//否则取消反色，将temp点置空。
		else temp = CPoint(-1, -1);
	}
	//刷新该界面
	Invalidate();
	CView::OnLButtonDown(nFlags, point);
}

BOOL CLvXiaoleView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CView::PreCreateWindow(cs);
}

// CLvXiaoleView 打印

void CLvXiaoleView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CLvXiaoleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CLvXiaoleView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CLvXiaoleView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


void CLvXiaoleView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CLvXiaoleView 诊断

#ifdef _DEBUG
void CLvXiaoleView::AssertValid() const
{
	CView::AssertValid();
}

void CLvXiaoleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLvXiaoleDoc* CLvXiaoleView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLvXiaoleDoc)));
	return (CLvXiaoleDoc*)m_pDocument;
}
#endif //_DEBUG


// 单折连接判断
bool CLvXiaoleView::Single_line(CPoint point1, CPoint point2){
	int bigger, smaller;
	//如果两个点横纵坐标都不等，则不对。
	if (point1.x != point2.x && point1.y != point2.y)return false;
	//如果y相同，则对x向中间的点进行遍历检测
	else if (point1.y == point2.y) {
		smaller = min(point1.x, point2.x);
		bigger = max(point1.x, point2.x);
		for (int x = smaller + 1; x < bigger; x++) {
			if (random[x][point1.y] != 0) return false;
			break;
		}
	}
	else {//如果x相同，则对y向中间的点进行遍历检测
		smaller = min(point1.y, point2.y);
		bigger = max(point1.y, point2.y);
			for (int y = smaller + 1; y < bigger; y++) {
				if (random[point1.x][y] != 0)return false;
				break;
			}
	}
	//将符合检测的点存储到点数组中以便绘制
	linepoints[0] = point1;
	linepoints[1] = point2;
	return true;
}

// 双折判断
bool CLvXiaoleView::Double_line(CPoint point1, CPoint point2){   
	//横纵坐标不能相同，相同了要么第一个要么第三个
	if (point1.x == point2.x || point1.y == point2.y)return false;
	//找到大小x、y坐标
	int bigger_x = max(point1.x, point2.x);
	int smaller_x = min(point1.x, point2.x);
	int bigger_y = max(point1.y, point2.y);
	int smaller_y = min(point1.y, point2.y);
	//如果是两点之间连线斜率大于0的情况
	if ((point1.x - point2.x) * (point1.y - point2.y) > 0) {
		//计算下边一条路有多少非空点
		int upcount = 0, downcount = 0;
		for (int x = smaller_x + 1; x < bigger_x; x++) {
			if (random[x][smaller_y] != 0)downcount++;
		}
		for (int y = smaller_y ; y < bigger_y; y++) {
			if (random[bigger_x][y] != 0)downcount++;
		}
		//计算上边一条路有多少非空点
		for (int x = smaller_x ; x < bigger_x; x++) {
			if (random[x][bigger_y] != 0)upcount++;
		}
		for (int y = smaller_y + 1; y < bigger_y; y++) {
			if (random[smaller_x][y] != 0)upcount++;
		}
		//如果下面通路
		if (downcount == 0) {
			//则把符合条件的点存储进数组
			linepoints[0] = point1;
			linepoints[1] = CPoint(bigger_x, smaller_y);
			linepoints[2] = point2;
			return true;
		}
		//如果上面通路
		else if (upcount == 0) {
			//则也把符合条件的点存储进数组
			linepoints[0] = point1;
			linepoints[1] = CPoint(smaller_x, bigger_y);
			linepoints[2] = point2;
			return true;
		}
	}
	//如果是两点之间连线斜率大于0的情况

	else {//计算上边一条路
		int upcount = 0, downcount = 0;
		for (int x = smaller_x + 1; x <= bigger_x; x++) {
			if (random[x][bigger_y] != 0)upcount++;
		}
		for (int y = smaller_y + 1; y <= bigger_y; y++) {
			if (random[bigger_x][y] != 0)upcount++;
		}
		//计算下边一条路
		for (int x = smaller_x; x < bigger_x; x++) {
			if (random[x][smaller_y] != 0)downcount++;
		}
		for (int y = smaller_y ; y < bigger_y; y++) {
			if (random[smaller_x][y] != 0)downcount++;
		}
		//过程同上
		if (downcount == 0) {
			linepoints[0] = point1;
			linepoints[1] = CPoint(smaller_x, smaller_y);
			linepoints[2] = point2;
			return true;
		}
		else if (upcount == 0) {
			linepoints[0] = point1;
			linepoints[1] = CPoint(bigger_x, bigger_y);
			linepoints[2] = point2;
			return true;
		}
	}
	return false;
}
// 三折判断
bool CLvXiaoleView::Trible_line(CPoint point1, CPoint point2)
{   //x方向遍历,到超出他的区域
	for (int x = 0; x < SET_WIDTH + 2; x++) {
		//找到的两个中间点需要为空
		if (random[x][point1.y] != 0||random[x][point2.y]!=0)continue;
		//将三折等效成一个单折+一个双折，判断一次
		if (Single_line(point1, CPoint(x, point1.y)) == false)continue;
		if (Double_line(point2, CPoint(x, point1.y)) == false)continue;
		//为了修改bug，反向再判断一次
		if (Single_line(point2, CPoint(x, point2.y)) == false)continue;
		if (Double_line(point1, CPoint(x, point2.y)) == false)continue;
		//如果符合条件，则将这些点存储进数组。
		linepoints[0] = point1;
		linepoints[1] = CPoint(x, point1.y);
		linepoints[2] = CPoint(x, point2.y);
		linepoints[3] = point2;
		return true;
	}
	//y方向遍历,到超出他的区域，具体方法同上
	for (int y = 0; y < SET_HEIGHT + 2; y++) {
		if (random[point1.x][y] != 0 || random[point2.x][y] != 0)continue;
		if (Single_line(point1, CPoint(point1.x, y)) == false)continue;
		if (Double_line(point2, CPoint(point1.x, y)) == false)continue;
		if (Single_line(point2, CPoint(point2.x, y)) == false)continue;
		if (Double_line(point1, CPoint(point2.x, y)) == false)continue;
		
	    linepoints[0] = point1;
		linepoints[1] = CPoint(point1.x, y);
		linepoints[2] = CPoint(point2.x, y);
		linepoints[3] = point2;
		return true;
	}
	//初始化默认,清空尝试走的路径
	for (int i = 0; i < 4; i++) {
		linepoints[i] = CPoint(-1, -1);
	}
	//否则三折也不通，返回false
	return false;
}
