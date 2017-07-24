//#include "stdafx.h"
#include "disp.h"
#include "util.h"
#include "float.h"
#include "math.h"
#include "rend.h"

//identity matrix
extern GzMatrix IDENTITY_MATRIX =
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

/*
float* distance(float a[3], float b[3])
{
	float c[3];
	c[0] = b[0] - a[0];
	c[1] = b[1] - a[1];
	c[2] = b[2] - a[2];
	return c;
}
*/

void crossProduct(float a[3], float b[3], float c[3])
{
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];

}

void multVector(GzCoord a, GzCoord b, GzCoord c)
{
	setCoord(c, a[0] * b[0], a[1] * b[1], a[2] * b[2]);
}

void negate(float a[3], float c[3])
{
	c[0] = -a[0];
	c[1] = -a[1];
	c[2] = -a[2];
}

void add(float a[3], float b[3], float c[3])
{
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];
}

void sub(float a[3], float b[3], float c[3])
{
	negate(a, c);
	add(c, b, c);
}

float dotProductRange(float* a, float* b, int dim)
{
	float result = 0;
	for (int i = 0; i < dim; i++)
		result += a[i] * b[i];
	return result;
}

float dotProduct(float a[3], float b[3])
{
	return dotProductRange(a, b, 3);
}

/*
float dotProduct(float a[4], float b[4])
{
	return dotProductRange(a, b, 4);
}
*/

void setF3(float a[3], float f1, float f2, float f3)
{
	a[0] = f1;
	a[1] = f2;
	a[2] = f3;
}

/* Helper function
*/

/*
General clamp
*/
unsigned int clamp(int v, int lowerBound, int upperBound)
{
	if (v < lowerBound)
		return lowerBound;
	if (v > upperBound)
		return upperBound;
	return v;

}

GzIntensity clampIntensity(unsigned int intensity)
{
	return (GzIntensity)clamp(intensity, LOWERBOUND, (1 << 12) - 1);
}

/* Generate 8-bit color
*/
GzIntensity genIntensity(int v)
{
	return (GzIntensity)(clampIntensity(v) << 4);
}

int clampInt(int v, int maxVal = UPPERBOUND)
{
	return clamp(v, 0, maxVal);
}

float clamp(float v, float lowerBound, float upperBound)
{
	if (v < lowerBound)
		return lowerBound;
	if (v > upperBound)
		return upperBound;
	return v;
}

/*
Get the rgb value, strip off alpha
*/
unsigned char rgb(GzIntensity intensity)
{
	return (unsigned char)(intensity >> 4);
}


short	ctoi(float color)		/* convert float color to GzIntensity short */
{
	return(short)((int)(color * ((1 << 12) - 1)));
}

int setColor(GzColor rcolor, float r, float g, float b)
{
	if (rcolor == NULL)
		return GZ_FAILURE;
	rcolor[RED] = r;
	rcolor[GREEN] = g;
	rcolor[BLUE] = b;
	return GZ_SUCCESS;
}



/*
	Set a color
*/
int setColor(GzColor rcolor, GzColor color)
{

	if (color == NULL)
	{
		return GZ_FAILURE;
	}

	return setColor(rcolor, color[RED], color[GREEN], color[BLUE]);
}


/*
Set a 3 coords
*/
int setCoord(GzCoord a, float x, float y, float z)
{
	if (a == NULL)
		return GZ_FAILURE;
	a[X] = x;
	a[Y] = y;
	a[Z] = z;

	return GZ_SUCCESS;
}

/*
	Set a with 3 coord of b
*/
int setCoord(GzCoord a, GzCoord b)
{
	return setCoord(a, b[X], b[Y], b[Z]);
}

void negateVector(GzCoord a)
{
	a[X] = -a[X];
	a[Y] = -a[Y];
	a[Z] = -a[Z];
}

void addVector(GzCoord a, GzCoord b, GzCoord c)
{
	c[X] = a[X] + b[X];
	c[Y] = a[Y] + b[Y];
	c[Z] = a[Z] + b[Z];
}

void subVector(GzCoord a, GzCoord b, GzCoord c)
{
	//setCoord(c, b);
	//negateVector(c);
	GzCoord bn;
	setCoord(bn, b);
	negateVector(bn);
	addVector(a, bn, c);
}

void scalarMultVector(GzCoord a, float k)
{
	setCoord(a, a[X] * k, a[Y] * k, a[Z] * k);
}

//vt : edges
bool isLeftOrTopEdge(GzCoord vt)
{
	//left edge : inc y
	//top edge : y == 0 but x inc
	return (vt[Y] == 0 && vt[X] > 0) || vt[Y] < 0;
}

void edgeFn(GzCoord v1, GzCoord v2, GzCoord e)
{
	setCoord(e, v1[Y] - v2[Y], v2[X] - v1[X], v1[X] * v2[Y] - v2[X] * v1[Y]);
}

/*
	V = v2 - v1 
	v2 : tail
	v1 : head
*/
void edgeVector(GzCoord v1, GzCoord v2, GzCoord v)
{
	setCoord(v, v2[X] - v1[X], v2[Y] - v1[Y], v2[Z] - v1[Z]);
}

float vectorLength(GzCoord v)
{
	return sqrt(v[X] * v[X] + v[Y] * v[Y] + v[Z] * v[Z]);
}

void normalizeVector(GzCoord v)
{
	float vLen = vectorLength(v);
	v[X] /= vLen;
	v[Y] /= vLen;
	v[Z] /= vLen;
}

/*
GzCoord* negateV(GzCoord v)
{
	return (GzCoord*)negate(v);
}

GzCoord* addV(GzCoord v1, GzCoord v2)
{
	return (GzCoord*)add(v1, v2);
}

GzCoord* subV(GzCoord v1, GzCoord v2)
{
	return (GzCoord*)sub(v1, v2);
}

GzCoord* scalarMultV(GzCoord v, float k)
{
	return (GzCoord*)mult(v,k);
}

GzCoord* edgeFn(GzCoord v1, GzCoord v2)
{
	return (GzCoord*)edgeEqn(v1, v2);
}

float lenV(GzCoord v)
{
	return len(v);
}

GzCoord* normV(GzCoord v)
{
	return (GzCoord*)norm(v);
}

GzCoord* v3dToV4d(GzCoord v)
{
	return (GzCoord*)_3_to_4(v, 1);
}

float* v4dTOV3d(GzCoord v)
{
	return (GzCoord*)_4_to_3(v);
}
*/

/*
	v1.v2 = v1x*v2x + v1y*v2y + v1z*v2z 
*/
/*
float dotProduct(GzCoord v1, GzCoord v2)
{
	return v1[X] * v2[X] + v1[Y] * v2[Y] + v1[Z] * v2[Z];
}
*/


/*
	v = v1xv2
*/
/*
void crossProduct(GzCoord v1, GzCoord v2, GzCoord v)
{
	setCoord(v,
		v1[Y] * v2[Z] - v1[Z] * v2[Y],
		v1[Z] * v2[X] - v1[X] * v2[Z],
		v1[X] * v2[Y] - v1[Y] * v2[X]
		);
}
*/

/*
GzCoord* crossProductV(GzCoord v1, GzCoord v2)
{
	return (GzCoord*)crossProduct(v1, v2);
}
*/

void clearMatrix(GzMatrix mat)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i][j] = 0;
}

void setMatrix(GzMatrix mata, GzMatrix matb)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mata[i][j] = matb[i][j];
}

void transpose(GzMatrix mata, GzMatrix matb)
{
	for (int i = 0; i < 4; i++ )
		for (int j = 0; j < 4; j++)
			matb[i][j] = mata[j][i];
}

void setRow(GzMatrix mat, int rowIndex, GzCoord v, float w)
{
	mat[rowIndex][X] = v[X];
	mat[rowIndex][Y] = v[Y];
	mat[rowIndex][Z] = v[Z];
	mat[rowIndex][W] = w;
}

/*
	Get row -vector
*/
/*
void row(GzMatrix mat, int rowIndex, GzCoord v)
{
	setCoord(v, mat[rowIndex][X], mat[rowIndex][Y], mat[rowIndex][Z]);
}
*/

void v3dToHomo(GzCoord v, float vHomo[4])
{
	vHomo[0] = v[X];
	vHomo[1] = v[Y];
	vHomo[2] = v[Z];
	vHomo[3] = 1;
}

void vHomoTo3d(float vHomo[4], GzCoord v)
{
	setCoord(v, vHomo[X] / vHomo[W], vHomo[Y] / vHomo[W], vHomo[Z] / vHomo[W]);
}

float dotProductHomo(float vHomo1[4], float vHomo2[4])
{
	float res = 0;
	for (int i = 0; i < 4; i++)
		res += vHomo1[i] * vHomo2[i];
	return res;
}



void matrixMult(GzMatrix mata, GzMatrix matb, GzMatrix matc)
{
	GzMatrix tMatb;
	transpose(matb, tMatb);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matc[i][j] = dotProductHomo(mata[i], tMatb[j]);//dotProduct(arow, brow) + mata[i][W] * transposeMatb[j][W];
		}
			
	}
}

void matrixMultVHomo(GzMatrix mat, float vHomo[4], float vHomoRet[4])
{
	for (int i = 0; i < 4; i++)
		vHomoRet[i] = dotProductHomo(mat[i], vHomo);
}

void matrixMultV3d(GzMatrix mat, GzCoord v, GzCoord vRet)
{
	float vHomo[4];
	float vRetHomo[4];
	v3dToHomo(v, vHomo);
	matrixMultVHomo(mat, vHomo, vRetHomo);
	vHomoTo3d(vRetHomo, vRet);
}

void matrixScalarMultRange(GzMatrix mata, float k, GzMatrix matb, int row, int col)
{
	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; j++)
			matb[i][j] = mata[i][j] * k;
}

void matrixScalarMult(GzMatrix mata, float k, GzMatrix matb)
{
	matrixScalarMultRange(mata, k, matb, 4, 4);
}

int setLight(GzLight& light, GzCoord dir, GzCoord color)
{
	if (dir == NULL || color == NULL)
		return GZ_FAILURE;
	setCoord(light.direction, dir);
	setCoord(light.color, color);
	return GZ_SUCCESS;
}

int setLight(GzLight& dest, GzLight src)
{
	return setLight(dest, src.direction, src.color);
}

int addLights(GzLight lights[], int topLightIndex, GzLight light)
{
	if (lights == NULL || topLightIndex == MAX_LIGHTS)
		return GZ_FAILURE;
	return setLight(lights[topLightIndex], light);
	//return GZ_SUCCESS;
}

int index(int x, int y, int maxX)
{
	return x + y*maxX;
}
/*
	  0 <= a <= an ?
*/
bool inRangeInclusive(int a0, int a, int an)
{
	return a >= a0 && a <= an;
}

float MINF(float a, float b)
{
	return (a < b) ? a : b;
}

float MAXF(float a, float b)
{
	return (a > b) ? a : b;
}

#define EPSILON 0.000001
bool LEF(float a, float b)
{
	if (a - b < EPSILON && fabs(a - b) > EPSILON)
		return true;
	return false;
}

bool GEF(float a, float b)
{
	return LEF(b, a);
}

int ABSI(int a)
{
	return (a < 0) ? -a : a;
}


/*
	Get pixel struct at (i,j) pixel screen
	Return NULL pointer if (i,j) outside the screen width*height
*/
GzPixel* getPixelAt(GzDisplay* display, int i, int j)
{
	int pixelIndex = ARRAY(i, j);
	if (inRangeInclusive(0, i, display->xres - 1) && inRangeInclusive(0, j, display->yres - 1) && inRangeInclusive(0, pixelIndex, (display->xres*display->yres)-1))
		return &(display->fbuf[ARRAY(i, j)]);
	return NULL;
}