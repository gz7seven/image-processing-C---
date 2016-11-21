
const int black=0;
const int white=255;


class Mask	//��ѧ��̬ѧ�е�ģ����
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
	bool Save(LPCTSTR filename);//�����ļ�
	void ShowEdge(CDC *pDC);//��ʾ��Եͼ��
	void ShowOri(CDC *pDC);//��ʾԭͼ��
	bool Load(LPCTSTR filename);//����ͼ��
	void Edge_Laplace( CImage img, CImage Edge);//
	void Edge_Rober(CImage img, CImage Edge);
	void Edge_Grad(CImage img, CImage Edge);
	void Math_Morph_Erode(Mask msk,CImage img);
	void Math_Morph_Dilate(Mask msk,CImage img);
    
	CImage_Class();
	virtual ~CImage_Class();



	CFile ImageFile;
	BITMAPFILEHEADER BitmapFileHeader;//�ļ�ͷ��Ϣ
	BITMAPINFO bitmapinfo;		//ͼ����Ϣ
	char BitmapInfo[1024];		//���ڼ�¼�ļ�ͷ��Ϣ
    CImage Imagedata[5];//���ڴ�Ŵ򿪻���ʱ����Ĺ�ʽͼ������
	CImage Edgedata;
	int ImageWidth,ImageHeight; //��ʽͼ��ʵ�ʵĿ�͸�
	int iShowFlag;				//���ڼ�¼��ʾ�����ı�־
};