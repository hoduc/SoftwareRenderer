/*   CS580 HW1 display functions to be completed   */

//#include   "stdafx.h"
#include	"Gz.h"
#include	"disp.h"
#include	"util.h"
#include <iostream> // for cerr
using namespace std;
// constant
//
#define R_BG 128
#define G_BG 128
#define B_BG 128

#define RED_BG genIntensity(R_BG)
#define GREEN_BG genIntensity(G_BG)
#define BLUE_BG genIntensity(B_BG)

int w_fbuf_width;
int w_fbuf_height;


void writeByte(unsigned char b, FILE* f)
{
	fwrite(&b, 1, sizeof(b), f);
}



int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- pass back pointer 
 */
	//normalize width, height
	
	int _width = clampInt(width, MAXXRES);
	int _height = clampInt(height, MAXYRES);

	w_fbuf_width = _width;
	w_fbuf_height = _height;
	
	*framebuffer = new char[3 * _width * _height];
	if (*framebuffer == NULL)
		return GZ_FAILURE;
	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, int xRes, int yRes)
{
/* HW1.2 create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/
	*display = new GzDisplay();
	if (!*display)
		return GZ_FAILURE;
	//cannot get past maxxres * maxyres
	(*display)->xres = clampInt(xRes, MAXXRES);
	(*display)->yres = clampInt(yRes, MAXYRES);
	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* HW1.3 clean up, free memory */
	if (display != NULL)
	{
		if (display->fbuf != NULL)
		{
			delete[] display->fbuf;
			display->fbuf = NULL;
		}
		
		if (display->fbuf != NULL)
		{
			cerr << "Error deallocating display framebuffer!!!\n";
			return GZ_FAILURE;
		}
		
		delete display;
		display = NULL;
	}
	if (display != NULL)
	{
		cerr << "Error deallocating display!!!\n";
		return GZ_FAILURE;
	}	
	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
/* HW1.4 pass back values for a display */
	if (display == NULL)
	{
		cerr << "display is not allocated\n";
		return GZ_FAILURE;
	}
	*xRes = display->xres;
	*yRes = display->yres;
	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* HW1.5 set everything to some default values - start a new frame */
	int displaySize = display->xres*display->yres;
	display->fbuf = new GzPixel[displaySize];
	if (display->fbuf == NULL)
	{
		cerr << "Error Allocating display framebuffer with the following resolution" << display->xres << " x " << display->yres << "\n";
		return GZ_FAILURE;
	}
	for (int i = 0; i < displaySize; i++ )
	{
		GzPixel* pixel = &display->fbuf[i];
		pixel->red = RED_BG;
		pixel->green = GREEN_BG;
		pixel->blue = BLUE_BG;
		pixel->alpha = 1;
		pixel->z = INT_MAX;//MAXINT;
	}
	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.6 write pixel values into the display */
	if (display == NULL)
	{
		cerr << "display is not allocated\n";
		return GZ_FAILURE;
	}
	GzPixel* pixel = getPixelAt(display, i, j);
	if (pixel != NULL)
	{
		pixel->red = clampIntensity(r);
		pixel->green = clampIntensity(g);
		pixel->blue = clampIntensity(b);
		pixel->alpha = clampIntensity(a);
		pixel->z = z;
	}
	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.7 pass back a pixel value to the display */
	if (display == NULL)
	{
		cerr << "display not allocated\n";
		return GZ_FAILURE;
	}
	GzPixel* pixel = getPixelAt(display, i, j);
	if (pixel == NULL)
	{
		cerr << "Pixel at [" << i << "," << j << "] not available\n";
		return GZ_FAILURE;
	}
	*r = pixel->red;
	*g = pixel->green;
	*b = pixel->blue;
	*a = pixel->alpha;
	*z = pixel->z;
	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\r" */
	if (display == NULL)
	{
		cerr << "display not allocated\n";
		return GZ_FAILURE;
	}
		
	fprintf(outfile, "P6 %d %d 255\r", display->xres, display->yres);
	int fbuf_size = display->xres*display->yres;
	for (int buf_index = 0; buf_index < fbuf_size; buf_index++)
	{
		GzPixel pixel = display->fbuf[buf_index];
		writeByte(rgb(pixel.red), outfile);
		writeByte(rgb(pixel.green), outfile);
		writeByte(rgb(pixel.blue), outfile);
	}
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

/* HW1.9 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	if (framebuffer == NULL)
	{
		cerr << "Window frame buffer is not allocated\n";
		return GZ_FAILURE;
	}
	
	if (display == NULL)
	{
		cerr << "Display is not allocated\n";
		return GZ_FAILURE;
	}

	if (display->fbuf == NULL)
	{
		cerr << "Display frame buffer is not allocated\n";
		return GZ_FAILURE;
	}
	
	int d_fbuf = display->xres*display->yres;
	//what if the display fb and windows fb are not of the same size ?
	int w_dbuf = w_fbuf_width * w_fbuf_height;
	if (w_dbuf != d_fbuf)
	{
		cerr << "windows framebuffer are different from display frame buffer\n";
		return GZ_FAILURE;
	}
	for (int i = 0, j = 0; i < d_fbuf; i++, j+=3)
	{
		GzPixel pixel = display->fbuf[i];
		framebuffer[j] = rgb(pixel.blue);
		framebuffer[j + 1] = rgb(pixel.green);
		framebuffer[j + 2] = rgb(pixel.red);
	}
	return GZ_SUCCESS;
}