#ifndef _CAMERA_H
#define _CAMERA_H
int fps=60;
// ���������һ��3Dʸ����
class CVect3
{
public:


	// ȱʡ���캯��
	CVect3() {}

	// �û����캯��
	CVect3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// ����ʸ��֮���'+'�� 
	CVect3 operator+(CVect3 vVector)
	{
		// ���ؽ��
		return CVect3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	// ����ʸ��֮���'-'�� 
	CVect3 operator-(CVect3 vVector)
	{
		// ����ʸ������Ľ��
		return CVect3(x - vVector.x, y - vVector.y, z - vVector.z);
	}

	// ����ʸ��������'*'��
	CVect3 operator*(float num)
	{
		// ���ؽ��
		return CVect3(x * num, y * num, z * num);
	}

	// ����ʸ��������'/'��
	CVect3 operator/(float num)
	{
		// ���ؽ��
		return CVect3(x / num, y / num, z / num);
	}

	float x, y, z;                        
};

// �������
class CCamera {

public:

	// �������Ĺ��캯��
	CCamera();    

	// ����ĺ����ǻ�ȡ�й������������
	CVect3 Position() {    return m_vPosition;        }
	CVect3 View()        {    return m_vView;            }
	CVect3 UpVector() {    return m_vUpVector;        }
	CVect3 Strafe()    {    return m_vStrafe;        }

	//  �����λ��
	void PositionCamera(float positionX, float positionY, float positionZ,
		float viewX,     float viewY,     float viewZ,
		float upVectorX, float upVectorY, float upVectorZ);

	// ��ת�����
	void RotateView(float angle, float X, float Y, float Z);

	// �ƶ��ӵ�
	void SetViewByMouse(); 

	// ��һ����ת�����
	void RotateAroundPoint(CVect3 vCenter, float X, float Y, float Z);
	//  �����ƶ������
	void StrafeCamera(float speed);
	//  �ƶ������
	void MoveCamera(float speed);
	// �����¼�
	void CheckForMovement();
	void Update();
	void Look();

private:

	// �������λ��
	CVect3 m_vPosition;                    
	// ���������Ұ
	CVect3 m_vView;                        
	// ����������ϵ�λ��
	CVect3 m_vUpVector;        
	//  ��������ҷ���    
	CVect3 m_vStrafe;                        
};


#endif

#define kSpeed    50.0f                                    

float g_FrameInterval = 0.0f;





//  ����ĺ����Ĺ����Ǽ����ʸ���Ĳ��������������ʸ������ֱ��ʸ��
CVect3 Cross(CVect3 vVector1, CVect3 vVector2)
{
	CVect3 vNormal;    

	// ���㴹ֱʸ��
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	// ���ؽ��
	return vNormal;                                         
}


//  ����ĺ����Ĺ�������ʸ���ĳ���
float Magnitude(CVect3 vNormal)
{
	return (float)sqrt( (vNormal.x * vNormal.x) + 
		(vNormal.y * vNormal.y) + 
		(vNormal.z * vNormal.z) );
}


//  ����ĺ����Ĺ����ǽ�ʸ����λ��
CVect3 Normalize(CVect3 vVector)
{
	// ���ʸ���ĳ���
	float magnitude = Magnitude(vVector);                

	vVector = vVector / magnitude;        

	return vVector;                                        
}


//  ����ĺ�������CCamera�Ĺ��캯��
CCamera::CCamera()
{
	CVect3 vZero = CVect3(0.0, 0.0, 0.0);        // ��ʼ�������λ��
	CVect3 vView = CVect3(0.0, 1.0, 0.5);        // ��ʼ����������� 
	CVect3 vUp   = CVect3(0.0, 0.0, 1.0);        // ��ʼ������������Ϸ���

	m_vPosition    = vZero;
	m_vView        = vView;
	m_vUpVector    = vUp;    

}


//  �����������λ�á�����
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


//  ����ĺ����Ĺ�����ͨ����������ӵ�
void CCamera::SetViewByMouse()
{
	POINT mousePos;    
	int middleX = 910;
	int middleY = 540;
	float angleY = 0.0f;
	float angleZ = 0.0f;
	static float currentRotX = 0.0f;
	
	// ������ĵ�ǰλ��
	GetCursorPos(&mousePos);                        
	
	// ������λ�ڴ��ڵ������룬�򷵻�
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;

	// ��������λ��Ϊ����������
	SetCursorPos(middleX, middleY);                            

	// ����Ƕ�
	angleY = (float)( (middleX - mousePos.x) ) / 500.0f;        
	angleZ = (float)( (middleY - mousePos.y) ) / 500.0f;        

	currentRotX -= angleZ;  

	if(currentRotX > 1.0f)
	    currentRotX = 1.0f;
	else if(currentRotX < -1.0f)
	    currentRotX = -1.0f;
	// ��ת�۲췽��
	else
	{
	    CVect3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
	    vAxis = Normalize(vAxis);

	    RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	    RotateView(angleY, 0, 1, 0);
	}
}


//  ����ĺ����Ĺ����ǽ�������Ĺ۲췽����ĳ����������תһ���ĽǶ�
void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVect3 vNewView;

	// ��ù۲췽��ʸ��
	CVect3 vView = m_vView - m_vPosition;        

	// ����Ƕȵ�cos��sinֵ
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// �����µĹ۲������X
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)        * vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)    * vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)    * vView.z;

	// �����µĹ۲������Y
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)    * vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)        * vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)    * vView.z;

	// �����µĹ۲������Z
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)    * vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)    * vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)        * vView.z;
	m_vView = m_vPosition + vNewView;
}

//  ����ĺ����Ĺ��������������ƶ������
void CCamera::StrafeCamera(float speed)
{    
	// Add the strafe vector to our position
	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.z += m_vStrafe.z * speed;

	// Add the strafe vector to our view
	m_vView.x += m_vStrafe.x * speed;
	m_vView.z += m_vStrafe.z * speed;
}

//  ����ĺ����Ĺ����Ǹ���һ�����ٶ�ǰ���ƶ������
void CCamera::MoveCamera(float speed)
{
	// ��õ�ǰ���������
	CVect3 vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector); //���������ĵ�λ����

	m_vPosition.x += vVector.x * speed;        // �ƶ��������λ������X
	m_vPosition.y += vVector.y * speed;        // �ƶ��������λ������Y
	m_vPosition.z += vVector.z * speed;        // �ƶ��������λ������Z
	m_vView.x += vVector.x * speed;            // �����X�����ƶ�
	m_vView.y += vVector.y * speed;            // �����Y�����ƶ�
	m_vView.z += vVector.z * speed;            // �����Z�����ƶ�
}


//  ����ĺ����Ĺ����Ǹ��ݲ�ͬ�İ������ƶ������
void CCamera::CheckForMovement()
{    
	if(fps==0) fps=60;

	// ��õ�ǰ֡��
	float speed = 60.0f/fps;

	// �Ƿ���UP��ͷ����'W'��
	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {                

		// �ƶ������
		MoveCamera(speed);                
	}

	// �Ƿ���DOWN����'S'��
	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {            

		// �ƶ������
		MoveCamera(-speed);                
	}

	// �Ƿ���LEFT��ͷ����'A'��
	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {            

		// �ƶ������
		StrafeCamera(-speed);
	}

	// �Ƿ���RIGHT��ͷ����'D'��
	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {            

		// �ƶ������
		StrafeCamera(speed);
	}    
}


//  ����ĺ����Ĺ����Ǹ������������
void CCamera::Update() 
{
	// ��ʼ������
	CVect3 vCross = Cross(m_vView - m_vPosition, m_vUpVector);

	// �淶��ʸ��
	m_vStrafe = Normalize(vCross);

	// �ƶ����������
	SetViewByMouse();

	// �ж��Ƿ��а���
	CheckForMovement();

}


//  ����ĺ����Ĺ���������ͶӰ�任
void CCamera::Look()
{
	// �����������λ�á���������·�������ͶӰ�任
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,    
		m_vView.x,     m_vView.y,     m_vView.z,    
		m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}

