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

	/* Text the bounds of U and V. Make sure they are within [0,1] */
	if( u < 0 ){u = 0;}
	if( u > 1 ){u = 1;}
	if( v < 0 ){v = 0;}
	if( v > 1 ){v = 1;}

	// compute texel index. Note that the maximum texel X is (xs - 1) and the maximum texel Y is (xy - 1)
	float texelX = u * (xs - 1);
	float texelY = v * (ys - 1);

	// find the texture corner values
	float minX = floor(texelX);
	float maxX = ceil(texelX);
	float minY = floor(texelY);
	float maxY = ceil(texelY);

	//get colors from four cell corners
	GzColor c0, c1, c2, c3;

	c0[RED] = image[(int)(minX + minY * xs)][RED];
	c0[GREEN] = image[(int)(minX + minY * xs)][GREEN];
	c0[BLUE] = image[(int)(minX + minY * xs)][BLUE];

	c1[RED] = image[(int)(maxX + minY * xs)][RED];
	c1[GREEN] = image[(int)(maxX + minY * xs)][GREEN];
	c1[BLUE] = image[(int)(maxX + minY * xs)][BLUE];

	c2[RED] = image[(int)(minX + maxY * xs)][RED];
	c2[GREEN] = image[(int)(minX + maxY * xs)][GREEN];
	c2[BLUE] = image[(int)(minX + maxY * xs)][BLUE];

	c3[RED] = image[(int)(maxX + maxY * xs)][RED];
	c3[GREEN] = image[(int)(maxX + maxY * xs)][GREEN];
	c3[BLUE] = image[(int)(maxX + maxY * xs)][BLUE];

	// perform bilinear interpolation
	GzColor colorTopRow, colorBottomRow;
	float left_wt = maxX - texelX;
	float top_wt = maxY - texelY;
	for( i = 0; i < 3; i++ )
	{
		// interpolate color component from top to bottom
		colorTopRow[i] = ( left_wt * c0[i] ) + ( ( 1 - left_wt ) * c1[i] );
		colorBottomRow[i] = ( left_wt * c2[i] ) + ( ( 1 - left_wt ) * c3[i] );

		// interpolate between top and bottom
		color[i] = ( top_wt * colorTopRow[i] ) + ( ( 1 - top_wt ) * colorBottomRow[i] );
	}

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

