/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include <math.h>
#include <time.h>
#include <cstdlib>

GzColor	*image;
int xs, ys;
int reset = 1;
float image2[128][128];

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */
  if (u<0)
  {
	  u=0;
  }
  if (u>1)
  {
	  u=1;
  }
  if (v<0)
  {
	  v=0;
  }
  if (v>1)
  {
	  v=1;
  }
  float s=u*(xs-1);
  float t=v*(ys-1);
  int indexA=floor(s)+(floor(t)*xs);
  int indexB=ceil(s)+(floor(t)*xs);
  int indexC=ceil(s)+(ceil(t)*xs);
  int indexD=floor(s)+(ceil(t)*xs);
  color[RED]=u*v*image[indexC][RED]+(1-u)*v*image[indexD][RED]+u*(1-v)*image[indexB][RED]+(1-u)*(1-v)*image[indexA][RED];
  color[GREEN]=u*v*image[indexC][GREEN]+(1-u)*v*image[indexD][GREEN]+u*(1-v)*image[indexB][GREEN]+(1-u)*(1-v)*image[indexA][GREEN];
  color[BLUE]=u*v*image[indexC][BLUE]+(1-u)*v*image[indexD][BLUE]+u*(1-v)*image[indexB][BLUE]+(1-u)*(1-v)*image[indexA][BLUE];

  return GZ_SUCCESS;
}


/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{  
	if(reset)
	{
		struct COMPLEX
		{
			double re;
			double im;
		};
		/////////////////////////////////////////////////
		/////////////////////////////////////////////////
			/////////////////////////////////////////////////
			COMPLEX z, c;
			c.re = -0.95, c.im = 0.0;
			for(int x=0; x<128; x++)
			{
				for(int y=0; y<128; y++)
				{
					z.re = -1.6 + 3.2 * (x / 128.0);
					z.im = -1.2 + 2.4 * (y / 128.0);
					int k=0;
					for(k=0; k<100; k++)
					{
						if ( z.re*z.re + z.im*z.im > 4.0 )     break;
						z.re = z.re * z.re - z.im * z.im;
						z.im = z.im * z.re + z.re * z.im;
						z.re = z.re + c.re;
						z.im = z.im + c.im;
					}
					if(k<100)
						image2[x][y]=(float)((k<<5) % 128);
				}
			}
			 reset = 0;          /* init is done */
	}
	if (u<0)
	{
		u=0;
	}
	if (u>1)
	{
		u=1;
	}
	if (v<0)
	{
		v=0;
	}
	if (v>1)
	{
		v=1;
	}
	color[0] = image2[(int)(u*128)][(int)(v*128)];
	color[1] = 1;
	color[2] = 0.5;

	
	return GZ_SUCCESS;
}

