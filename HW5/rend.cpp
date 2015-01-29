/* CS580 Homework 4 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
//HW3 Method declaration
void matrixMul(GzMatrix a, GzMatrix b, GzMatrix c);
void normalize(GzCoord v);
void calculateNewCood(GzCoord* co, GzCoord* newCo, GzMatrix m);
int GzPushNormalMatrix(GzRender* render, GzMatrix matrix);
short	ctoi(float color);	
// new struct vertex
typedef struct
{
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	float normalX;
	float normalY;
	float normalZ;
	float tx;
	float ty;
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

void passNormalValue(GzCoord* normalValue, vertex* v, int indexco, int indexv)
{
	v[indexv].normalX=normalValue[indexco][0];
	v[indexv].normalY=normalValue[indexco][1];
	v[indexv].normalZ=normalValue[indexco][2];
}

void passTextureValue(GzTextureIndex* newTexture, vertex* v, int indexco, int indexv)
{
	v[indexv].tx=newTexture[indexco][0];
	v[indexv].ty=newTexture[indexco][1];
}

void sortByY(GzCoord* co, vertex* v, GzCoord* normalValue, GzTextureIndex*  newTexture)
{
	float* temp=new float[3];
	if(co[0][1]>=co[1][1] && co[0][1]>=co[2][1])
	{
		passValue(co,v,0,2);
		passNormalValue(normalValue,v,0,2);
		passTextureValue(newTexture,v,0,2);
		if(co[1][1]>=co[2][1])
		{	
			passValue(co,v,1,1);
			passValue(co,v,2,0);
			passNormalValue(normalValue,v,1,1);
			passNormalValue(normalValue,v,2,0);
			passTextureValue(newTexture,v,1,1);
			passTextureValue(newTexture,v,2,0);
			return;
		}
		else
		{
			passValue(co,v,1,0);
			passValue(co,v,2,1);
			passNormalValue(normalValue,v,1,0);
			passNormalValue(normalValue,v,2,1);
			passTextureValue(newTexture,v,1,0);
			passTextureValue(newTexture,v,2,1);
			return;
		}
	}
	if(co[1][1]>=co[0][1] && co[1][1]>=co[2][1])
	{
		passValue(co,v,1,2);
		passNormalValue(normalValue,v,1,2);
		passTextureValue(newTexture,v,1,2);
		if(co[0][1]>=co[2][1])
		{	
			passValue(co,v,0,1);
			passValue(co,v,2,0);
			passNormalValue(normalValue,v,0,1);
			passNormalValue(normalValue,v,2,0);
			passTextureValue(newTexture,v,0,1);
			passTextureValue(newTexture,v,2,0);
			return;
		}
		else
		{
			passValue(co,v,0,0);
			passValue(co,v,2,1);
			passNormalValue(normalValue,v,0,0);
			passNormalValue(normalValue,v,2,1);
			passTextureValue(newTexture,v,0,0);
			passTextureValue(newTexture,v,2,1);
			return;
		}
	}
	if(co[2][1]>=co[1][1] && co[2][1]>=co[0][1])
	{
		passValue(co,v,2,2);
		passNormalValue(normalValue,v,2,2);
		passTextureValue(newTexture,v,2,2);
		if(co[1][1]>=co[0][1])
		{	
			passValue(co,v,1,1);
			passValue(co,v,0,0);
			passNormalValue(normalValue,v,1,1);
			passNormalValue(normalValue,v,0,0);
			passTextureValue(newTexture,v,1,1);
			passTextureValue(newTexture,v,0,0);
			return;
		}
		else
		{
			passValue(co,v,1,0);
			passValue(co,v,0,1);
			passNormalValue(normalValue,v,1,0);
			passNormalValue(normalValue,v,0,1);
			passTextureValue(newTexture,v,1,0);
			passTextureValue(newTexture,v,0,1);
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
	if(slopex01>=slopex02)
		*triType=3;
	if(slopex02>=slopex01)
		*triType=2;
	return;
}

float dotMul(GzCoord a, GzCoord b)
{
	return (float)(a[X]*b[X]+a[Y]*b[Y]+a[Z]*b[Z]);
}

void Coloring(GzRender* render, vertex v, GzColor color)
{
	GzCoord eye={0,0,-1};
	GzCoord Specular={0,0,0};
	GzCoord Didffuse={0,0,0};
	GzCoord Reflect;
	GzCoord newNorm={v.normalX, v.normalY, v.normalZ};
	for (int i=0; i<render->numlights; i++)
	{
		float NdotL=dotMul(render->lights[i].direction, newNorm);
		float NdotE=dotMul(eye, newNorm);
		if (NdotE*NdotL>0)
		{
			if (NdotL< 0 && NdotL < 0)
			{
				newNorm[X]=-newNorm[X];
				newNorm[Y]=-newNorm[Y];
				newNorm[Z]=-newNorm[Z];
				NdotL=-NdotL;
			}
			//Specular Light
			Reflect[X]=2*NdotL*newNorm[X] - render->lights[i].direction[X];
			Reflect[Y]=2*NdotL*newNorm[Y] - render->lights[i].direction[Y];
			Reflect[Z]=2*NdotL*newNorm[Z] - render->lights[i].direction[Z];
			normalize(Reflect);
			float RdotE=dotMul(Reflect, eye);
			//Keep R[0,1]
			if (RdotE < 0)
				RdotE = 0;
			if (RdotE > 1)
				RdotE = 1;
			RdotE=pow(RdotE, render->spec);
			Specular[RED]+=(render->lights[i].color[RED]*RdotE);
			Specular[GREEN]+=(render->lights[i].color[GREEN]*RdotE);
			Specular[BLUE]+=(render->lights[i].color[BLUE]*RdotE);
			//Diffuse Light
			Didffuse[RED]+=(render->lights[i].color[RED]*NdotL);
			Didffuse[GREEN]+=(render->lights[i].color[GREEN]*NdotL);
			Didffuse[BLUE]+=(render->lights[i].color[BLUE]*NdotL);
		}
	}
	if (render->interp_mode==GZ_NORMALS)
	{
		color[RED] = render->Ks[0] * Specular[RED]+ render->Kd[0] * Didffuse[RED] + render->Ka[0] * render->ambientlight.color[RED];
		color[GREEN] = render->Ks[1] * Specular[GREEN]+ render->Kd[1] * Didffuse[GREEN] + render->Ka[1] * render->ambientlight.color[GREEN];
		color[BLUE] = render->Ks[2] * Specular[BLUE]+ render->Kd[2] * Didffuse[BLUE] + render->Ka[2] * render->ambientlight.color[BLUE];
	}
	if (render->interp_mode == GZ_COLOR)
	{
		color[RED] = Specular[RED] + Didffuse[RED] + render->ambientlight.color[RED];
		color[GREEN] = Specular[GREEN] + Didffuse[GREEN] + render->ambientlight.color[GREEN];
		color[BLUE] = Specular[BLUE] + Didffuse[BLUE] + render->ambientlight.color[BLUE];
	}
	for(int i=0; i<3; i++)
	{
		if (color[i]<0)
			color[i]=0;
		if (color[i]>1)
			color[i]=1;
	}
}

void goRender(GzRender* render, vertex* v, edge* e, int triType)
{
	e[0].s.r=v[0].r;
	e[0].s.g=v[0].g;
	e[0].s.b=v[0].b;
	e[0].e.r=v[1].r;
	e[0].e.g=v[1].g;
	e[0].e.b=v[1].b;
	e[1].s.r=v[1].r;
	e[1].s.g=v[1].g;
	e[1].s.b=v[1].b;
	e[1].e.r=v[2].r;
	e[1].e.g=v[2].g;
	e[1].e.b=v[2].b;
	e[2].s.r=v[0].r;
	e[2].s.g=v[0].g;
	e[2].s.b=v[0].b;
	e[2].e.r=v[2].r;
	e[2].e.g=v[2].g;
	e[2].e.b=v[2].b;
	float deltaY=ceil(v[0].y)-v[0].y;
	// init slopeX and slopeZ for each edge
	float slopex0 = e[0].slopex;
	float slopex1 = (e[1].e.x-e[1].s.x)/(e[1].e.y-e[1].s.y);
	float slopex2	= e[2].slopex;
	//set normal slope and RGB slope
	float slopeNormalX0=(e[0].e.normalX-e[0].s.normalX)/(e[0].e.y-e[0].s.y);
	float slopeNormalX1=(e[1].e.normalX-e[1].s.normalX)/(e[1].e.y-e[1].s.y);
	float slopeNormalX2=(e[2].e.normalX-e[2].s.normalX)/(e[2].e.y-e[2].s.y);
	float slopeNormalY0=(e[0].e.normalY-e[0].s.normalY)/(e[0].e.y-e[0].s.y);
	float slopeNormalY1=(e[1].e.normalY-e[1].s.normalY)/(e[1].e.y-e[1].s.y);
	float slopeNormalY2=(e[2].e.normalY-e[2].s.normalY)/(e[2].e.y-e[2].s.y);
	float slopeNormalZ0=(e[0].e.normalZ-e[0].s.normalZ)/(e[0].e.y-e[0].s.y);
	float slopeNormalZ1=(e[1].e.normalZ-e[1].s.normalZ)/(e[1].e.y-e[1].s.y);
	float slopeNormalZ2=(e[2].e.normalZ-e[2].s.normalZ)/(e[2].e.y-e[2].s.y);
	float slopeR0=(e[0].e.r-e[0].s.r)/(e[0].e.y-e[0].s.y);
	float slopeR1=(e[1].e.r-e[1].s.r)/(e[1].e.y-e[1].s.y);
	float slopeR2=(e[2].e.r-e[2].s.r)/(e[2].e.y-e[2].s.y);
	float slopeG0=(e[0].e.g-e[0].s.g)/(e[0].e.y-e[0].s.y);
	float slopeG1=(e[1].e.g-e[1].s.g)/(e[1].e.y-e[1].s.y);
	float slopeG2=(e[2].e.g-e[2].s.g)/(e[2].e.y-e[2].s.y);
	float slopeB0=(e[0].e.b-e[0].s.b)/(e[0].e.y-e[0].s.y);
	float slopeB1=(e[1].e.b-e[1].s.b)/(e[1].e.y-e[1].s.y);
	float slopeB2=(e[2].e.b-e[2].s.b)/(e[2].e.y-e[2].s.y);

	float slopez0 = (e[0].e.z-e[0].s.z)/(e[0].e.y-e[0].s.y);
	float slopez1 = (e[1].e.z-e[1].s.z)/(e[1].e.y-e[1].s.y);
	float slopez2 = (e[2].e.z-e[2].s.z)/(e[2].e.y-e[2].s.y);
	//texture
	float slopetx0=(e[0].e.tx-e[0].s.tx)/(e[0].e.y-e[0].s.y);
	float slopetx1=(e[1].e.tx-e[1].s.tx)/(e[1].e.y-e[1].s.y);
	float slopetx2=(e[2].e.tx-e[2].s.tx)/(e[2].e.y-e[2].s.y);
	
	float slopety0=(e[0].e.ty-e[0].s.ty)/(e[0].e.y-e[0].s.y);
	float slopety1=(e[1].e.ty-e[1].s.ty)/(e[1].e.y-e[1].s.y);
	float slopety2=(e[2].e.ty-e[2].s.ty)/(e[2].e.y-e[2].s.y);
	// init e[0] e[2]
	e[0].c.x=e[0].s.x+slopex0*deltaY;
	e[0].c.y=e[0].s.y+deltaY;
	e[0].c.z=e[0].s.z+slopez0*deltaY;

	e[0].c.normalX=e[0].s.normalX+(slopeNormalX0*deltaY);
	e[0].c.normalY=e[0].s.normalY+(slopeNormalY0*deltaY);
	e[0].c.normalZ=e[0].s.normalZ+(slopeNormalZ0*deltaY);
	e[0].c.r =e[0].s.r+(slopeR0*deltaY);
	e[0].c.g=e[0].s.g+(slopeG0*deltaY);
	e[0].c.b=e[0].s.b+(slopeB0*deltaY);
	//texture
	e[0].c.tx=e[0].s.tx+slopetx0*deltaY;
	e[2].c.tx=e[2].s.tx+slopetx2*deltaY;
	e[0].c.ty=e[0].s.ty+slopety0*deltaY;
	e[2].c.ty=e[2].s.ty+slopety2*deltaY;

	e[2].c.x=e[2].s.x+slopex2*deltaY;
	e[2].c.y=e[2].s.y+deltaY;
	e[2].c.z=e[2].s.z+slopez2*deltaY;

	e[2].c.normalX=e[2].s.normalX+(slopeNormalX2*deltaY);
	e[2].c.normalY=e[2].s.normalY+(slopeNormalY2*deltaY);
	e[2].c.normalZ=e[2].s.normalZ+(slopeNormalZ2*deltaY);
	e[2].c.r =e[2].s.r+(slopeR2*deltaY);
	e[2].c.g=e[2].s.g+(slopeG2*deltaY);
	e[2].c.b=e[2].s.b+(slopeB2*deltaY);

	deltaY=ceil(v[1].y)-v[1].y;
	e[1].c.x=e[1].s.x+slopex1*deltaY;
	e[1].c.y=e[1].s.y+deltaY;
	e[1].c.z=e[1].s.z+slopez1*deltaY;

	e[1].c.normalX=e[1].s.normalX+(slopeNormalX1*deltaY);
	e[1].c.normalY=e[1].s.normalY+(slopeNormalY1*deltaY);
	e[1].c.normalZ=e[1].s.normalZ+(slopeNormalZ1*deltaY);
	e[1].c.r =e[1].s.r+(slopeR1*deltaY);
	e[1].c.g=e[1].s.g+(slopeG1*deltaY);
	e[1].c.b=e[1].s.b+(slopeB1*deltaY);
	//texture
	e[1].c.tx=e[1].s.tx+slopetx1*deltaY;
	e[1].c.ty=e[1].s.ty+slopety1*deltaY;
	// init span
	span sp;
	float slopez=0.0;
	float deltaX=0.0;
	float slopeSpanR=0.0;
	float slopeSpanG=0.0;
	float slopeSpanB=0.0;
	float slopeSpanNormalX=0.0;
	float slopeSpanNormalY=0.0;
	float slopeSpanNormalZ=0.0;
	float slopeSpanTx=0.0;
	float slopeSpanTy=0.0;
	// e[0] to e[2] or e[2] to e[0]
	while (e[0].c.y<e[0].e.y)
	{
		if(triType==3)	//point right
		{
			sp.s=e[2].c;
			sp.e=e[0].c;
			deltaX=ceil(e[2].c.x)-e[2].c.x;
			slopez=(e[0].c.z-e[2].c.z)/(e[0].c.x-e[2].c.x);
			sp.c.x=e[2].c.x+deltaX;
			sp.c.y=e[2].c.y;
			sp.c.z=e[2].c.z+deltaX*slopez;
			//coloring
			slopeSpanNormalX=(e[0].c.normalX-e[2].c.normalX)/(e[0].c.x-e[2].c.x);
			slopeSpanNormalY=(e[0].c.normalY-e[2].c.normalY)/(e[0].c.x-e[2].c.x);
			slopeSpanNormalZ=(e[0].c.normalZ-e[2].c.normalZ)/(e[0].c.x-e[2].c.x);
			slopeSpanR=(e[0].c.r -e[2].c.r)/(e[0].c.x-e[2].c.x);
			slopeSpanG=(e[0].c.g-e[2].c.g)/(e[0].c.x-e[2].c.x);
			slopeSpanB=(e[0].c.b-e[2].c.b)/(e[0].c.x-e[2].c.x);
			sp.c.normalX=e[2].c.normalX+(deltaX*slopeSpanNormalX);
			sp.c.normalY=e[2].c.normalY+(deltaX*slopeSpanNormalY);
			sp.c.normalZ=e[2].c.normalZ+(deltaX*slopeSpanNormalZ);
			sp.c.r=e[2].c.r+(deltaX*slopeSpanR);
			sp.c.g=e[2].c.g+(deltaX*slopeSpanG);
			sp.c.b=e[2].c.b+(deltaX*slopeSpanB);
			//Texture
			slopeSpanTx=(e[0].c.tx-e[2].c.tx)/(e[0].c.x-e[2].c.x);
			slopeSpanTy=(e[0].c.ty-e[2].c.ty)/(e[0].c.x-e[2].c.x);
			sp.c.tx=e[2].c.tx+(deltaX*slopeSpanTx);
			sp.c.ty=e[2].c.ty+(deltaX*slopeSpanTy);
		}
		if(triType==2) //point left
		{
			sp.s=e[0].c;
			sp.e=e[2].c;
			deltaX=ceil(e[0].c.x)-e[0].c.x;
			slopez=(e[2].c.z-e[0].c.z)/(e[2].c.x-e[0].c.x);
			sp.c.x=e[0].c.x+deltaX;
			sp.c.y=e[0].c.y;
			sp.c.z=e[0].c.z+deltaX*slopez;

			//coloring
			slopeSpanNormalX=(e[2].c.normalX-e[0].c.normalX)/(e[2].c.x-e[0].c.x);
			slopeSpanNormalY=(e[2].c.normalY-e[0].c.normalY)/(e[2].c.x-e[0].c.x);
			slopeSpanNormalZ=(e[2].c.normalZ-e[0].c.normalZ)/(e[2].c.x-e[0].c.x);
			slopeSpanR=(e[2].c.r -e[0].c.r)/(e[2].c.x-e[0].c.x);
			slopeSpanG=(e[2].c.g-e[0].c.g)/(e[2].c.x-e[0].c.x);
			slopeSpanB=(e[2].c.b-e[0].c.b)/(e[2].c.x-e[0].c.x);
			sp.c.normalX=e[0].c.normalX+(deltaX*slopeSpanNormalX);
			sp.c.normalY=e[0].c.normalY+(deltaX*slopeSpanNormalY);
			sp.c.normalZ=e[0].c.normalZ+(deltaX*slopeSpanNormalZ);
			sp.c.r=e[0].c.r+(deltaX*slopeSpanR);
			sp.c.g=e[0].c.g+(deltaX*slopeSpanG);
			sp.c.b=e[0].c.b+(deltaX*slopeSpanB);
			//texture
			slopeSpanTx=(e[2].c.tx-e[0].c.tx)/(e[2].c.x-e[0].c.x);
			slopeSpanTy=(e[2].c.ty-e[0].c.ty)/(e[2].c.x-e[0].c.x);
			sp.c.tx=e[0].c.tx+(deltaX*slopeSpanTx);
			sp.c.ty=e[0].c.ty+(deltaX*slopeSpanTy);
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
				if (render->interp_mode == GZ_COLOR)
				{
					float warpz=sp.c.z/(INT_MAX-sp.c.z);
					float u=sp.c.tx*(warpz+1);
					float v=sp.c.ty*(warpz+1);
					GzColor color;
					render->tex_fun(u, v, color);
					color[0]=color[0]*sp.c.r;
					color[1]=color[1]*sp.c.g;
					color[2]=color[2]*sp.c.b;
					GzPutDisplay(render->display, i, j, ctoi(color[0]), ctoi(color[1]), ctoi(color[2]), 0, (GzDepth)sp.c.z);
				}
				if (render->interp_mode == GZ_NORMALS)
				{
					float warpz=sp.c.z/(INT_MAX-sp.c.z);
					float u=sp.c.tx*(warpz+1);
					float v=sp.c.ty*(warpz+1);
					GzColor color;
					render->tex_fun(u, v, color);
					for (int i=0; i<3; i++)
					{
						render->Ka[i]=color[i];
						render->Kd[i]=color[i];
					}
					Coloring(render, sp.c, color);
					GzPutDisplay(render->display, i, j, ctoi(color[0]), ctoi(color[1]), ctoi(color[2]), 0, (GzDepth)sp.c.z);
				}
			
			}
			sp.c.x++;
			sp.c.z+=slopez;
			sp.c.normalX+=slopeSpanNormalX;
			sp.c.normalY+=slopeSpanNormalY;
			sp.c.normalZ+=slopeSpanNormalZ;
			sp.c.r+=slopeSpanR;
			sp.c.g+=slopeSpanG;
			sp.c.b+=slopeSpanB;
			//texture
			sp.c.tx+=slopeSpanTx;
			sp.c.ty+=slopeSpanTy;
		}
		e[0].c.y++;
		e[2].c.y++;
		e[0].c.x+=slopex0;
		e[0].c.z+=slopez0;
		e[2].c.x+=slopex2;
		e[2].c.z+=slopez2;
		e[0].c.normalX+=slopeNormalX0;
		e[0].c.normalY+=slopeNormalY0;
		e[0].c.normalZ+=slopeNormalZ0;
		e[2].c.normalX+=slopeNormalX2;
		e[2].c.normalY+=slopeNormalY2;
		e[2].c.normalZ+=slopeNormalZ2;
		e[0].c.r+=slopeR0;
		e[0].c.g+=slopeG0;
		e[0].c.b+=slopeB0;
		e[2].c.r+=slopeR2;
		e[2].c.g+=slopeG2;
		e[2].c.b+=slopeB2;
		//texture y++
		e[0].c.tx+=slopetx0;
		e[2].c.tx+=slopetx2;
		e[0].c.ty+=slopety0;
		e[2].c.ty+=slopety2;
	}
	while(e[1].c.y<e[1].e.y)
	{
		if(triType==3)//point right
		{
			sp.s=e[2].c;
			sp.e=e[1].c;
			deltaX=ceil(e[2].c.x)-e[2].c.x;
			slopez=(e[1].c.z-e[2].c.z)/(e[1].c.x-e[2].c.x);
			sp.c.x=e[2].c.x+deltaX;
			sp.c.y=e[2].c.y;
			sp.c.z=e[2].c.z+deltaX*slopez;
			//coloring
			slopeSpanNormalX=(e[1].c.normalX-e[2].c.normalX)/(e[1].c.x-e[2].c.x);
			slopeSpanNormalY=(e[1].c.normalY-e[2].c.normalY)/(e[1].c.x-e[2].c.x);
			slopeSpanNormalZ=(e[1].c.normalZ-e[2].c.normalZ)/(e[1].c.x-e[2].c.x);
			slopeSpanR=(e[1].c.r -e[2].c.r)/(e[1].c.x-e[2].c.x);
			slopeSpanG=(e[1].c.g-e[2].c.g)/(e[1].c.x-e[2].c.x);
			slopeSpanB=(e[1].c.b-e[2].c.b)/(e[1].c.x-e[2].c.x);
			sp.c.normalX=e[2].c.normalX+(deltaX*slopeSpanNormalX);
			sp.c.normalY=e[2].c.normalY+(deltaX*slopeSpanNormalY);
			sp.c.normalZ=e[2].c.normalZ+(deltaX*slopeSpanNormalZ);
			sp.c.r=e[2].c.r+(deltaX*slopeSpanR);
			sp.c.g=e[2].c.g+(deltaX*slopeSpanG);
			sp.c.b=e[2].c.b+(deltaX*slopeSpanB);
			//texture
			slopeSpanTx=(e[1].c.tx-e[2].c.tx)/(e[1].c.x-e[2].c.x);
			slopeSpanTy=(e[1].c.ty-e[2].c.ty)/(e[1].c.x-e[2].c.x);
			sp.c.tx=e[2].c.tx+(deltaX*slopeSpanTx);
			sp.c.ty=e[2].c.ty+(deltaX*slopeSpanTy);
		}
		if(triType==2) //point left
		{
			sp.s=e[1].c;
			sp.e=e[2].c;
			deltaX=ceil(e[1].c.x)-e[1].c.x;
			slopez=(e[2].c.z-e[1].c.z)/(e[2].c.x-e[1].c.x);
			sp.c.x=e[1].c.x+deltaX;
			sp.c.y=e[1].c.y;
			sp.c.z=e[1].c.z+deltaX*slopez;
			//coloring
			slopeSpanNormalX=(e[2].c.normalX-e[1].c.normalX)/(e[2].c.x-e[1].c.x);
			slopeSpanNormalY=(e[2].c.normalY-e[1].c.normalY)/(e[2].c.x-e[1].c.x);
			slopeSpanNormalZ=(e[2].c.normalZ-e[1].c.normalZ)/(e[2].c.x-e[1].c.x);
			slopeSpanR=(e[2].c.r -e[1].c.r)/(e[2].c.x-e[1].c.x);
			slopeSpanG=(e[2].c.g-e[1].c.g)/(e[2].c.x-e[1].c.x);
			slopeSpanB=(e[2].c.b-e[1].c.b)/(e[2].c.x-e[1].c.x);
			sp.c.normalX=e[1].c.normalX+(deltaX*slopeSpanNormalX);
			sp.c.normalY=e[1].c.normalY+(deltaX*slopeSpanNormalY);
			sp.c.normalZ=e[1].c.normalZ+(deltaX*slopeSpanNormalZ);
			sp.c.r=e[1].c.r+(deltaX*slopeSpanR);
			sp.c.g=e[1].c.g+(deltaX*slopeSpanG);
			sp.c.b=e[1].c.b+(deltaX*slopeSpanB);
			//texture
			slopeSpanTx=(e[2].c.tx-e[1].c.tx)/(e[2].c.x-e[1].c.x);
			slopeSpanTy=(e[2].c.ty-e[1].c.ty)/(e[2].c.x-e[1].c.x);
			sp.c.tx=e[1].c.tx+(deltaX*slopeSpanTx);
			sp.c.ty=e[1].c.ty+(deltaX*slopeSpanTy);
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
				if (render->interp_mode == GZ_COLOR)
				{
					float warpz=sp.c.z/(INT_MAX-sp.c.z);
					float u=sp.c.tx*(warpz+1);
					float v=sp.c.ty*(warpz+1);
					GzColor color;
					render->tex_fun(u, v, color);
					color[0]=color[0]*sp.c.r;
					color[1]=color[1]*sp.c.g;
					color[2]=color[2]*sp.c.b;
					GzPutDisplay(render->display, i, j, ctoi(color[0]), ctoi(color[1]), ctoi(color[2]), 0, (GzDepth)sp.c.z);
				}
				if (render->interp_mode == GZ_NORMALS)
				{
					float warpz=sp.c.z/(INT_MAX-sp.c.z);
					float u=sp.c.tx*(warpz+1);
					float v=sp.c.ty*(warpz+1);
					GzColor color;
					render->tex_fun(u, v, color);
					for (int i=0; i<3; i++)
					{
						render->Ka[i]=color[i];
						render->Kd[i]=color[i];
					}
					Coloring(render, sp.c, color);
					GzPutDisplay(render->display, i, j, ctoi(color[0]), ctoi(color[1]), ctoi(color[2]), 0, (GzDepth)sp.c.z);
				}
			}
			sp.c.x++;
			sp.c.z+=slopez;
			sp.c.normalX+=slopeSpanNormalX;
			sp.c.normalY+=slopeSpanNormalY;
			sp.c.normalZ+=slopeSpanNormalZ;
			sp.c.r+=slopeSpanR;
			sp.c.g+=slopeSpanG;
			sp.c.b+=slopeSpanB;
			//texture
			sp.c.tx+=slopeSpanTx;
			sp.c.ty+=slopeSpanTy;
		}
		e[1].c.y++;
		e[2].c.y++;
		e[1].c.x+=slopex1;
		e[1].c.z+=slopez1;
		e[2].c.x+=slopex2;
		e[2].c.z+=slopez2;
		e[1].c.normalX+=slopeNormalX1;
		e[1].c.normalY+=slopeNormalY1;
		e[1].c.normalZ+=slopeNormalZ1;
		e[2].c.normalX+=slopeNormalX2;
		e[2].c.normalY+=slopeNormalY2;
		e[2].c.normalZ+=slopeNormalZ2;
		e[1].c.r+=slopeR1;
		e[1].c.g+=slopeG1;
		e[1].c.b+=slopeB1;
		e[2].c.r+=slopeR2;
		e[2].c.g+=slopeG2;
		e[2].c.b+=slopeB2;
		//texture
		e[1].c.tx+=slopetx1;
		e[2].c.tx+=slopetx2;
		e[1].c.ty+=slopety1;
		e[2].c.ty+=slopety2;
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
	memset((**render).Xnorm,0,sizeof((**render).Xnorm));
	memset((**render).Xsp,0,sizeof((**render).Xsp));
	memset((**render).camera.Xiw,0,sizeof((**render).camera.Xiw));
	memset((**render).camera.Xpi,0,sizeof((**render).camera.Xpi));
	//For stack push
	(**render).Ximage[0][0][0]=1;
	(**render).Ximage[0][1][1]=1;
	(**render).Ximage[0][2][2]=1;
	(**render).Ximage[0][3][3]=1;
	//For Xnorm push
	(**render).Xnorm[0][0][0]=1;
	(**render).Xnorm[0][1][1]=1;
	(**render).Xnorm[0][2][2]=1;
	(**render).Xnorm[0][3][3]=1;
	//init Xsp
	(**render).Xsp[0][0] = (**render).Xsp[0][3] = (**render).display->xres/2;
	(**render).Xsp[1][1] = -((**render).display->yres/2);
	(**render).Xsp[1][3] = (**render).display->yres/2;
	(**render).camera.FOV=DEFAULT_FOV;
	float d=1/(tan(PI*((**render).camera.FOV)/360));
	(*render)->Xsp[2][2] =INT_MAX/d;
	(*render)->Xsp[3][3] = 1.0;
	//Camera parameters
	
	(**render).camera.lookat[X]=0;
	(**render).camera.lookat[Y]=0;
	(**render).camera.lookat[Z]=0;

	(**render).camera.position[X]=DEFAULT_IM_X;
	(**render).camera.position[Y]=DEFAULT_IM_Y;
	(**render).camera.position[Z]=DEFAULT_IM_Z;

	(**render).camera.worldup[X]=0;
	(**render).camera.worldup[Y]=1;
	(**render).camera.worldup[Z]=0;

	(**render).numlights=0;
	(*render)->interp_mode = GZ_RGB_COLOR;
	GzColor Ka = DEFAULT_AMBIENT;
	GzColor Kd = DEFAULT_DIFFUSE;
	GzColor Ks = DEFAULT_SPECULAR;
	for(int i=0; i<3; i++)
	{
		(*render)->Ka[i]=Ka[i];
		(*render)->Kd[i]=Kd[i];
		(*render)->Ks[i]=Ks[i];
	}
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
	GzMatrix unitMatrix={1.0,	0.0,	0.0,	0.0, 
									0.0,	1.0,	0.0,	0.0, 
									0.0,	0.0,	1.0,	0.0, 
									0.0,	0.0,	0.0,	1.0 };
	GzInitDisplay(render->display);
	//put Xsp at base of stack
	GzPushMatrix(render, render->Xsp);
	GzPushNormalMatrix(render, unitMatrix);
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
	GzPushNormalMatrix(render, unitMatrix);
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
	render->Xsp[2][2] = INT_MAX * tan((render->camera.FOV / 2.0) * (PI / 180.0));
	return GZ_SUCCESS;
}

int GzPushNormalMatrix(GzRender* render, GzMatrix matrix)
{
	matrixMul(render->Xnorm[render->matlevel-1], matrix, render->Xnorm[render->matlevel]);
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

	if(render->matlevel>=3)
	{
		float k = 1 / sqrt(matrix[0][0] * matrix[0][0] + matrix[1][0] * matrix[1][0] + matrix[2][0] * matrix[2][0]);
		GzMatrix r;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				r[i][j] = matrix[i][j] * k;
		for (int i=0; i<3; i++)
		{
			r[3][i]=0;
			r[i][3]=0;
		}
		r[3][3] = 1;
		matrixMul(render->Xnorm[render->matlevel-1], r, render->Xnorm[render->matlevel]);
	}
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
			continue;
		}
		if (nameList[i]==GZ_INTERPOLATE)
		{
			int* mode=(int*) valueList[i];
			render->interp_mode=*mode;
			continue;
		}
		if (nameList[i]==GZ_DIRECTIONAL_LIGHT)
		{
			GzLight* directionalLight = (GzLight*) valueList[i];
			render->lights[render->numlights] = *directionalLight;
			normalize(render->lights[render->numlights].direction);
			render->numlights++;
			continue;
		}
		if (nameList[i]==GZ_AMBIENT_LIGHT)
		{
			GzLight* ambientLight = (GzLight*) valueList[i];
			render->ambientlight = *ambientLight;
		}
		if (nameList[i]==GZ_AMBIENT_COEFFICIENT)
		{
			GzColor* ambientCo = (GzColor*) valueList[i];
			render->Ka[0]=ambientCo[0][0];
			render->Ka[1]=ambientCo[0][1];
			render->Ka[2]=ambientCo[0][2];
			continue;
		}
		if (nameList[i]==GZ_DIFFUSE_COEFFICIENT)
		{
			GzColor* diffuseCo = (GzColor*) valueList[i];
			render->Kd[0]=diffuseCo[0][0];
			render->Kd[1]=diffuseCo[0][1];
			render->Kd[2]=diffuseCo[0][2];
			continue;
		}
		if (nameList[i]==GZ_SPECULAR_COEFFICIENT)
		{
			GzColor* specularCo = (GzColor*) valueList[i];
			render->Ks[0]=specularCo[0][0];
			render->Ks[1]=specularCo[0][1];
			render->Ks[2]=specularCo[0][2];
			continue;
		}
		if (nameList[i]==GZ_DISTRIBUTION_COEFFICIENT)
		{
			float* spec=(float*) valueList[i];
			render->spec=*spec;
		}
		if (nameList[i]==GZ_TEXTURE_MAP)
		{
			GzTexture texture=GzTexture(valueList[i]);
			render->tex_fun=texture;
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
	GzCoord* newNorm=new GzCoord[3];
	GzCoord* newCo=new GzCoord[3];
	GzTextureIndex* newTexture=new GzTextureIndex[3];
	int flag=0;
	vertex* v=new vertex[3];

	for(int i=0;i< numParts; i++)
	{
		if(nameList[i]==GZ_POSITION)
		{
			GzCoord* co=(GzCoord*) valueList[i];
			calculateNewCood(co, newCo, render->Ximage[render->matlevel]);
			for (int j=0;j<2;j++)
			{
				if (newCo[i][2]<render->camera.position[Z])
				{
					flag=1;
				}
			}
		}
		if (nameList[i]==GZ_NORMAL)
		{
				GzCoord* norm=(GzCoord*) valueList[i];
				calculateNewCood(norm, newNorm, render->Xnorm[render->matlevel]);
				normalize(newNorm[0]);
				normalize(newNorm[1]);
				normalize(newNorm[2]);
		}
		if (nameList[i]==GZ_TEXTURE_INDEX)
		{
			GzTextureIndex* texture = (GzTextureIndex*)(valueList[2]);
			for (int i=0; i<3; i++)
			{
				float Vz = newCo[i][Z] / (INT_MAX - newCo[i][Z]);
				newTexture[i][0]=texture[i][0]/(Vz+1);
				newTexture[i][1]=texture[i][1]/(Vz+1); 
			}
		}
	}
		if (flag!=1)
		{
			sortByY(newCo, v, newNorm, newTexture);
			edge* e=new edge[3]; 
			int triType;
			initEdge(e, v, &triType);
			GzColor* color=new GzColor[3];
			for (int i=0;i<3;i++)
			{
				Coloring(render, v[i], color[i]);
				v[i].r=color[i][RED];
				v[i].g=color[i][GREEN];
				v[i].b=color[i][BLUE];
			}
			goRender(render, v, e, triType);
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
