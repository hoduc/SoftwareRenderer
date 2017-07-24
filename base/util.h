#include "stdio.h"
#include "gz.h"
#include "disp.h"

#define MAXINT INT_MAX
#define LOWERBOUND 0
#define UPPERBOUND 1 << 31
#define  W 3 // add-on
//probably redundant
//index accessing the edge
#define A 0
#define B 1
#define C 2

extern GzMatrix IDENTITY_MATRIX;


void negate(float a[3], float c[3]);
void sub(float a[3], float b[3], float c[3]);
void add(float a[3], float b[3], float c[3]);
void crossProduct(float a[3], float b[3], float c[3]);
float dotProduct(float a[3], float b[3]);
//float dotProduct(float a[4], float b[4]);

void setF3(float a[3], float f1, float f2, float f3);

//color-related
short	ctoi(float color);		/* convert float color to GzIntensity short */
unsigned int clamp(int v, int lowerBound, int upperBound);
float clamp(float v, float lowerBound, float upperBound);
GzIntensity clampIntensity(unsigned int intensity);
GzIntensity genIntensity(int v);
int clampInt(int v, int maxVal);
unsigned char rgb(GzIntensity intensity);
int setColor(GzColor rcolor, GzColor color);
int setColor(GzColor rcolor, float r, float g, float b);


//vector & point
void crossProduct(float a[3], float b[3], float c[3]);
void crossProduct(GzCoord v1, GzCoord v2, GzCoord v);
GzCoord* crossProductV(GzCoord v1, GzCoord v2);
float dotProductV(GzCoord v1, GzCoord v2);
float dotProductHomo(float vHomo1[4], float vHomo2[4]);

int setCoord(GzCoord a, float x, float y, float z);
int setCoord(GzCoord a, GzCoord b);
void negateVector(GzCoord a);
void addVector(GzCoord a, GzCoord b, GzCoord c);
void subVector(GzCoord a, GzCoord b, GzCoord c);
void multVector(GzCoord a, GzCoord b, GzCoord c);
void scalarMultVector(GzCoord a, float k);
bool isLeftOrTopEdge(GzCoord vt);
void edgeFn(GzCoord v1, GzCoord v2, GzCoord e);
void edgeVector(GzCoord v1, GzCoord v2, GzCoord v);
float vectorLength(GzCoord v);
void normalizeVector(GzCoord v);
void v3dToHomo(GzCoord v, float vHomo[4]);
void vHomoTo3d(float vHomo[4], GzCoord v);


//matrix
void row(GzMatrix mat, int rowIndex, GzCoord v);
void setRow(GzMatrix mat, int rowIndex, GzCoord v, float w);
void clearMatrix(GzMatrix mat);
void setMatrix(GzMatrix mata, GzMatrix matb);
void matrixMult(GzMatrix mata, GzMatrix matb, GzMatrix matc);
void matrixMultVHomo(GzMatrix mat, float vHomo[4], float vHomoRet[4]);
void matrixMultV3d(GzMatrix mat, GzCoord v, GzCoord vRet);
void matrixScalarMultRange(GzMatrix mata, float k, GzMatrix matb, int row, int col);
void matrixScalarMult(GzMatrix mata, float k, GzMatrix matb);
void transpose(GzMatrix mata, GzMatrix matb);


//light
int setLight(GzLight& dest, GzLight src);
int setLight(GzLight& light, GzCoord dir, GzCoord color);
int addLights(GzLight lights[], int topLightIndex, GzLight light);

//general
bool inRangeInclusive(int a0, int a, int an);
int index(int x, int y, int maxX);

float MINF(float a, float b);
float MAXF(float a, float b);

bool LEF(float a, float b);
bool GEF(float a, float b);
int ABSI(int a);

GzPixel* getPixelAt(GzDisplay* display, int i, int j);