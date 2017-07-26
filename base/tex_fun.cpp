/* Texture functions for cs580 GzLib	*/
//#include    "stdafx.h"
#include    "pch.h"
#include	"stdio.h"
#include    "math.h"
#include	"Gz.h"
#include	"util.h"

GzColor	*image=NULL;
int xs, ys;
int reset = 1;

#define ITERATION  100
#define ESC_MAG 300
#define RANGE 5000.0
#define JULIA_C_R -0.12375
#define JULIA_C_I 0.56805

typedef float COMPLEX[2];
#define R 0
#define I 1

void complexAdd(COMPLEX a, COMPLEX b, COMPLEX c)
{
	c[R] = a[R] + b[R];
	c[I] = a[I] + b[I];
}

void complexMult(COMPLEX a, COMPLEX b, COMPLEX c)
{
	c[R] = a[R] * b[R] - a[I] * b[I];
	c[I] = a[R] * b[I] + a[I] * b[R];
}

void bilinInterpolate(GzColor* image, float s, float t, int t1x, int t1y, int t2x, int t2y, int b1x, int b1y, int b2x, int b2y, GzColor retColor)
{
	float topLeftCoeff = (1 - s)*(1 - t);
	float topRightCoeff = s*(1 - t);
	float bottomLeftCoeff = (1 - s)*t;
	float bottomRightCoeff = s*t;

	for (int i = 0; i < 3; i++)
	{
		retColor[i] =	topLeftCoeff*image[index(t1x, t1y, xs)][i]		+
						topRightCoeff*image[index(t2x, t2y, xs)][i]		+
						bottomLeftCoeff*image[index(b1x, b1y, xs)][i]	+
						bottomRightCoeff*image[index(b2x, b2y, xs)][i]
		;
	}

}

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  std::string cfp(__FILE__);
  size_t lastSlash = cfp.find_last_of(SEPARATOR);
  if (lastSlash){
      cfp = cfp.substr(0,lastSlash+1);
  }
  if (reset) {          /* open and load texture file */
    fd = fopen ((cfp +  TEXFILE).c_str(), "rb");
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
  clamp(u, 0.0, 1.0);
  clamp(v, 0.0, 1.0);
  /*
  if (u < 0 || u > 1 || v < 0 || v > 1)
	  return GZ_FAILURE;*/
  u = u*xs-1;
  v = v*ys-1;
  float xmin = floor(u);
  float xmax = ceil(u);
  float ymin = floor(v);
  float ymax = ceil(v);

  float s = u - xmin;
  float t = v - ymin;
  bilinInterpolate(image, s, t, 
	  xmin, ymin,	/* top left */
	  xmax, ymin,	/* top right */
	  xmin, ymax,  /* bottom left */
	  xmax, ymax,  /* bottom right*/
	  color);
  return GZ_SUCCESS;
}


GzColor clut[] = {
	{0, 7, 100},
	{ 4, 4, 73 },
	{ 9, 1, 47 },
	{ 12, 44, 138 },
	{ 24, 82, 177 },
	{ 25, 7, 26 },
	{ 57, 125, 209 },
	{66,30,15},
	{134, 181, 229},
	{211, 236, 248},
	{241, 233, 191},
	{248, 201, 95},
	{255, 170, 0},
	{204, 128, 0},
	{153, 87, 0},
	{106, 52, 3},
	
};

float rgbToFloat(int f)
{
	return (f % 256) / 255.0;
}

void rgbToFloat(GzColor* rgbs, int size)
{
	for (int i = 0; i < size; i++)
	{
		float r = rgbToFloat(int(rgbs[i][0]));
		float g = rgbToFloat(int(rgbs[i][1]));
		float b = rgbToFloat(int(rgbs[i][2]));

		setColor(rgbs[i], r, g, b);
	}
}

void frac_fn1(COMPLEX x_iter, COMPLEX c, float& r, float& i)
{
	r = x_iter[R] * x_iter[R] - x_iter[I] * x_iter[I] + c[R];
	i = 2 * x_iter[R] * x_iter[I] + c[I];
}

void frac_fn2(COMPLEX x_iter, COMPLEX c, float& r, float& i)
{
	r = x_iter[R] * x_iter[R] * x_iter[R]  - x_iter[I] * x_iter[I] *x_iter[I]+ c[R];
	i = 3 * x_iter[R] * x_iter[R] + c[I];
}

void fractal(COMPLEX x_iter, COMPLEX c, int& i_ter, float& lx_iter)
{
	while (i_ter < ITERATION)
	{
		float r = 0;
		float i = 0;
		frac_fn1(x_iter, c, r, i);
		//float r = x_iter[R] * x_iter[R] - x_iter[I] * x_iter[I] + c[R];
		//float i = 2 * x_iter[R] * x_iter[I] + c[I];

		lx_iter = sqrt(r*r + i*i);
		if (lx_iter > ESC_MAG)
			break;

		x_iter[R] = r;
		x_iter[I] = i;
		i_ter++;
	}
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	clamp(u, 0.0, 1.0);
	clamp(v, 0.0, 1.0);
	setColor(color, 0, 0, 0);
	xs = 109;
	ys = 255;
	u = u*xs - 1;
	v = v*ys - 1;

	if (reset)
	{
		rgbToFloat(clut, sizeof(clut) / sizeof(clut[0]));
		reset = 0;
	}

	COMPLEX x_iter{ (u - xs / 2) / (xs / 2), (v - ys / 2) / (ys / 2) };
	//COMPLEX c{ JULIA_C_R, JULIA_C_I };
	//COMPLEX c{ 0.285, 0.01 };
	//COMPLEX c{ 0.279, 0.0 };
	COMPLEX c{ 0.0, -0.621 };

	int i_ter = 0;
	float lx_iter = 0;
	
	fractal(x_iter, c, i_ter, lx_iter);
	
	//if (i_ter < ITERATION)
	//{
		//setColor(color, 0.5, 0, 0);
	
		lx_iter = clamp(lx_iter, 0.0, RANGE);
		float diff = RANGE / ((sizeof(clut) / sizeof(clut[0])) - 1);
		int i = floor(lx_iter / diff);
		int i_1 = ceil(lx_iter / diff);
		if (i == i_1)
		{
			setColor(color, clut[i]);
		}
		else
		{
			float si = i*diff;
			float si_1 = i_1 * diff;

			float a = (si_1 - lx_iter) / (si_1 - si);
			float b = (lx_iter - si) / (si_1 - si);
			GzColor ci;
			GzColor ci_1;
			setColor(ci, clut[i]);
			setColor(ci_1, clut[i_1]);
			scalarMultVector(ci, a);
			scalarMultVector(ci_1, b);
			add(ci, ci_1, color);
		}
		
		
		
		/*
		color[RED] = sin(float(i_ter) / 2.0);
		color[GREEN] = cos(float(i_ter)*0.05);
		color[BLUE] = cos(float(i_ter) / 3.0 + 3.14 / 4.0);
		*/
		
	//}
	//else
	//{
		//setColor(color, 1, 0, 0);
	//}
	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

