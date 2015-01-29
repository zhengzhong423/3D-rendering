/*   CS580 HW   */
#include   "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* create a framebuffer:
 -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
 -- pass back pointer 
*/
	*framebuffer=new char[sizeof(GzPixel)*width*height];
	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, GzDisplayClass dispClass, int xRes, int yRes)
{
/* create a display:
  -- allocate memory for indicated class and resolution
  -- pass back pointer to GzDisplay object in display
*/
	*display = new GzDisplay;
	(*display)->dispClass=dispClass;
	(*display)->xres = (unsigned short)xRes;

	(*display)->yres=(unsigned short)yRes;

	(*display)->fbuf= new GzPixel[(*display)->xres*(*display)->yres];
	(*display)->open=0;

	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* clean up, free memory */
	delete display->fbuf;
	delete display;
	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
/* pass back values for an open display */
	*xRes=display->xres;
	*yRes=display->yres;
	*dispClass=display->dispClass;
	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* set everything to some default values - start a new frame */
	for (int j = 0; j < display->yres; j++)
	{
		for (int i = 0; i < display->xres; i++)
		{
			display->fbuf[ARRAY(i,j)].red	=	2677; 
			display->fbuf[ARRAY(i,j)].green	=	2342;
			display->fbuf[ARRAY(i,j)].blue	=	2008  ;
			display->fbuf[ARRAY(i,j)].alpha	=	0;
			display->fbuf[ARRAY(i,j)].z	=	0;
		}
	}
	display->open=1;
	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* write pixel values into the display */
	if(display->xres>=i&&i>=0&&display->yres>=j&&j>=0){
	display->fbuf[ARRAY(i,j)].blue=b;
	display->fbuf[ARRAY(i,j)].green=g;
	display->fbuf[ARRAY(i,j)].red=r;
	display->fbuf[ARRAY(i,j)].alpha=a;
	display->fbuf[ARRAY(i,j)].z=z;
	return GZ_SUCCESS;
	}
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	/* pass back pixel value in the display */
	/* check display class to see what vars are valid */
	if(display->xres>=i&&i>=0&&display->yres>=j&&j>=0){
	*a=display->fbuf[ARRAY(i,j)].alpha;
	*z=display->fbuf[ARRAY(i,j)].z;
	*b=display->fbuf[ARRAY(i,j)].blue;
	*g=display->fbuf[ARRAY(i,j)].green;
	*r=display->fbuf[ARRAY(i,j)].red;
	return GZ_SUCCESS;}
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

	/* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
	fprintf(outfile, "P3 %d %d 5333\r", display->xres, display->yres);
	for (int j = 0; j < display->yres; j++)
	{
		for (int i = 0; i < display->xres; i++)
		{
				fprintf(outfile, "%d %d %d ", display->fbuf[ARRAY(i,j)].red, display->fbuf[ARRAY(i,j)].green, display->fbuf[ARRAY(i,j)].blue);
		}
	}
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

	/* write pixels to framebuffer: 
		- Put the pixels into the frame buffer
		- Caution: store the pixel to the frame buffer as the order of blue, green, and red 
		- Not red, green, and blue !!!
	*/
	int iFrameBufferSize = display->xres * display->yres;
	int i = 0,j = 0;
	
	while(j < iFrameBufferSize)
	{
		framebuffer[i++] = (char)(256.0*display->fbuf[j].blue/5333);    
		framebuffer[i++] = (char)(256.0*display->fbuf[j].green/5333);
		framebuffer[i++] = (char)(256.0*display->fbuf[j].red/5333);		
		j++;
	}
		
	return GZ_SUCCESS;
}