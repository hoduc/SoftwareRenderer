/* CS580 Homework 3 */

//#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include	"util.h"


#ifndef M_PI
	#define M_PI       3.14159265358979323846
//#define M_PI 3.1459
#endif

#define TO_RADIAN(degree) degree*M_PI/180

typedef struct s3f
{
	float a;
	float b;
	float c;
}s3f;

typedef struct Gzplane
{
	float aCoeff;
	float bCoeff;
	float cCoeff;
	float dCoeff;
}Gzplane;


int sets3f(float a[3], s3f s)
{
	if (a == NULL)
		return GZ_FAILURE;
	a[0] = s.a;
	a[1] = s.b;
	a[2] = s.c;
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

/*
short	ctoi(float color)	
{
	return(short)((int)(color * ((1 << 12) - 1)));
}
*/

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	setMatrix(mat, IDENTITY_MATRIX);
	float radian = TO_RADIAN(degree);
	mat[1][Y] = mat[2][Z] = cos(radian);
	mat[1][Z] = -sin(radian);
	mat[2][Y] = -mat[1][Z];
	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	setMatrix(mat, IDENTITY_MATRIX);
	float radian = TO_RADIAN(degree);
	mat[0][X] = mat[2][Z] = cos(radian);
	mat[0][Z] = sin(radian);
	mat[2][X] = -mat[0][Z];
	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	//setMatrix(mat, IDENTITY_MATRIX);
	float radian = TO_RADIAN(degree);
	mat[0][X] = mat[1][Y] = cos(radian);
	mat[0][Y] = -sin(radian);
	mat[1][X] = -mat[0][Y];
	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	mat[0][W] += translate[X];
	mat[1][W] += translate[Y];
	mat[2][W] += translate[Z];
	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	//only allow uniform scaling => calculating scale factor
	float scalef = MAXF(MAXF(scale[X], scale[Y]), scale[Z]);
	mat[0][X] *= /*scale[X]*/scalef;
	mat[1][Y] *= /*scale[Y]*/scalef;
	mat[2][Z] *= /*scale[Z]*/scalef;
	return GZ_SUCCESS;
}


void initCamDefault(GzCamera& camera)
{
	camera.FOV = DEFAULT_FOV;
	setCoord(camera.lookat, 0, 0, 0);
	setCoord(camera.position, DEFAULT_IM_X, DEFAULT_IM_Y, DEFAULT_IM_Z);
	setCoord(camera.worldup, 0, 1, 0);	//default up vector is y-axis
}


/*
	Transformation
	All vertices are transformed thru the following state:
		Xsp*Xpi*Xiw
*/

/*
	Transform perspective to screen
	Mapping origin0 at center => origin1 at top-left
	Scale   by the size of screen buffer
*/
void buildXsp(GzMatrix xsp, GzDisplay* display)
{
	setMatrix(xsp, IDENTITY_MATRIX);
	xsp[0][X] = xsp[0][W] = display->xres / 2;
	xsp[1][Y] = xsp[1][W] = display->yres / 2;
	xsp[1][Y] = -xsp[1][Y];
	xsp[2][Z] = MAXINT;
}

/*
	image -> perspective
*/
void buildXpi(GzMatrix& xpi, float fov)
{
	setMatrix(xpi, IDENTITY_MATRIX);
	float scaleValue = tan(TO_RADIAN(fov/2));
	xpi[2][Z] = xpi[3][Z] = scaleValue;
}

/*
	World -> image(camera space)
*/
void buildXiw(GzMatrix xiw, GzCoord camPos, GzCoord lookAtPos, GzCoord upVector )
{
	setMatrix(xiw, IDENTITY_MATRIX);
	//constructing the camera coordinate system
	GzCoord zAxis; // z-axis
	edgeVector(camPos, lookAtPos, zAxis);
	normalizeVector(zAxis);
	GzCoord yAxis;
	setCoord(yAxis, zAxis);
	scalarMultVector(yAxis, dotProduct(upVector, zAxis));
	subVector(upVector, yAxis, yAxis);
	normalizeVector(yAxis);
	GzCoord xAxis;
	crossProduct(yAxis, zAxis, xAxis);
	normalizeVector(xAxis);
	setRow(xiw, 0, xAxis, -dotProduct(xAxis, camPos));
	setRow(xiw, 1, yAxis, -dotProduct(yAxis, camPos));
	setRow(xiw, 2, zAxis, -dotProduct(zAxis, camPos));
	
}

void camSpaceXform(GzCamera& cam)
{
	buildXpi(cam.Xpi, cam.FOV);
	buildXiw(cam.Xiw, cam.position, cam.lookat, cam.worldup);
}

void initXform(GzMatrix xform[MATLEVELS])
{
	for (int i = 0; i < MATLEVELS; i++)
		setMatrix(xform[i], IDENTITY_MATRIX);
}

int applyXform(GzMatrix xformMat, GzCoord* coords, GzCoord* coordsRet)
{
	if (coords == NULL)
		return GZ_FAILURE;
	float vHomo[4];
	float vHomoRet[4];
	for (int i = 0; i < 3; i++)
	{
		v3dToHomo(coords[i], vHomo);
		matrixMultVHomo(xformMat, vHomo, vHomoRet);
		vHomoTo3d(vHomoRet, coordsRet[i]);
	}

	return GZ_SUCCESS;
}



int initRenderDefaultParam(GzRender* render)
{
	int result = GZ_SUCCESS;

	render->matlevel = 0;
	render->numlights = 0;
	initCamDefault(render->camera);
	buildXsp(render->Xsp, render->display);
	initXform(render->Ximage);
	initXform(render->Xnorm);

	s3f t3 = DEFAULT_AMBIENT;

	sets3f(render->Ka, t3);
	
	t3 = DEFAULT_DIFFUSE;
	sets3f(render->Kd, t3);

	t3 = DEFAULT_SPECULAR;
	sets3f(render->Ks, t3);

	render->spec = DEFAULT_SPEC;
	
	return result;
}

//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- setup Xsp and anything only done once 
- save the pointer to display 
- init default camera 
*/ 
	*render = new GzRender;
	(*render)->display = display;
	(*render)->matlevel = -1; //for a reason
	(*render)->numlights = 0;
	initRenderDefaultParam(*render);
	(*render)->tex_fun = (GzTexture)0;
	return GZ_SUCCESS;

}



int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	if (render != NULL)
	{
		render->display = NULL; //DO NOT FREE THE DISPLAY
		delete render;
		render = NULL;
	}
	// CHECK FOR RENDER NOT FREE!!!
	return GZ_SUCCESS;
}




int GzBeginRender(GzRender *render)
{
/*  
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/
	GzInitDisplay(render->display);
	GzPushMatrix(render, render->Xsp);	//Xsp
	
	camSpaceXform(render->camera);
	GzPushMatrix(render, render->camera.Xpi);	//Xsp*Xpi
	GzPushMatrix(render, render->camera.Xiw);	//Xsp*Xpi*Xiw
	//skip Xsp, Xpi for Xnorm

	//now top is Xform : world -> screen
	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	render->camera = *camera;
	return GZ_SUCCESS;	
}

int GzPushMatrix(GzMatrix stack[MATLEVELS], GzMatrix matrix, short level)
{
	if (level == MATLEVELS)
		return GZ_FAILURE;
	if (level == 0)
		setMatrix(stack[level], matrix);
	else
		matrixMult(stack[level-1], matrix, stack[level]);
	return GZ_SUCCESS;
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	int result = GZ_SUCCESS;
	result &= GzPushMatrix(render->Ximage, matrix, render->matlevel);
	if (render->matlevel <= 1)
		result &= GzPushMatrix(render->Xnorm, IDENTITY_MATRIX, render->matlevel);
	else
	{
		//making sure xnorm contain no scaling and translation
		GzMatrix matrixForXnorm;
		setMatrix(matrixForXnorm, matrix);
		//remove the translation part
		matrixForXnorm[0][W] = matrixForXnorm[1][W] = matrixForXnorm[2][W] = 0;
		//convert to unitary rotation
		float scaleFactor = sqrt(matrix[0][X] * matrix[0][X] + matrix[0][Y] * matrix[0][Y] + matrix[0][Z] * matrix[0][Z]);
		matrixScalarMultRange(matrixForXnorm, 1 / scaleFactor, matrixForXnorm, 3, 3);
		result &= GzPushMatrix(render->Xnorm, matrixForXnorm, render->matlevel);
	}
	render->matlevel++;
	return result;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (render->matlevel < 0)
		return GZ_FAILURE;
	render->matlevel--;
	return GZ_SUCCESS;
}

float pointInEdge(GzCoord edge, int pixelX, int pixelY)
{
	return (edge[A] * pixelX + edge[B] * pixelY + edge[C]);
}


/*
	p(x,y) in triangle ?	
*/
bool inTriangle(GzCoord ef1, GzCoord ef2, GzCoord ef3, GzCoord vt12, GzCoord vt23, GzCoord vt31, int x, int y)
{
	float pie1 = pointInEdge(ef1, x, y);
	float pie2 = pointInEdge(ef2, x, y);
	float pie3 = pointInEdge(ef3, x, y);

	bool inTri = true;
	inTri &= (pie1 == 0) ? isLeftOrTopEdge(vt12) : pie1 > 0;
	inTri &= (pie2 == 0) ? isLeftOrTopEdge(vt23) : pie2 > 0;
	inTri &= (pie3 == 0) ? isLeftOrTopEdge(vt31) : pie3 > 0;
	return inTri;
}



/*
Given vertices list, sort by Y
bottom and top are special case
outcome : v1, v2, v3 creating a clock-wise order
*/
void sortVertical(GzCoord* vList, GzCoord v1, GzCoord v2, GzCoord v3)
{
	int minYIndex, maxYIndex, midYIndex;
	minYIndex = maxYIndex = 0;
	midYIndex = 0;
	for (int i = 0; i < 3; i++)
	{
		if (vList[i][Y] < vList[minYIndex][Y])
		{
			minYIndex = i;
		}
		else if (vList[i][Y] > vList[maxYIndex][Y])
		{
			maxYIndex = i;
		}
	}

	if (maxYIndex == 0 && minYIndex == 2 || maxYIndex == 2 && minYIndex == 0)
		midYIndex = 1;
	else if (maxYIndex == 0 && minYIndex == 1 || maxYIndex == 1 && minYIndex == 0)
		midYIndex = 2;
	//edge not beginning with mid
	GzCoord minMaxEdge;
	edgeFn(vList[minYIndex], vList[maxYIndex], minMaxEdge);

	float ymid = vList[midYIndex][Y];
	float xmid = vList[midYIndex][X];

	//special
	//top
	if (ymid == vList[minYIndex][Y])
	{
		setCoord(v1, vList[maxYIndex]);
		if (vList[minYIndex][X] < vList[midYIndex][X])
		{
			setCoord(v2, vList[minYIndex]);
			setCoord(v3, vList[midYIndex]);
		}
		else
		{
			setCoord(v2, vList[midYIndex]);
			setCoord(v3, vList[minYIndex]);
		}
	}
	//bottom
	else if (ymid == vList[maxYIndex][Y])
	{
		setCoord(v3, vList[minYIndex]);
		if (vList[maxYIndex][X] < vList[midYIndex][X])
		{
			setCoord(v1, vList[midYIndex]);
			setCoord(v2, vList[maxYIndex]);
		}
		else
		{
			setCoord(v1, vList[maxYIndex]);
			setCoord(v2, vList[midYIndex]);
		}
	}
	else
	{
		float xmidInMinMaxEdge = -(minMaxEdge[B] * ymid + minMaxEdge[C]) / minMaxEdge[A];

		//mid is on the left
		if (xmid < xmidInMinMaxEdge)
		{
			setCoord(v1, vList[maxYIndex]);
			setCoord(v2, vList[midYIndex]);
			setCoord(v3, vList[minYIndex]);
		}
		else
		{
			setCoord(v1, vList[maxYIndex]);
			setCoord(v2, vList[minYIndex]);
			setCoord(v3, vList[midYIndex]);
		}
	}


}


/*
interpolate Z value at (x,y)
Using plane eqn : Ax + By + Cz + d = 0
normal(A,B,C)
d is usually calculated after plugging in normal and (x,y,z) of a point inside the place(of course)
=> Z = -(Ax + By + d) / C
*/
/*
int interpolateZ(int x, int y, GzCoord normal, float d)
{
	return (int)(-(normal[A] * x + normal[B] * y + d) / normal[C]);
}
*/

/*
	what is the value of interpolant(z,R,G,B,Nx,Ny,Nz) in the plane?
*/
float interpolate(int x, int y, Gzplane plane)
{
	return (-(plane.aCoeff * x + plane.bCoeff * y + plane.dCoeff) / plane.cCoeff);
}

Gzplane interpPlaneNorm(float* distance_1_2, float* distance_2_3, float* term)
{
	Gzplane plane;
	float n[3];
	crossProduct(distance_1_2, distance_2_3, n);
	plane.aCoeff = n[0];
	plane.bCoeff = n[1];
	plane.cCoeff = n[2];
	plane.dCoeff = -dotProduct(n, term);
	return plane;
}


Gzplane interpPlane(float term1[3], float term2[3], float term3[3])
{
	float t_1_2[3];
	float t_2_3[3];
	sub(term1, term2, t_1_2);
	sub(term2, term3, t_2_3);
	return interpPlaneNorm(t_1_2, t_2_3, term1);
}


void calShading(GzCoord normal, GzMatrix normXform, GzLight* lights, int numLights, float spec, GzColor diffuse, GzColor specular)
{
	GzCoord eye{ 0, 0, -1 };
	GzCoord nImg;
	
	matrixMultV3d(normXform, normal, nImg);
	for (int i = 0; i < numLights; i++)
	{
		GzLight light = lights[i];
		GzColor le;

		GzCoord lImg;
		setCoord(lImg, light.direction);


		float NL = dotProduct(lImg, nImg);
		float NE = dotProduct(nImg, eye);

		if (NL > 0 && NE < 0 || NL < 0 && NE > 0)
			continue;
		if (NL < 0 && NE < 0)
		{
			negateVector(nImg);
			NL = dotProduct(lImg, nImg);
			NE = dotProduct(nImg, eye);
		}

		//R
		GzCoord R;
		setCoord(R, nImg);
		scalarMultVector(R, 2 * NL);
		subVector(R, lImg, R);
		float RE = dotProduct(R, eye);

		RE = clamp(RE, 0.0, 1.0);
		float REs = pow(RE, spec);


		//spec
		setColor(le, light.color);
		scalarMultVector(le, REs);
		add(specular, le, specular);

		//diffuse
		setColor(le, light.color);
		scalarMultVector(le, NL);
		add(diffuse, le, diffuse);
	}	
}

void calColorAtSurf(GzRender* render, GzCoord normal, GzColor color)
{
	setColor(color, 0, 0, 0);
	//GzCoord eye{ 0, 0, -1 };
	//GzCoord nImg;
	GzColor ambient{ 0, 0, 0 };
	GzColor diffuse{ 0, 0, 0 };
	GzColor spec{ 0, 0, 0 };

	

	calShading(normal, render->Xnorm[render->matlevel - 1], render->lights, render->numlights, render->spec, diffuse, spec);
	
	//color*Ka
	setColor(ambient, render->ambientlight.color);
	multVector(ambient, render->Ka, ambient);

	//color*Ks
	multVector(spec, render->Ks, spec);

	//*Kd
	multVector(diffuse, render->Kd, diffuse);

	//sum
	add(color, ambient, color);
	add(color, diffuse, color);
	add(color, spec, color);
}


/*
Render a triangle with a passed in vertices list & vertex's normals
*/
int renderTriangle(GzRender* render, GzCoord* vertices, GzCoord* normals, GzTextureIndex* textures)
{
	Gzplane interp;
	Gzplane cInterp[3];
	Gzplane nInterp[3];
	Gzplane uvInterp[2];
	int result = GZ_SUCCESS;

	GzCoord verticesScreen[3];
	//transform to screen space
	applyXform(render->Ximage[render->matlevel - 1], vertices, verticesScreen);

	///////////////////////RENDER/////////////////////////////////
	
	GzCoord* vList = verticesScreen;
	//shift the vertices
	for (int i = 0; i < 3; i++)
	{
		verticesScreen[i][X] += render->xOffset;
		verticesScreen[i][Y] += render->yOffset;
	}
	//sort by y
	GzCoord v1, v2, v3;	
	sortVertical(vList, v1, v2, v3);

	//create edge function
	GzCoord e1, e2, e3;
	edgeFn(v1, v2, e1);
	edgeFn(v2, v3, e2);
	edgeFn(v3, v1, e3);

	
	GzCoord vt12, vt23, vt31;
	edgeVector(v1, v2, vt12);
	edgeVector(v2, v3, vt23);
	edgeVector(v3, v1, vt31);

	interp = interpPlaneNorm(vt12, vt23, v1);

	float term1[3];
	float term2[3];
	float term3[3];

	//transform textures to perspective space
	//if given
	if (render->tex_fun)
	{
		float screenText[3][2];
		float zscreen = 0;
		float zp = 0;
		for (int i = 0; i < 2; i++)
		{
			//v1
			zscreen = vList[0][Z];
			zp = zscreen / (MAXINT - zscreen);
			screenText[0][i] = textures[0][i] / (zp + 1);

			//v2
			zscreen = vList[1][Z];
			zp = zscreen / (MAXINT - zscreen);
			screenText[1][i] = textures[1][i] / (zp + 1);

			//v3
			zscreen = vList[2][Z];
			zp = zscreen / (MAXINT - zscreen);
			screenText[2][i] = textures[2][i] / (zp + 1);

			//setup interpolation plane
			setF3(term1, vList[0][X], vList[0][Y], screenText[0][i]);
			setF3(term2, vList[1][X], vList[1][Y], screenText[1][i]);
			setF3(term3, vList[2][X], vList[2][Y], screenText[2][i]);
			uvInterp[i] = interpPlane(term1, term2, term3);
		}
	}
	
	
	if (render->interp_mode == GZ_COLOR)
	{
		
		GzColor rgbV[3];
		
		for (int i = 0; i < 3; i++)
		{
			if (render->tex_fun)
			{
				GzColor ambient{ 0, 0, 0 }, diffuse{ 0, 0, 0 }, specular{ 0, 0, 0 };
				setColor(ambient, render->ambientlight.color);
				//delay multiplying term
				setColor(rgbV[i], ambient);
				calShading(normals[i], render->Xnorm[render->matlevel - 1], render->lights, render->numlights, render->spec, diffuse, specular);
				add(rgbV[i], diffuse, rgbV[i]);
				add(rgbV[i], specular, rgbV[i]);
			}
			else
			{
				calColorAtSurf(render, normals[i], rgbV[i]);
			}
		}
		for (int i = 0; i < 3; i++)
		{
			setF3(term1, vList[0][X], vList[0][Y], rgbV[0][i]);
			setF3(term2, vList[1][X], vList[1][Y], rgbV[1][i]);
			setF3(term3, vList[2][X], vList[2][Y], rgbV[2][i]);

			cInterp[i] = interpPlane(term1, term2, term3);
		}
	}
	
	else if (render->interp_mode == GZ_NORMAL)
	{
		//transform normal to screen
		for (int i = 0; i < 3; i++)
		{
			setF3(term1, vList[0][X], vList[0][Y], normals[0][i]);
			setF3(term2, vList[1][X], vList[1][Y], normals[1][i]);
			setF3(term3, vList[2][X], vList[2][Y], normals[2][i]);
			nInterp[i] = interpPlane(term1, term2, term3);
		}
	}

	//triangle bounding box : seem to be inefficient but it will do
	float xmin, ymin, xmax, ymax;
	xmin = MINF(MINF(v1[X], v2[X]), v3[X]);
	ymin = MINF(MINF(v1[Y], v2[Y]), v3[Y]);
	xmax = MAXF(MAXF(v1[X], v2[X]), v3[X]);
	ymax = MAXF(MAXF(v1[Y], v2[Y]), v3[Y]);

	for (int y = ymin; y < ymax; y++)
	{

		for (int x = xmin; x < xmax; x++)
		{
			if (inTriangle(e1, e2, e3, vt12, vt23, vt31, x, y))
			{

				int interpZ = interpolate(x, y, interp);
				float zp = (float)interpZ / (MAXINT - interpZ);
				
				GzPixel* pixel = getPixelAt(render->display, x, y);
				if (pixel != NULL && interpZ > 0 && interpZ < pixel->z)
				{
					GzColor interpC;
					float r, g, b;
					GzColor tColor;	//color lookup by texture
					float interpolatedU = 0;
					float interpolatedV = 0;
					float u, v;
					//using textures
					if (render->tex_fun)
					{
						interpolatedU = interpolate(x, y, uvInterp[U]);
						interpolatedV = interpolate(x, y, uvInterp[V]);
						u = interpolatedU*(zp + 1);
						v = interpolatedV*(zp + 1);
						result = render->tex_fun(u, v, tColor);
						if (result == GZ_FAILURE)
							break;
					}
					//GOURARD SHADING
					if (render->interp_mode == GZ_COLOR)
					{
						
						if (render->tex_fun)
						{
							//setColor(render->Ka, tColor);
							//setColor(render->Kd, tColor);
							//setColor(render->Ks, tColor);
						
							GzColor ipC;
							ipC[RED] = interpolate(x, y, cInterp[RED]);
							ipC[GREEN] = interpolate(x, y, cInterp[GREEN]);
							ipC[BLUE] = interpolate(x, y, cInterp[BLUE]);

							multVector(ipC, tColor, ipC);
							r = ctoi(ipC[RED]);
							g = ctoi(ipC[GREEN]);
							b = ctoi(ipC[BLUE]);
						}
						else
						{
							r = ctoi(interpolate(x, y, cInterp[RED]));
							g = ctoi(interpolate(x, y, cInterp[GREEN]));
							b = ctoi(interpolate(x, y, cInterp[BLUE]));
						}
						
					}
					//PHONG SHADING
					else if (render->interp_mode == GZ_NORMALS)
					{
						GzColor ipC;
						float interp_n[3];
						interp_n[X] = interpolate(x, y, nInterp[X]);	//nx
						interp_n[Y] = interpolate(x, y, nInterp[Y]);	//ny
						interp_n[Z] = interpolate(x, y, nInterp[Z]);	//nz
						normalizeVector(interp_n);

						//tColor is in screen space
						
						if (render->tex_fun)
						{
							setColor(render->Kd, tColor);
							setColor(render->Ka, tColor);
						}
						
						calColorAtSurf(render, interp_n, ipC);
						r = ctoi(ipC[RED]);
						g = ctoi(ipC[GREEN]);
						b = ctoi(ipC[BLUE]);

					}
					else
					{
						r = ctoi(render->flatcolor[RED]);
						g = ctoi(render->flatcolor[GREEN]);
						b = ctoi(render->flatcolor[BLUE]);

					}
					setColor(interpC, r, g, b);
					GzPutDisplay(render->display, x, y, interpC[RED], interpC[GREEN], interpC[BLUE], 1, interpZ);
				}
			}
		}
	}

	return result;
}

int setInterpolateMode(GzRender* render, int mode)
{
	int result = GZ_SUCCESS;
	switch (mode)
	{
		case GZ_FLAT:
		case GZ_COLOR:
		case GZ_NORMAL:	render->interp_mode = mode;
						break;
		default:		result = GZ_FAILURE;
						break;
	}

	return result;
}

int setSpecPower(GzRender* render, float spec)
{
	if (render == NULL)
		return GZ_FAILURE;
	render->spec = spec;
	return GZ_SUCCESS;
}

int setTexfun(GzRender* render, GzTexture tex_fun)
{
	if (tex_fun != 0)
		render->tex_fun = *tex_fun;
	return GZ_SUCCESS;
}

int runRenderCommand(GzRender* render, int commandType, GzPointer param)
{
	int result = GZ_SUCCESS;
	switch (commandType)
	{
		case GZ_RGB_COLOR:					result = setColor(render->flatcolor, (float*)param);
											break;
		case GZ_AMBIENT_LIGHT:				result = setLight(render->ambientlight, *(GzLight*)param);
											break;
		case GZ_DIRECTIONAL_LIGHT:			result = addLights(render->lights, render->numlights, *(GzLight*)param);
											render->numlights++;
											break;
		
		case GZ_AMBIENT_COEFFICIENT:		result = setColor(render->Ka, (float*)param);
											break;
		case GZ_DIFFUSE_COEFFICIENT:		result = setColor(render->Kd, (float*)param);
											break;
		case GZ_SPECULAR_COEFFICIENT:		result = setColor(render->Ks, (float*)param);
											break;

		case GZ_DISTRIBUTION_COEFFICIENT:	result = setSpecPower(render, *(float*)param);
											break;
		case GZ_INTERPOLATE:				result = setInterpolateMode(render, *(int*)param);
											break;	
		case GZ_TEXTURE_MAP:				result = setTexfun(render, (GzTexture)param);
											break;
		case GZ_AASHIFTX:					render->xOffset = *(float*)param;
											break;
		case GZ_AASHIFTY:				    render->yOffset = *(float*)param;
											break;
		case GZ_WEIGHT:					    render->weight = *(float*)param;
											break;

	}
	return result;
}

int runRenderCommands(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList)
{
	int result = GZ_SUCCESS;
	for (int i = 0; i < numParts; i++)
	{
		result = runRenderCommand(render, nameList[i], valueList[i]);
		if (result == GZ_FAILURE)
			break;
	}
	return result;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	return runRenderCommands(render, numAttributes, nameList, valueList);
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts using matrix on top of stack 
- Clip - just discard any triangle with any vert(s) behind view plane 
       - optional: test for triangles with all three verts off-screen (trivial frustum cull)
- invoke triangle rasterizer  
*/ 
	GzCoord* vertices = NULL;
	GzCoord* normals = NULL;
	GzTextureIndex* textures = NULL;
	for (int i = 0; i < numParts; i++)
	{
		switch (nameList[i])
		{
			case GZ_POSITION:		vertices = (GzCoord*)valueList[i];
									break;
			case GZ_NORMAL:			normals = (GzCoord*)valueList[i];
									break;
			case GZ_TEXTURE_INDEX:	textures = (GzTextureIndex*)valueList[i];
									break;
		}
	}

	return renderTriangle(render, vertices, normals, textures);
	//return runRenderCommands(render, numParts, nameList, valueList);
}

