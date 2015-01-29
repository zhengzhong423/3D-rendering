#ifndef _CAMERA_H
#define _CAMERA_H
int fps=60;
// 下面的类是一个3D矢量类
class CVect3
{
public:


	// 缺省构造函数
	CVect3() {}

	// 用户构造函数
	CVect3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// 定义矢量之间的'+'法 
	CVect3 operator+(CVect3 vVector)
	{
		// 返回结果
		return CVect3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	// 定义矢量之间的'-'法 
	CVect3 operator-(CVect3 vVector)
	{
		// 返回矢量相减的结果
		return CVect3(x - vVector.x, y - vVector.y, z - vVector.z);
	}

	// 定义矢量与数的'*'法
	CVect3 operator*(float num)
	{
		// 返回结果
		return CVect3(x * num, y * num, z * num);
	}

	// 定义矢量与数的'/'法
	CVect3 operator/(float num)
	{
		// 返回结果
		return CVect3(x / num, y / num, z / num);
	}

	float x, y, z;                        
};

// 摄像机类
class CCamera {

public:

	// 摄像机类的构造函数
	CCamera();    

	// 下面的函数是获取有关摄像机的数据
	CVect3 Position() {    return m_vPosition;        }
	CVect3 View()        {    return m_vView;            }
	CVect3 UpVector() {    return m_vUpVector;        }
	CVect3 Strafe()    {    return m_vStrafe;        }

	//  摄像机位置
	void PositionCamera(float positionX, float positionY, float positionZ,
		float viewX,     float viewY,     float viewZ,
		float upVectorX, float upVectorY, float upVectorZ);

	// 旋转摄像机
	void RotateView(float angle, float X, float Y, float Z);

	// 移动视点
	void SetViewByMouse(); 

	// 绕一点旋转摄像机
	void RotateAroundPoint(CVect3 vCenter, float X, float Y, float Z);
	//  左右移动摄像机
	void StrafeCamera(float speed);
	//  移动摄像机
	void MoveCamera(float speed);
	// 键盘事件
	void CheckForMovement();
	void Update();
	void Look();

private:

	// 摄像机的位置
	CVect3 m_vPosition;                    
	// 摄像机的视野
	CVect3 m_vView;                        
	// 摄像机的向上的位置
	CVect3 m_vUpVector;        
	//  摄像机左右方向    
	CVect3 m_vStrafe;                        
};


#endif

#define kSpeed    50.0f                                    

float g_FrameInterval = 0.0f;





//  下面的函数的功能是计算个矢量的叉积，即求与两个矢量都垂直的矢量
CVect3 Cross(CVect3 vVector1, CVect3 vVector2)
{
	CVect3 vNormal;    

	// 计算垂直矢量
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	// 返回结果
	return vNormal;                                         
}


//  下面的函数的功能是求矢量的长度
float Magnitude(CVect3 vNormal)
{
	return (float)sqrt( (vNormal.x * vNormal.x) + 
		(vNormal.y * vNormal.y) + 
		(vNormal.z * vNormal.z) );
}


//  下面的函数的功能是将矢量单位化
CVect3 Normalize(CVect3 vVector)
{
	// 获得矢量的长度
	float magnitude = Magnitude(vVector);                

	vVector = vVector / magnitude;        

	return vVector;                                        
}


//  下面的函数是类CCamera的构造函数
CCamera::CCamera()
{
	CVect3 vZero = CVect3(0.0, 0.0, 0.0);        // 初始化摄像机位置
	CVect3 vView = CVect3(0.0, 1.0, 0.5);        // 初始化摄像机方向 
	CVect3 vUp   = CVect3(0.0, 0.0, 1.0);        // 初始化摄像机的向上方向

	m_vPosition    = vZero;
	m_vView        = vView;
	m_vUpVector    = vUp;    

}


//  设置摄像机的位置、方向
void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
	float viewX,     float viewY,     float viewZ,
	float upVectorX, float upVectorY, float upVectorZ)
{
	CVect3 vPosition    = CVect3(positionX, positionY, positionZ);
	CVect3 vView        = CVect3(viewX, viewY, viewZ);
	CVect3 vUpVector    = CVect3(upVectorX, upVectorY, upVectorZ);

	m_vPosition = vPosition;
	m_vView     = vView;
	m_vUpVector = vUpVector;
}


//  下面的函数的功能是通过鼠标设置视点
void CCamera::SetViewByMouse()
{
	POINT mousePos;    
	int middleX = 910;
	int middleY = 540;
	float angleY = 0.0f;
	float angleZ = 0.0f;
	static float currentRotX = 0.0f;
	
	// 获得鼠标的当前位置
	GetCursorPos(&mousePos);                        
	
	// 如果鼠标位于窗口的正中央，则返回
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;

	// 设置鼠标的位置为窗口正中央
	SetCursorPos(middleX, middleY);                            

	// 计算角度
	angleY = (float)( (middleX - mousePos.x) ) / 500.0f;        
	angleZ = (float)( (middleY - mousePos.y) ) / 500.0f;        

	currentRotX -= angleZ;  

	if(currentRotX > 1.0f)
	    currentRotX = 1.0f;
	else if(currentRotX < -1.0f)
	    currentRotX = -1.0f;
	// 旋转观察方向
	else
	{
	    CVect3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
	    vAxis = Normalize(vAxis);

	    RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	    RotateView(angleY, 0, 1, 0);
	}
}


//  下面的函数的功能是将摄像机的观察方向绕某个方向轴旋转一定的角度
void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVect3 vNewView;

	// 获得观察方向矢量
	CVect3 vView = m_vView - m_vPosition;        

	// 计算角度的cos和sin值
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// 计算新的观察点坐标X
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)        * vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)    * vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)    * vView.z;

	// 计算新的观察点坐标Y
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)    * vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)        * vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)    * vView.z;

	// 计算新的观察点坐标Z
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)    * vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)    * vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)        * vView.z;
	m_vView = m_vPosition + vNewView;
}

//  下面的函数的功能是向左向右移动摄像机
void CCamera::StrafeCamera(float speed)
{    
	// Add the strafe vector to our position
	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.z += m_vStrafe.z * speed;

	// Add the strafe vector to our view
	m_vView.x += m_vStrafe.x * speed;
	m_vView.z += m_vStrafe.z * speed;
}

//  下面的函数的功能是根据一定的速度前后移动摄像机
void CCamera::MoveCamera(float speed)
{
	// 获得当前摄像机方向
	CVect3 vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector); //摄像机方向的单位向量

	m_vPosition.x += vVector.x * speed;        // 移动摄像机的位置坐标X
	m_vPosition.y += vVector.y * speed;        // 移动摄像机的位置坐标Y
	m_vPosition.z += vVector.z * speed;        // 移动摄像机的位置坐标Z
	m_vView.x += vVector.x * speed;            // 摄像机X方向移动
	m_vView.y += vVector.y * speed;            // 摄像机Y方向移动
	m_vView.z += vVector.z * speed;            // 摄像机Z方向移动
}


//  下面的函数的功能是根据不同的按键，移动摄像机
void CCamera::CheckForMovement()
{    
	if(fps==0) fps=60;

	// 获得当前帧率
	float speed = 60.0f/fps;

	// 是否按下UP箭头键或'W'键
	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {                

		// 移动摄像机
		MoveCamera(speed);                
	}

	// 是否按下DOWN键或'S'键
	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {            

		// 移动摄像机
		MoveCamera(-speed);                
	}

	// 是否按下LEFT箭头键或'A'键
	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {            

		// 移动摄像机
		StrafeCamera(-speed);
	}

	// 是否按下RIGHT箭头键或'D'键
	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {            

		// 移动摄像机
		StrafeCamera(speed);
	}    
}


//  下面的函数的功能是更新摄像机方向
void CCamera::Update() 
{
	// 初始化变量
	CVect3 vCross = Cross(m_vView - m_vPosition, m_vUpVector);

	// 规范化矢量
	m_vStrafe = Normalize(vCross);

	// 移动摄像机方向
	SetViewByMouse();

	// 判断是否有按键
	CheckForMovement();

}


//  下面的函数的功能是设置投影变换
void CCamera::Look()
{
	// 根据摄像机的位置、方向和上下方向设置投影变换
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,    
		m_vView.x,     m_vView.y,     m_vView.z,    
		m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}

