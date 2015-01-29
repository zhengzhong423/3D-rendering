#include <windows.h>
#include <math.h>
#include <gl\glut.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <olectl.h>  
#include <list>
#include "Misc.h"
#include "camera.h"

float w, h, ratio;
static int xpos = 0, ypos = 0, zpos = 0;
static int yscope = 0;
float Noise1(int x, int y);
float SmoothNoise_1(int x, int y);
float Cosine_Interpolate(float a, float b, float x);
float InterpolatedNoise_1(float x, float y);
float PerlinNoise_2D(float x, float y);

GLuint texgrass;
GLuint texlightspot;
GLuint texlightspot2;
GLuint texlightspot3;

CCamera camera;
CVect3 EyePosition;

float alphaSky=0.0;
float angle;
vec axis;

struct Terrain
{
	float x, y, z;
	float s, t;
	float norx, nory, norz;
};

bool isAdd=true;
float cover=10.0f;
float Noise(int x, int y, int random)
{
	int n = x + y * 57 + random * 131;
	n = (n<<13) ^ n;
	return (1.0f - ( (n * (n * n * 15731 + 789221) +
		1376312589)&0x7fffffff)* 0.000000000931322574615478515625f);
}

void SetNoise(float *map32)   
{
	float temp[34][34];
	int random = rand() % 5000;

	for (int y=1; y<33; y++){
		for (int x=1; x<33; x++)
		{
			temp[x][y] = 128.0f + Noise(x,y,random) * 128.0f;
		}
	}
	for (int x=1; x<33; x++)
	{
		temp[0][x] = temp[32][x];
		temp[33][x] = temp[1][x];
		temp[x][0] = temp[x][32];
		temp[x][33] = temp[x][1];
	}
	temp[0][0] = temp[32][32];
	temp[33][33] = temp[1][1];
	temp[0][33] = temp[32][1];
	temp[33][0] = temp[1][32];
	for (int y = 1; y < 33; y++){        
		for (int x = 1; x < 33; x++)
		{
			float center = temp[x][y] / 4.0f;
			float sides = (temp[x+1][y] + temp[x-1][y] + temp[x][y+1] + temp[x][y-1])/8.0f;
			float corners = (temp[x+1][y+1] + temp[x+1][y-1] + temp[x-1][y+1] + temp[x-1][y-1])/16.0f;

			map32[((x-1)*32) + (y-1)] = center + sides + corners;
		}
	}
}

float Interpolate(float x,float y,float *map)      
{
	int Xint = (int)x;
	int Yint = (int)y;

	float Xfrac = x - Xint;
	float Yfrac = y - Yint;

	int X0 = Xint % 32;
	int Y0 = Yint % 32;
	int X1 = (Xint + 1) % 32;
	int Y1 = (Yint + 1) % 32;

	float bot = map[X0*32 + Y0] + Xfrac * (map[X1*32 + Y0] - map[X0*32 + Y0]);
	float top = map[X0*32 + Y1] + Xfrac * (map[X1*32 + Y1] - map[X0*32 + Y1]);

	return (bot + Yfrac * (top - bot));
}

void OverlapOctaves(float *map32, float *map256)  
{
	for (int x=0; x<256*256; x++)
	{
		map256[x] = 0;
	}
	for (int octave=0; octave<4; octave++){
		for (int x=0; x<256; x++){
			for (int y=0; y<256; y++)
			{
				float scale = 1 / pow(2.0, 2.0-octave);
				float noise = Interpolate(x*scale, y*scale , map32);
				map256[(y*256) + x] += noise / pow(2.0, 0.0+octave);
			}
		}
	}
}

void ExpFilter(float  *map)         
{
	float sharpness = 0.95f;
	for (int x=0; x<256*256; x++)
	{
		float c = map[x] - (255.0f-cover);
		if (c<0)     c = 0;
		map[x] = 255.0f - ((float)(pow(sharpness, c))*255.0f);
	}
}
float map32[32 * 32];  
float map256[256 * 256];          

void cloud()
{
	OverlapOctaves(map32, map256);
	ExpFilter(map256);
}

Terrain terrain[250][250];

void initTerrain()
{
	int x,z;
	for(x=0;x<=249;x++)
		for( z=0;z<=249;z++)
		{
			terrain[x][z].x =(float)(x- 125)*4.0f;
			terrain[x][z].y =100.0f*PerlinNoise_2D((x+10000)/10.0f, (z+10000)/10.0f);
			terrain[x][z].z =(float)((z- 125)*4.0f);
			terrain[x][z].s =x/15.0f;
			terrain[x][z].t =z/15.0f;
		}

		for(x=0;x<=249;x++)
			for(z=0;z<=249;z++)
			{
				if(x>0 && z>0 && x<249 && z<249)
				{
					vec v1;
					v1.x=terrain[x+1][z].y - terrain[x-1][z].y;
					v1.y= 0.5f;
					v1.z=terrain[x][z+1].y - terrain[x][z-1].y;
					v1.Normalize();
					terrain[x][z].norx =v1.x;
					terrain[x][z].nory =v1.y;
					terrain[x][z].norz =v1.z;
				}
				else
				{
					terrain[x][z].norx =0.0f;
					terrain[x][z].nory =1.0f;
					terrain[x][z].norz =0.0f;
				}
			}

}


void DrawTerrain()
{
	int z,x;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texgrass);
	for(z=0;z<=248;z++)
	{
		glBegin(GL_QUAD_STRIP);
		for(x=0;x<=249;x++)
		{
			glNormal3f(terrain[x][z].norx , terrain[x][z].nory , terrain[x][z].norz); 
			glTexCoord2f(terrain[x][z].s, terrain[x][z].t);
			glVertex3f(terrain[x][z].x, terrain[x][z].y, terrain[x][z].z);
			glNormal3f(terrain[x][z+1].norx , terrain[x][z+1].nory , terrain[x][z+1].norz); 
			glTexCoord2f(terrain[x][z+1].s, terrain[x][z+1].t );
			glVertex3f(terrain[x][z+1].x, terrain[x][z+1].y, terrain[x][z+1].z);
		}
		glEnd();
	}
	glBindTexture(GL_TEXTURE_2D, texlightspot3);
	for(z=149; z<=158;z++)
	{
		glBegin(GL_QUAD_STRIP);
		for(x=139;x<=147;x++)
		{
			glTexCoord2f((x-139)/(9.0f), (z-149)/(9.0f));
			glVertex3f(terrain[x][z].x, terrain[x][z].y, terrain[x][z].z);
			glTexCoord2f((x-139)/(9.0f), (z-148)/(9.0f));
			glVertex3f(terrain[x][z+1].x, terrain[x][z+1].y, terrain[x][z+1].z);
		}
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, texlightspot2);
	glDisable(GL_BLEND);

	for(z=0;z<=248;z++)
	{
		glBegin(GL_QUAD_STRIP);
		for(x=0;x<=249;x++)
		{  
			if(terrain[x][z].y>50)
			{
				glNormal3f(terrain[x][z].norx , terrain[x][z].nory , terrain[x][z].norz); 
				glTexCoord2f(terrain[x][z].s, terrain[x][z].t);
				glVertex3f(terrain[x][z].x, terrain[x][z].y, terrain[x][z].z);
				glNormal3f(terrain[x][z+1].norx , terrain[x][z+1].nory , terrain[x][z+1].norz); 
				glTexCoord2f(terrain[x][z+1].s, terrain[x][z+1].t );
				glVertex3f(terrain[x][z+1].x, terrain[x][z+1].y, terrain[x][z+1].z);
			}
		}
		glEnd();
	}


	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_FOG);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void DrawCloud()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cloud();                  
	char texture[256][256][3];       
	for(int i=0; i<256; i++){         
		for(int j=0; j<256; j++) 
		{
			float color = map256[ i * 256 + j];
			texture[i][j][0] = color;
			texture[i][j][1] = color;
			texture[i][j][2] = 255;
		}
	}
	char texture2[512][512][3];      
	for(int i=0; i<512; i++){         
		for(int j=0; j<512; j++) 
		{
			texture2[i][j][0]=texture[i%256][j%256][0];
			texture2[i][j][1]=texture[i%256][j%256][1];
			texture2[i][j][2]=texture[i%256][j%256][2];
		}
	}
	unsigned int ID;                                    
	glGenTextures(1, &ID);        
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, texture2);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ID);
	GLUquadricObj *quadPlanet;
	quadPlanet = gluNewQuadric();
	gluQuadricDrawStyle(quadPlanet,GLU_FILL);
	gluQuadricTexture(quadPlanet,GL_TRUE);
	glPushMatrix();
	glTranslatef(0.0f, -150.0f, 0.0f);
	glRotatef(alphaSky,0,1,0);
	gluSphere(quadPlanet,500.0f,320,160);
	GLdouble eqn[4] = {0.0,1.0,0.0,0.0};
	alphaSky+=0.3f;
	glPopMatrix();
	gluDeleteQuadric(quadPlanet);
}

float persistence = 0.45f;

float Noise1(int x, int y)
{
	x = x % 25;
	y = y % 25;
	int n = x + y * 57;
	n = (n<<13) ^ n;
	return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f); 
}

float SmoothNoise_1(int x, int y)
{
	float corners = ( Noise1(x-1, y-1)+Noise1(x+1, y-1)+Noise1(x-1, y+1)+Noise1(x+1, y+1) ) / 16.0f;
	float sides   = ( Noise1(x-1, y)  +Noise1(x+1, y)  +Noise1(x, y-1)  +Noise1(x, y+1) ) /  8.0f;
	float center  =  Noise1(x, y) / 4.0f;
	return corners + sides + center;
}

float Cosine_Interpolate(float a, float b, float x)
{
	double ft = x * 3.1415927;
	double f = (1 - cos(ft)) * 0.5f;

	return  a*(1-f) + b*f;

}

float InterpolatedNoise_1(float x, float y)
{

	int integer_X    = int(x);
	float fractional_X = x - integer_X;

	int integer_Y    = int(y);
	float fractional_Y = y - integer_Y;

	float v1 = SmoothNoise_1(integer_X,     integer_Y);
	float v2 = SmoothNoise_1(integer_X + 1, integer_Y);
	float v3 = SmoothNoise_1(integer_X,     integer_Y + 1);
	float v4 = SmoothNoise_1(integer_X + 1, integer_Y + 1);

	float i1 = Cosine_Interpolate(v1 , v2 , fractional_X);
	float i2 = Cosine_Interpolate(v3 , v4 , fractional_X);

	return Cosine_Interpolate(i1 , i2 , fractional_Y);
}
float PerlinNoise_2D(float x, float y)
{
	float total = 0.0f;
	float p = persistence;
	int n = 3;
	for(int i=0;i<=n;i++)
	{
		float frequency = pow((float)2,i);
		float amplitude = pow(p,i);

		total = total + InterpolatedNoise_1(x * frequency, y * frequency) * amplitude;
	}

	return total;
} 

BOOL BuildTexture(char *szPathName, GLuint &texid)
{
	HDC      hdcTemp;                        // The DC To Hold Our Bitmap
	HBITMAP    hbmpTemp;                        // Holds The Bitmap Temporarily
	IPicture  *pPicture;                        // IPicture Interface
	OLECHAR    wszPath[MAX_PATH+1];                  // Full Path To Picture (WCHAR)
	char    szPath[MAX_PATH+1];                    // Full Path To Picture
	long    lWidth;                          // Width In Logical Units
	long    lHeight;                        // Height In Logical Units
	long    lWidthPixels;                      // Width In Pixels
	long    lHeightPixels;                      // Height In Pixels
	GLint    glMaxTexDim ;                      // Holds Maximum Texture Size

	if (strstr(szPathName, "http://"))                  // If PathName Contains http:// Then
	{
		strcpy(szPath, szPathName);                    // Append The PathName To szPath
	}
	else                                // Otherwise We Are Loading From A File
	{
		GetCurrentDirectory(MAX_PATH, szPath);              // Get Our Working Directory
		strcat(szPath, "\\");                      // Append "\" After The Working Directory
		strcat(szPath, szPathName);                    // Append The PathName
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);    // Convert From ASCII To Unicode
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if(FAILED(hr))                            // If Loading Failed
		return FALSE;                          // Return False

	hdcTemp = CreateCompatibleDC(GetDC(0));                // Create The Windows Compatible Device Context
	if(!hdcTemp)                            // Did Creation Fail?
	{
		pPicture->Release();                      // Decrements IPicture Reference Count
		return FALSE;                          // Return False (Failure)
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);          // Get Maximum Texture Size Supported

	pPicture->get_Width(&lWidth);                    // Get IPicture Width (Convert To Pixels)
	lWidthPixels  = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);                    // Get IPicture Height (Convert To Pixels)
	lHeightPixels  = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// Resize Image To Closest Power Of Two
	if (lWidthPixels <= glMaxTexDim) // Is Image Width Less Than Or Equal To Cards Limit
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
	else // Otherwise Set Width To "Max Power Of Two" That The Card Can Handle
		lWidthPixels = glMaxTexDim;

	if (lHeightPixels <= glMaxTexDim) // Is Image Height Greater Than Cards Limit
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
	else // Otherwise Set Height To "Max Power Of Two" That The Card Can Handle
		lHeightPixels = glMaxTexDim;

	//  Create A Temporary Bitmap
	BITMAPINFO  bi = {0};                        // The Type Of Bitmap We Request
	DWORD    *pBits = 0;                        // Pointer To The Bitmap Bits

	bi.bmiHeader.biSize      = sizeof(BITMAPINFOHEADER);        // Set Structure Size
	bi.bmiHeader.biBitCount    = 32;                  // 32 Bit
	bi.bmiHeader.biWidth    = lWidthPixels;              // Power Of Two Width
	bi.bmiHeader.biHeight    = lHeightPixels;            // Make Image Top Up (Positive Y-Axis)
	bi.bmiHeader.biCompression  = BI_RGB;                // RGB Encoding
	bi.bmiHeader.biPlanes    = 1;                  // 1 Bitplane

	//  Creating A Bitmap This Way Allows Us To Specify Color Depth And Gives Us Imediate Access To The Bits
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);

	if(!hbmpTemp)                            // Did Creation Fail?
	{
		DeleteDC(hdcTemp);                        // Delete The Device Context
		pPicture->Release();                      // Decrements IPicture Reference Count
		return FALSE;                          // Return False (Failure)
	}

	SelectObject(hdcTemp, hbmpTemp);                  // Select Handle To Our Temp DC And Our Temp Bitmap Object

	// Render The IPicture On To The Bitmap
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// Convert From BGR To RGB Format And Add An Alpha Value Of 255
	for(long i = 0; i < lWidthPixels * lHeightPixels; i++)        // Loop Through All Of The Pixels
	{
		BYTE* pPixel  = (BYTE*)(&pBits[i]);              // Grab The Current Pixel
		BYTE temp    = pPixel[0];                  // Store 1st Color In Temp Variable (Blue)
		pPixel[0]    = pPixel[2];                  // Move Red Value To Correct Position (1st)
		pPixel[2]    = temp;                      // Move Temp Value To Correct Blue Position (3rd)

		// This Will Make Any Black Pixels, Completely Transparent    (You Can Hardcode The Value If You Wish)
		if ((pPixel[0]==0) && (pPixel[1]==0) && (pPixel[2]==0))      // Is Pixel Completely Black
			pPixel[3]  = 0;                      // Set The Alpha Value To 0
		else                              // Otherwise
			pPixel[3]  = 255;                      // Set The Alpha Value To 255
	}

	glGenTextures(1, &texid);                      // Create The Texture

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texid);                // Bind To The Texture ID
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);    // (Modify This For The Type Of Filtering You Want)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // (Modify This For The Type Of Filtering You Want)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR );    // (Modify This For The Type Of Filtering You Want)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR ); // (Modify This For The Type Of Filtering You Want)
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, lWidthPixels, lHeightPixels, GL_RGBA, GL_UNSIGNED_BYTE, pBits);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);  // (Modify This If You Want Mipmaps)

	DeleteObject(hbmpTemp);                        // Delete The Object
	DeleteDC(hdcTemp);                          // Delete The Device Context

	pPicture->Release();                        // Decrements IPicture Reference Count

	return TRUE;                            // Return True (All Good)
}


void changeSize(int w1, int h1)
{
	w=w1;
	h=h1;
	if(h == 0)
		h = 1;
	ratio = 1.0f * w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	w=w1;
	h=h1;
	glViewport(0, 0, w, h);
	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}
GLfloat LightPosition[]={0.0f, 900.0f, 900.0f, 1.0f};

int InitGL(GLvoid)                                        // All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                // Black Background
	glClearDepth(10.0f);                                    // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                               // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);    // Really Nice Perspective Calculations
	SetNoise(map32);
	glClearColor (0.047, 0.29, 0.94, 0.0);
	glShadeModel (GL_SMOOTH);  
	glEnable(GL_DEPTH_TEST);  
	GLfloat LightAmbient[]= { 5.0f, 5.0f, 5.0f, 1.0f };
	GLfloat mat_shininess[] ={50.0f};
	GLfloat LightDiffuse[]= { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_specular[]= {10.0f, 10.0f, 10.0f, 1.0f};
	GLfloat LightPosition[]={0.0f, 100.0f, 100.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightDiffuse);
	glEnable(GL_LIGHT1);    

	BuildTexture("grass2.jpg", texgrass);
	BuildTexture("lightspot.jpg", texlightspot);
	BuildTexture("lightspot2.jpg", texlightspot2);
	BuildTexture("logo.jpg", texlightspot3);
	camera.PositionCamera(50.0f, 20.0f, 0.0f, 0.0f, 40.0f, 300.0f, 0.0f, 1.0f, 0.0f); 
	initTerrain();

	return TRUE;                                       
}

CVect3 oldPos;
CVect3 oldView;

void ContrainMove()
{
	CVect3 newPos=camera.Position();
}

void renderScene(void) 
{
	glClearColor(0 , 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // Clear Screen And Depth Buffer
	glLoadIdentity();                                    // Reset The Current Modelview Matrix
	//////// Draw Scene /////////////////////

	camera.Look();
	camera.SetViewByMouse();

	oldPos=camera.Position();
	oldView=camera.Position(); 

	camera.Update();
	ContrainMove();

	EyePosition = camera.Position();

	float fogColor[4] = { 0.15f, 0.15f, 0.15f, 1.0f};



	glFogfv(GL_FOG_COLOR, fogColor);    
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_DENSITY, 0.2f);
	glHint(GL_FOG_HINT, GL_DONT_CARE); 
	glFogf(GL_FOG_START, 50.0f);
	glFogf(GL_FOG_END, 800.0f);
	glEnable(GL_FOG);
	glColor3f(0.6f, 0.6f, 0.9f);


	glEnable(GL_LIGHTING);
	glPushMatrix();
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);

	DrawCloud();

	DrawTerrain();
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslated(xpos,ypos,zpos);
	glPopMatrix();

	glutSwapBuffers();
}

void myIdle(void)
{
	//Sleep(100);
	if (isAdd)
		cover+=1.0;
	else
		cover-=1.0;
	if (cover>=40.0)
		isAdd=false;
	else if (cover<=1.0f)
	{
		isAdd=true;
	}
	renderScene();
}


void pressKey(int key, int x1, int y1)
{
	switch (key)
	{
	case GLUT_KEY_LEFT : 
		break;
	case GLUT_KEY_RIGHT : 
		break;
	case GLUT_KEY_UP : 
		break;
	case GLUT_KEY_DOWN : 
		break;
	case GLUT_KEY_F1 :
		break;
	case GLUT_KEY_F2 : 
		break;

	}
}

void releaseKey(int key, int x, int y)
{

	switch (key) 
	{
	case GLUT_KEY_LEFT : 
	case GLUT_KEY_RIGHT : 
		break;
	case GLUT_KEY_UP : 
	case GLUT_KEY_DOWN : 
		break;
	case GLUT_KEY_F1: 
	case GLUT_KEY_F2: 
		break;
	}
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) 
		exit(0);
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1920,1080);

	glutCreateWindow("My OpenGL");
	InitGL();

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	glutDisplayFunc(myIdle);
	glutIdleFunc(myIdle);

	glutReshapeFunc(changeSize);

	glutMainLoop();

	return(0);
}
