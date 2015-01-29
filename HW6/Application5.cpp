// Application5.cpp: implementation of the Application5 class.
//
//////////////////////////////////////////////////////////////////////

/*
 * application test code for homework assignment #5
*/

#include "stdafx.h"
#include "CS580HW.h"
#include "Application5.h"
#include "Gz.h"
#include "disp.h"
#include "rend.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INFILE  "ppot.asc"
#define OUTFILE "output.ppm"

extern int tex_fun(float u, float v, GzColor color); /* image texture function */
extern int ptex_fun(float u, float v, GzColor color); /* procedural texture function */

void shade(GzCoord norm, GzCoord color);

float AAFilter[6][3]=
{
	-0.52, 0.38, 0.128,
	0.41, 0.56, 0.119,
	0.27, 0.08, 0.294,
	-0.17, -0.29, 0.249,
	0.58, -0.55, 0.104,
	-0.31, -0.71, 0.106
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application5::Application5()
{

}

Application5::~Application5()
{
	
}

int Application5::Initialize()
{
	GzCamera	camera;  
	int		    xRes, yRes, dispClass;	/* display parameters */ 

	GzToken		nameListShader[9]; 	    /* shader attribute names */
	GzPointer   valueListShader[9];		/* shader attribute pointers */
	GzToken     nameListLights[10];		/* light info */
	GzPointer   valueListLights[10];
	int			shaderType, interpStyle;
	float		specpower;
	int		status; 

	status = 0; 

	/* 
	 * Allocate memory for user input
	 */
	m_pUserInput = new GzInput;

	/* 
	 * initialize the display and the renderer 
	 */ 
 	m_nWidth = 256;		// frame buffer and display width
	m_nHeight = 256;    // frame buffer and display height

	status |= GzNewFrameBuffer(&m_pFrameBuffer, m_nWidth, m_nHeight);

	status |= GzNewDisplay(&finalDisplay, GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight);

	status |= GzGetDisplayParams(finalDisplay, &xRes, &yRes, &dispClass); 

	status |= GzInitDisplay(finalDisplay); 

	shiftedDisplay = new GzDisplay* [6];
	shiftedRender = new GzRender* [6];

/*

	status |= GzNewFrameBuffer(&m_pFrameBuffer, m_nWidth, m_nHeight);

	status |= GzNewDisplay(&m_pDisplay, GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight);

	status |= GzGetDisplayParams(m_pDisplay, &xRes, &yRes, &dispClass); 
	 
	status |= GzInitDisplay(m_pDisplay); 
 
	status |= GzNewRender(&m_pRender, GZ_Z_BUFFER_RENDER, m_pDisplay); 
*/


/* Translation matrix */
GzMatrix	scale = 
{ 
	3.25,	0.0,	0.0,	0.0, 
	0.0,	3.25,	0.0,	-3.25, 
	0.0,	0.0,	3.25,	3.5, 
	0.0,	0.0,	0.0,	1.0 
}; 
 
GzMatrix	rotateX = 
{ 
	1.0,	0.0,	0.0,	0.0, 
	0.0,	.7071,	.7071,	0.0, 
	0.0,	-.7071,	.7071,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 
 
GzMatrix	rotateY = 
{ 
	.866,	0.0,	-0.5,	0.0, 
	0.0,	1.0,	0.0,	0.0, 
	0.5,	0.0,	.866,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 
for (int i=0; i<6; i++)
{
	status |= GzNewDisplay(&shiftedDisplay[i], GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight);

	status |= GzGetDisplayParams(shiftedDisplay[i], &xRes, &yRes, &dispClass); 

	status |= GzInitDisplay(shiftedDisplay[i]); 

	status |= GzNewRender(&shiftedRender[i], GZ_Z_BUFFER_RENDER, shiftedDisplay[i]); 

#if 1 	/* set up app-defined camera if desired, else use camera defaults */
    camera.position[X] = -3;
    camera.position[Y] = -25;
    camera.position[Z] = -4;

    camera.lookat[X] = 7.8;
    camera.lookat[Y] = 0.7;
    camera.lookat[Z] = 6.5;

    camera.worldup[X] = -0.2;
    camera.worldup[Y] = 1.0;
    camera.worldup[Z] = 0.0;

    camera.FOV = 63.7;              /* degrees *              /* degrees */

	status |= GzPutCamera(shiftedRender[i], &camera); 
#endif 

	/* Start Renderer */
	status |= GzBeginRender(shiftedRender[i]);

	/* Light */
	GzLight	light1 = { {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };
	GzLight	light2 = { {0, -0.7071, -0.7071}, {0.9, 0.2, 0.3} };
	GzLight	light3 = { {0.7071, 0.0, -0.7071}, {0.2, 0.7, 0.3} };
	GzLight	ambientlight = { {0, 0, 0}, {0.3, 0.3, 0.3} };

	/* Material property */
	GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
	GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
	GzColor diffuseCoefficient = {0.7, 0.7, 0.7};

/* 
  renderer is ready for frame --- define lights and shader at start of frame 
*/

        /*
         * Tokens associated with light parameters
         */
        nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[0] = (GzPointer)&light1;
        nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[1] = (GzPointer)&light2;
        nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[2] = (GzPointer)&light3;
        status |= GzPutAttribute(shiftedRender[i], 3, nameListLights, valueListLights);

        nameListLights[0] = GZ_AMBIENT_LIGHT;
        valueListLights[0] = (GzPointer)&ambientlight;
        status |= GzPutAttribute(shiftedRender[i], 1, nameListLights, valueListLights);

        /*
         * Tokens associated with shading 
         */
        nameListShader[0]  = GZ_DIFFUSE_COEFFICIENT;
        valueListShader[0] = (GzPointer)diffuseCoefficient;

	/* 
	* Select either GZ_COLOR or GZ_NORMALS as interpolation mode  
	*/
        nameListShader[1]  = GZ_INTERPOLATE;
		interpStyle = GZ_NORMALS;         /* Phong shading */
	//	interpStyle=GZ_COLOR;		/*Gouraud shading*/
        valueListShader[1] = (GzPointer)&interpStyle;

        nameListShader[2]  = GZ_AMBIENT_COEFFICIENT;
        valueListShader[2] = (GzPointer)ambientCoefficient;
        nameListShader[3]  = GZ_SPECULAR_COEFFICIENT;
        valueListShader[3] = (GzPointer)specularCoefficient;
        nameListShader[4]  = GZ_DISTRIBUTION_COEFFICIENT;
        specpower = 32;
        valueListShader[4] = (GzPointer)&specpower;

        nameListShader[5]  = GZ_TEXTURE_MAP;
#if 0   /* set up null texture function or valid pointer */
        valueListShader[5] = (GzPointer)0;
#else
      valueListShader[5] = (GzPointer)(tex_fun);	/* or use ptex_fun */
//	 valueListShader[5] = (GzPointer)(ptex_fun);
#endif
	  nameListShader[6]  = GZ_AASHIFTX;
	  valueListShader[6] = (GzPointer)&(AAFilter[i][0]);
	  nameListShader[7]  = GZ_AASHIFTY;
	  valueListShader[7] = (GzPointer)&(AAFilter[i][1]);

    status |= GzPutAttribute(shiftedRender[i], 8, nameListShader, valueListShader);


	status |= GzPushMatrix(shiftedRender[i], scale);  
	status |= GzPushMatrix(shiftedRender[i], rotateY); 
	status |= GzPushMatrix(shiftedRender[i], rotateX); 

	if (status) exit(GZ_FAILURE); 
}
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Render() 
{
	GzToken		nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[3]; 	/* vertex attribute pointers */
	GzCoord		vertexList[3];	/* vertex position coordinates */ 
	GzCoord		normalList[3];	/* vertex normals */ 
	GzTextureIndex  	uvList[3];		/* vertex texture map indices */ 
	char		dummy[256]; 
	int			status; 


	/* Initialize Display */
	for (int i = 0; i < 6; i++)
	{
		status |= GzInitDisplay(shiftedDisplay[i]); 
	}
	
	/* 
	* Tokens associated with triangle vertex values 
	*/ 
	nameListTriangle[0] = GZ_POSITION; 
	nameListTriangle[1] = GZ_NORMAL; 
	nameListTriangle[2] = GZ_TEXTURE_INDEX;  

	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE , "r" )) == NULL )
	{
         AfxMessageBox( "The input file was not opened\n" );
		 return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
	{
         AfxMessageBox( "The output file was not opened\n" );
		 return GZ_FAILURE;
	}

	/* 
	* Walk through the list of triangles, set color 
	* and render each triangle 
	*/ 
	while( fscanf(infile, "%s", dummy) == 1) { 	/* read in tri word */
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[0][0]), &(vertexList[0][1]),  
		&(vertexList[0][2]), 
		&(normalList[0][0]), &(normalList[0][1]), 	
		&(normalList[0][2]), 
		&(uvList[0][0]), &(uvList[0][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[1][0]), &(vertexList[1][1]), 	
		&(vertexList[1][2]), 
		&(normalList[1][0]), &(normalList[1][1]), 	
		&(normalList[1][2]), 
		&(uvList[1][0]), &(uvList[1][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[2][0]), &(vertexList[2][1]), 	
		&(vertexList[2][2]), 
		&(normalList[2][0]), &(normalList[2][1]), 	
		&(normalList[2][2]), 
		&(uvList[2][0]), &(uvList[2][1]) ); 

	    /* 
	     * Set the value pointers to the first vertex of the 	
	     * triangle, then feed it to the renderer 
	     * NOTE: this sequence matches the nameList token sequence
	     */ 
	     valueListTriangle[0] = (GzPointer)vertexList; 
		 valueListTriangle[1] = (GzPointer)normalList; 
		 valueListTriangle[2] = (GzPointer)uvList; 
		 for (int i = 0; i < 6; i++)
		 {
			 GzPutTriangle(shiftedRender[i], 3, nameListTriangle, valueListTriangle); 
		 }
		 
	} 
	#define	ARRAY2(i,j)	(i+(j*finalDisplay->xres))	/* simplify fbuf indexing */
	for (int i = 0; i < finalDisplay->yres; i++)
	{
		for (int j = 0; j < finalDisplay->xres; j++)
		{
			finalDisplay->fbuf[ARRAY2(i,j)].red = 0;
			finalDisplay->fbuf[ARRAY2(i,j)].green = 0;
			finalDisplay->fbuf[ARRAY2(i,j)].blue = 0;
			for (int k = 0; k < 6; k++)
			{
				finalDisplay->fbuf[ARRAY2(i,j)].red = finalDisplay->fbuf[ARRAY2(i,j)].red + (shiftedDisplay[k]->fbuf[ARRAY2(i,j)].red * AAFilter[k][2]);
				finalDisplay->fbuf[ARRAY2(i,j)].green = finalDisplay->fbuf[ARRAY2(i,j)].green + (shiftedDisplay[k]->fbuf[ARRAY2(i,j)].green * AAFilter[k][2]);
				finalDisplay->fbuf[ARRAY2(i,j)].blue = finalDisplay->fbuf[ARRAY2(i,j)].blue + (shiftedDisplay[k]->fbuf[ARRAY2(i,j)].blue * AAFilter[k][2]);
			}
		}
		}
	GzFlushDisplay2File(outfile, finalDisplay); 	/* write out or update display to file*/
	GzFlushDisplay2FrameBuffer(m_pFrameBuffer, finalDisplay);	// write out or update display to frame buffer

	/* 
	 * Close file
	 */ 

	if( fclose( infile ) )
      AfxMessageBox( "The input file was not closed\n" );

	if( fclose( outfile ) )
      AfxMessageBox( "The output file was not closed\n" );
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Clean()
{
	/* 
	 * Clean up and exit 
	 */ 
	int	status = 0; 
	for (int i=0; i<6; i++)
	{
		status |= GzFreeRender(shiftedRender[i]); 
		status |= GzFreeDisplay(shiftedDisplay[i]);
	}
	status |= GzFreeDisplay(finalDisplay);
	
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS);
}



