
const int black=0;
const int white=255;


class Mask	//数学形态学中的模板类
{
public:
	int cols;
	int rows;
	BYTE *kern;
};
class  CMor
{
public:
	int x;
	int y;
};
class CImage
{
public:
    CImage();
	~CImage();

public:
	int MaxX,MaxY,MinX,MinY;
	BYTE *data;
	bool flag;
	int ImageHeight,ImageWidth;
	
};

class CImage_Class
{
public:
	bool Save(LPCTSTR filename);//保存文件
	void ShowEdge(CDC *pDC);//显示边缘图像
	void ShowOri(CDC *pDC);//显示原图像
	bool Load(LPCTSTR filename);//加载图像
	void Edge_Laplace( CImage img, CImage Edge);//
	void Edge_Rober(CImage img, CImage Edge);
	void Edge_Grad(CImage img, CImage Edge);
	void Math_Morph_Erode(Mask msk,CImage img);
	void Math_Morph_Dilate(Mask msk,CImage img);
    
	CImage_Class();
	virtual ~CImage_Class();



	CFile ImageFile;
	BITMAPFILEHEADER BitmapFileHeader;//文件头信息
	BITMAPINFO bitmapinfo;		//图像信息
	char BitmapInfo[1024];		//用于记录文件头信息
    CImage Imagedata[5];//用于存放打开或临时保存的公式图像数据
	CImage Edgedata;
	int ImageWidth,ImageHeight; //公式图像实际的宽和高
	int iShowFlag;				//用于记录显示方法的标志
};