/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
//Method declaration
void matrixMul(GzMatrix a, GzMatrix b, GzMatrix c);
void normalize(GzCoord v);
void calculateNewCood(GzCoord* co, GzCoord* newCo, GzMatrix m);
short	ctoi(float color);	
// new struct vertex
typedef struct
{
	float x;
	float y;
	float z;
} vertex;

//new struct edge
typedef struct
{
	vertex s;
	vertex e;
	vertex c;
	float slopex;
	float slopez;
} edge;

//new struct span
typedef struct
{
	vertex s;
	vertex e;
	vertex c;
	float slopez;
} span;


//From HW2
void passValue(GzCoord* co, vertex* v, int indexco, int indexv)
{
	v[indexv].x=co[indexco][0];
	v[indexv].y=co[indexco][1];
	v[indexv].z=co[indexco][2];
}

void sortByY(GzCoord* co, vertex* v)
{
	float* temp=new float[3];
	if(co[0][1]>=co[1][1] && co[0][1]>=co[2][1])
	{
		passValue(co,v,0,2);
		if(co[1][1]>=co[2][1])
		{	
			passValue(co,v,1,1);
			passValue(co,v,2,0);
			return;
		}
		else
		{
			passValue(co,v,1,0);
			passValue(co,v,2,1);
			return;
		}
	}
	if(co[1][1]>=co[0][1] && co[1][1]>=co[2][1])
	{
		passValue(co,v,1,2);
		if(co[0][1]>=co[2][1])
		{	
			passValue(co,v,0,1);
			passValue(co,v,2,0);
			return;
		}
		else
		{
			passValue(co,v,0,0);
			passValue(co,v,2,1);
			return;
		}
	}
	if(co[2][1]>=co[1][1] && co[2][1]>=co[0][1])
	{
		passValue(co,v,2,2);
		if(co[1][1]>=co[0][1])
		{	
			passValue(co,v,1,1);
			passValue(co,v,0,0);
			return;
		}
		else
		{
			passValue(co,v,1,0);
			passValue(co,v,0,1);
			return;
		}
	}

}

void initEdge( edge* e, vertex* v, int* triType)
{
	/*
	if (v[0].y==v[1].y) // point down
	{
		*triType=0;
		if(v[0].x<v[1].x)
		{	
			e[0].s=v[0];
			e[0].e=v[1];
			e[1].s=v[1];
			e[1].e=v[2];
			e[2].s=v[0];
			e[2].e=v[2];
		}
		else
		{
			e[0].s=v[1];
			e[0].e=v[0];
			e[1].s=v[0];
			e[1].e=v[2];
			e[2].s=v[1];
			e[2].e=v[2];
		}
		return;
	}
	//point up
	if(v[1].y==v[2].y)
	{
		*triType=1;
		if(v[1].x<v[2].x)
		{
			e[0].s=v[0];
			e[0].e=v[1];
			e[1].s=v[1];
			e[1].e=v[2];
			e[2].s=v[0];
			e[2].e=v[2];
		}
		else
		{
			e[0].s=v[0];
			e[0].e=v[2];
			e[1].s=v[2];
			e[1].e=v[1];
			e[2].s=v[0];
			e[2].e=v[1];
		}
		return;
	}*/
	e[0].s=v[0];
	e[0].e=v[1];
	e[1].s=v[1];
	e[1].e=v[2];
	e[2].s=v[0];
	e[2].e=v[2];
	float slopex01=(v[1].x-v[0].x)/(v[1].y-v[0].y);
	e[0].slopex=slopex01;
	float slopex02=(v[2].x-v[0].x)/(v[2].y-v[0].y);
	e[2].slopex=slopex02;
	if(slopex01>slopex02)
		*triType=3;
	if(slopex02>slopex01)
		*triType=2;
	return;
}

void goRender(GzRender* render, vertex* v, edge* e, int triType)
{
	float deltaY=ceil(v[0].y)-v[0].y;
	// init slopeX and slopeZ for each edge
	float slopex0 = e[0].slopex;
	float slopex1 = (e[1].e.x-e[1].s.x)/(e[1].e.y-e[1].s.y);
	float slopex2	= e[2].slopex;
	
	float slopez0 = (e[0].e.z-e[0].s.z)/(e[0].e.y-e[0].s.y);
	float slopez1 = (e[1].e.z-e[1].s.z)/(e[1].e.y-e[1].s.y);
	float slopez2 = (e[2].e.z-e[2].s.z)/(e[2].e.y-e[2].s.y);
	// init e[0] e[2]
	e[0].c.x=e[0].s.x+slopex0*deltaY;
	e[0].c.y=e[0].s.y+deltaY;
	e[0].c.z=e[0].s.z+slopez0*deltaY;

	e[2].c.x=e[2].s.x+slopex2*deltaY;
	e[2].c.y=e[2].s.y+deltaY;
	e[2].c.z=e[2].s.z+slopez2*deltaY;

	deltaY=ceil(v[1].y)-v[1].y;
	e[1].c.x=e[1].s.x+slopex1*deltaY;
	e[1].c.y=e[1].s.y+deltaY;
	e[1].c.z=e[1].s.z+slopez1*deltaY;
	// init span
	span sp;
	float slopez=0.0;
	float deltaX=0.0;
	// e[0] to e[2] or e[2] to e[0]
	while (e[0].c.y<e[0].e.y)
	{
		if(triType==3)	//point right
		{
			sp.s=e[2].c;
			sp.e=e[0].c;
			deltaX=ceil(e[2].c.x)-e[2].c.x;
			slopez=(e[2].c.z-e[0].c.z)/(e[2].c.x-e[0].c.x);
			sp.c.x=e[2].c.x+deltaX;
			sp.c.y=e[2].c.y;
			sp.c.z=e[2].c.z+deltaX*slopez;
		}
		if(triType==2) //point left
		{
			sp.s=e[0].c;
			sp.e=e[2].c;
			deltaX=ceil(e[0].c.x)-e[0].c.x;
			slopez=(e[0].c.z-e[2].c.z)/(e[0].c.x-e[2].c.x);
			sp.c.x=e[0].c.x+deltaX;
			sp.c.y=e[0].c.y;
			sp.c.z=e[0].c.z+deltaX*slopez;
		}
		while (sp.c.x<sp.e.x)
		{
			int i=sp.c.x;
			int j=sp.c.y;
			GzIntensity r,g,b,a;
			GzDepth z;
			GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
			if(z==0 || sp.c.z<z)
			{
				GzPutDisplay(render->display, i, j, ctoi(render->flatcolor[0]), ctoi(render->flatcolor[1]), ctoi(render->flatcolor[2]), 0, (GzDepth)sp.c.z);
			}
			sp.c.x++;
			sp.c.z+=slopez;
		}
		e[0].c.y++;
		e[2].c.y++;
		e[0].c.x+=slopex0;
		e[0].c.z+=slopez0;
		e[2].c.x+=slopex2;
		e[2].c.z+=slopez2;
	}
	while(e[1].c.y<e[1].e.y)
	{
		if(triType==3)//point right
		{
			sp.s=e[2].c;
			sp.e=e[1].c;
			deltaX=ceil(e[2].c.x)-e[2].c.x;
			slopez=(e[2].c.z-e[1].c.z)/(e[2].c.x-e[1].c.x);
			sp.c.x=e[2].c.x+deltaX;
			sp.c.y=e[2].c.y;
			sp.c.z=e[2].c.z+deltaX*slopez;
		}
		if(triType==2) //point left
		{
			sp.s=e[1].c;
			sp.e=e[2].c;
			deltaX=ceil(e[1].c.x)-e[1].c.x;
			slopez=(e[1].c.z-e[2].c.z)/(e[1].c.x-e[2].c.x);
			sp.c.x=e[1].c.x+deltaX;
			sp.c.y=e[1].c.y;
			sp.c.z=e[1].c.z+deltaX*slopez;
		}
		while (sp.c.x<sp.e.x)
		{
			int i=sp.c.x;
			int j=sp.c.y;
			GzIntensity r,g,b,a;
			GzDepth z;
			GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
			if(z==0 || sp.c.z<z)
			{
				GzPutDisplay(render->display, i, j, ctoi(render->flatcolor[0]), ctoi(render->flatcolor[1]), ctoi(render->flatcolor[2]), 0, (GzDepth)sp.c.z);
			}
			sp.c.x++;
			sp.c.z+=slopez;
		}
		e[1].c.y++;
		e[2].c.y++;
		e[1].c.x+=slopex1;
		e[1].c.z+=slopez1;
		e[2].c.x+=slopex2;
		e[2].c.z+=slopez2;
	}
}

//HW3
int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	float rad=2*PI*degree/360;
	memset(mat,0,sizeof(mat));
	mat[0][0]=1;
	mat[3][3]=1;
	mat[1][1]=cos(rad);
	mat[1][2]=-sin(rad);
	mat[2][2]=cos(rad);
	mat[2][1]=sin(rad);
	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	float rad=2*PI*degree/360;
	memset(mat,0,sizeof(mat));
	mat[1][1]=1;
	mat[3][3]=1;
	mat[0][0]=cos(rad);
	mat[0][2]=sin(rad);
	mat[2][2]=cos(rad);
	mat[2][0]=-sin(rad);

	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	float rad=2*PI*degree/360;
	memset(mat,0,sizeof(mat));
	mat[2][2]=1;
	mat[3][3]=1;
	mat[0][0]=cos(rad);
	mat[1][1]=cos(rad);
	mat[1][0]=sin(rad);
	mat[0][1]=-sin(rad);
	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	memset(mat,0,sizeof(mat));
	mat[0][0]=1;
	mat[1][1]=1;
	mat[2][2]=1;
	mat[3][3]=1;
	mat[0][3] = translate[0];
	mat[1][3] = translate[1];
	mat[2][3] = translate[2];
	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	memset(mat,0,sizeof(mat));
	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- keep closed until all inits are done 
- setup Xsp and anything only done once 
- span interpolator needs pointer to display 
- check for legal class GZ_Z_BUFFER_RENDER 
- init default camera 
*/ 
	*render=new GzRender;
	(**render).renderClass=renderClass;
	(**render).display=display;
	(**render).open=0;
	(**render).matlevel = 0;
	//init each element in matrix as 0
	memset((**render).Ximage,0,sizeof((**render).Ximage));
	memset((**render).Xsp,0,sizeof((**render).Xsp));
	memset((**render).camera.Xiw,0,sizeof((**render).camera.Xiw));
	memset((**render).camera.Xpi,0,sizeof((**render).camera.Xpi));
	//For stack push
	(**render).Ximage[0][0][0]=1;
	(**render).Ximage[0][1][1]=1;
	(**render).Ximage[0][2][2]=1;
	(**render).Ximage[0][3][3]=1;
	//init Xsp
	(**render).Xsp[0][0] = (**render).Xsp[0][3] = (**render).display->xres/2;
	(**render).Xsp[1][1] = -((**render).display->yres/2);
	(**render).Xsp[1][3] = (**render).display->yres/2;
	float d=1/(tan(PI*((**render).camera.FOV)/360));
	(*render)->Xsp[2][2] =10000;
	(*render)->Xsp[3][3] = 1.0;
	//Camera parameters
	(**render).camera.FOV=DEFAULT_FOV;
	(**render).camera.lookat[X]=0;
	(**render).camera.lookat[Y]=0;
	(**render).camera.lookat[Z]=0;

	(**render).camera.position[X]=DEFAULT_IM_X;
	(**render).camera.position[Y]=DEFAULT_IM_Y;
	(**render).camera.position[Z]=DEFAULT_IM_Z;

	(**render).camera.worldup[X]=0;
	(**render).camera.worldup[Y]=1;
	(**render).camera.worldup[Z]=0;

	return GZ_SUCCESS;

}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	delete render->display;
	delete render;
	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
/*  
- set up for start of each frame - clear frame buffer 
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms if it want to. 
*/ 
	GzInitDisplay(render->display);
	//put Xsp at base of stack
	GzPushMatrix(render, render->Xsp);
	//Xiw init
	//cameraZ
	GzCoord cl, cameraZ;
	cl[X] = render->camera.lookat[X] - render->camera.position[X];
	cl[Y] = render->camera.lookat[Y] - render->camera.position[Y];
	cl[Z] = render->camera.lookat[Z] - render->camera.position[Z];
	cameraZ[X] = cl[X];
	cameraZ[Y] = cl[Y];
	cameraZ[Z] = cl[Z];
	normalize(cameraZ);
	//cameraY
	GzCoord Up, cameraY;
	float product = render->camera.worldup[X]*cameraZ[X] + render->camera.worldup[Y]*cameraZ[Y] +
		render->camera.worldup[Z]*cameraZ[Z];
	Up[X] = render->camera.worldup[X] - product*cameraZ[X];
	Up[Y] = render->camera.worldup[Y] - product*cameraZ[Y];
	Up[Z] = render->camera.worldup[Z] - product*cameraZ[Z];
	cameraY[X] = Up[X];
	cameraY[Y] = Up[Y];
	cameraY[Z] = Up[Z];
	normalize(cameraY);
	//cameraX
	GzCoord cameraX;
	cameraX[X] = cameraY[Y]*cameraZ[Z] - cameraY[Z]*cameraZ[Y];
	cameraX[Y] = cameraY[Z]*cameraZ[X] - cameraY[X]*cameraZ[Z];
	cameraX[Z] = cameraY[X]*cameraZ[Y] - cameraY[Y]*cameraZ[X];
	normalize(cameraX);
	//init Xiw matrix
	render->camera.Xiw[0][0] = cameraX[X];
	render->camera.Xiw[0][1] = cameraX[Y];
	render->camera.Xiw[0][2] = cameraX[Z];
	render->camera.Xiw[0][3] = -(cameraX[X]*render->camera.position[X] + cameraX[Y]*render->camera.position[Y] + 
		cameraX[Z]*render->camera.position[Z]);
	render->camera.Xiw[1][0] = cameraY[X];
	render->camera.Xiw[1][1] = cameraY[Y];
	render->camera.Xiw[1][2] = cameraY[Z];
	render->camera.Xiw[1][3] = -(cameraY[X]*render->camera.position[X] + cameraY[Y]*render->camera.position[Y]+ 
		cameraY[Z]*render->camera.position[Z]);
	render->camera.Xiw[2][0] = cameraZ[X];
	render->camera.Xiw[2][1] = cameraZ[Y];
	render->camera.Xiw[2][2] = cameraZ[Z];
	render->camera.Xiw[2][3] = -(cameraZ[X]*render->camera.position[X] + cameraZ[Y]*render->camera.position[Y]+ 
		cameraZ[Z]*render->camera.position[Z]);
	render->camera.Xiw[3][3] = 1;
	//init Xpi
	float d=1/(tan(PI*(render->camera.FOV)/360));
	render->camera.Xpi[0][0] = 1;
	render->camera.Xpi[1][1] = 1;
	render->camera.Xpi[2][2] = 1;
	render->camera.Xpi[3][3] = 1;
	render->camera.Xpi[3][2] = 1 / d;
	//push Xpi, then push Xiw
	GzPushMatrix(render, render->camera.Xpi);
	GzPushMatrix(render, render->camera.Xiw);
	//Open render
	render->open=1;
	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	render->camera.FOV = camera->FOV;
	render->camera.lookat[X] = camera->lookat[X];
	render->camera.lookat[Y] = camera->lookat[Y];
	render->camera.lookat[Z] = camera->lookat[Z];
	render->camera.position[X] = camera->position[X];
	render->camera.position[Y] = camera->position[Y];
	render->camera.position[Z] = camera->position[Z];
	render->camera.worldup[X] = camera->worldup[X];
	render->camera.worldup[Y] = camera->worldup[Y];
	render->camera.worldup[Z] = camera->worldup[Z];
	return GZ_SUCCESS;
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	render->matlevel++;
	if(render->matlevel>=MATLEVELS)
		return GZ_FAILURE;
	matrixMul(render->Ximage[render->matlevel-1], matrix, render->Ximage[render->matlevel]);
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if(render->matlevel<=0)
		return GZ_FAILURE;
	render->matlevel--;
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	for (int i=0; i<numAttributes; i++)
	{
		if (nameList[i]==GZ_RGB_COLOR)
		{
			GzColor* c=(GzColor*)valueList[i];
			render->flatcolor[0]=c[i][0];
			render->flatcolor[1]=c[i][1];
			render->flatcolor[2]=c[i][2];
		}
	}
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, 
				  GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts  
- Clip - just discard any triangle with verts behind view plane 
       - test for triangles with all three verts off-screen 
- invoke triangle rasterizer  
*/ 
	for(int i=0;i< numParts; i++)
	{
		if(nameList[i]==GZ_POSITION)
		{
			GzCoord* co=(GzCoord*) valueList[i];
			GzCoord* newCo=new GzCoord[3];
			int flag=0;
			calculateNewCood(co, newCo, render->Ximage[render->matlevel]);
			for (int j=0;j<2;j++)
			{
				if (newCo[i][2]<render->camera.position[Z])
				{
					flag=1;
				}
			}
			if (flag!=1)
			{
				vertex* v=new vertex[3];
				sortByY(newCo, v);
				edge* e=new edge[3];
				int triType;
				initEdge(e, v, &triType);
				goRender(render, v ,e, triType);
			}
		}
	}
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

void matrixMul(GzMatrix a, GzMatrix b, GzMatrix c)
{

	for (int i = 0; i < 4; i++)
	{
		for (int j =  0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				c[i][j] = c[i][j] + a[i][k] * b[k][j];
			}
		}
	}
}

void normalize(GzCoord v)
{
	float len = sqrt((v[X]*v[X]) + (v[Y]*v[Y]) + (v[Z]*v[Z]));
	v[X] = v[X] / len;
	v[Y] = v[Y] / len;
	v[Z] = v[Z] / len;

}

void calculateNewCood(GzCoord* co, GzCoord* newCo, GzMatrix m)
{
	float W;
	for (int j = 0; j < 3; ++j)
	{
		newCo[j][X] = m[0][0]*co[j][X] + m[0][1]*co[j][Y] + m[0][2]*co[j][Z]
		+ m[0][3]*1;
		newCo[j][Y] = m[1][0]*co[j][X] + m[1][1]*co[j][Y] + m[1][2]*co[j][Z]
		+ m[1][3]*1;
		newCo[j][Z] = m[2][0]*co[j][X] + m[2][1]*co[j][Y] + m[2][2]*co[j][Z]
		+ m[2][3]*1;
		W = m[3][0]*co[j][X] + m[3][1]*co[j][Y] + m[3][2]*co[j][Z]
		+ m[3][3]*1;
		newCo[j][X] /= W;
		newCo[j][Y] /= W;
		newCo[j][Z] /= W;
	}
		
}