#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"gz.h"
#include	"rend.h"
#include	"disp.h"

using namespace std;
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

/* NOT part of API - just for general assistance */
short	ctoi(float color)		/* convert float color to GzIntensity short */
{
	return(short)((int)(color * ((1 << 12) - 1)));
}

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

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay *display)
{
/* 
- malloc a renderer struct
- keep closed until BeginRender inits are done
- span interpolator needs pointer to display for pixel writes
- check for legal class GZ_Z_BUFFER_RENDER
*/
	*render = new GzRender;
	(*render)->renderClass = renderClass;
	(*render)->display = display;
	(*render)->open = 0;
	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	delete render;
	return GZ_SUCCESS;
}


int GzBeginRender(GzRender	*render)
{
/* 
- set up for start of each frame - init frame buffer
*/
	GzInitDisplay(render->display);
	render->open = 1;
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	for (int i = 0; i < numAttributes; i++)
	{
		if(nameList[i] == GZ_RGB_COLOR)
		{
			GzColor* co= (GzColor*)(valueList[0]);
			render->flatcolor[0] = co[0][0];
			render->flatcolor[1] = co[0][1];
			render->flatcolor[2] = co[0][2];
		}
	}
	return GZ_SUCCESS;
}


int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList) 
/* numParts - how many names and values */
{
/* 
- pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
- Invoke the scan converter and return an error code
*/
	for(int i=0;i< numParts; i++)
	{
		if(nameList[i]==GZ_POSITION)
		{
			GzCoord* co=(GzCoord*) valueList[i];
			vertex* v=new vertex[3];
			sortByY(co, v);
			edge* e=new edge[3];
			int triType;
			initEdge(e, v, &triType);
			goRender(render, v ,e, triType);
		}
	}
	return GZ_SUCCESS;
}


