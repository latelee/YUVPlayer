/**
 * @file   yuv2rgb.c
 * @author Late Lee
 * @date   2012.01.20
 *
 * @brief
 *         YUV420/YUV420×ªRGB24ÊµÏÖ´úÂë£¬Áí¸½²âÊÔº¯Êý
 *
 * @note & @bug
 *         1¡¢ÔÚWindows±àÒë²âÊÔ
 */

#include "stdafx.h" // for MFC
#include "yuv2rgb.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static long U[256], V[256], Y1[256], Y2[256];

void init_yuv422_table(void)
{
    int i;

    // Initialize table
    for (i = 0; i < 256; i++)
    {
        V[i]  = 15938 * i - 2221300;
        U[i]  = 20238 * i - 2771300;
        Y1[i] = 11644 * i;
        Y2[i] = 19837 * i - 311710;
    }
}

void yuv422_to_rgb24(unsigned char *yuvbuffer, unsigned char *rgbbuffer, int width, int height)
{
    unsigned char *pY, *pU, *pV, *pUbase, *pVbase;
    unsigned char *pR, *pG, *pB;
    int i = 0;
    int j = 0;
    long RGB_SIZE = 0;

    RGB_SIZE = width * height * 3;

    pY = yuvbuffer;
    pU = pUbase = yuvbuffer + width * height;
    pV = pVbase = pUbase + width * height / 2;

    for (i = 0; i < height; i++)
    {
        pB = rgbbuffer + RGB_SIZE - 3*width*(i+1);
        pG = rgbbuffer + RGB_SIZE - 3*width*(i+1) + 1;
        pR = rgbbuffer + RGB_SIZE - 3*width*(i+1) + 2;

        for (j=0; j < width; j += 2)
        {
            *pR = MAX (0, MIN (255, (V[*pV] + Y1[*pY])/10000));   //R value
            *pB = MAX (0, MIN (255, (U[*pU] + Y1[*pY])/10000));   //B value
            *pG = MAX (0, MIN (255, (Y2[*pY] - 5094*(*pR) - 1942*(*pB))/10000)); //G value

            pR += 3;
            pB += 3;
            pG += 3;
            pY++;

            *pR = MAX (0, MIN (255, (V[*pV] + Y1[*pY])/10000) );  //R value
            *pB = MAX (0, MIN (255, (U[*pU] + Y1[*pY])/10000) );  //B value
            *pG = MAX (0, MIN (255, (Y2[*pY] - 5094*(*pR) - 1942*(*pB))/10000) ); //G value

            pR += 3;
            pB += 3;
            pG += 3;

            pY++;
            pU++;
            pV++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////

static long int crv_tab[256];
static long int cbu_tab[256];
static long int cgu_tab[256];
static long int cgv_tab[256];
static long int tab_76309[256];
static unsigned char clp[1024];   //for clip in CCIR601

void init_yuv420_table(void)
{
    long int crv,cbu,cgu,cgv;
    int i,ind;

    crv = 104597; cbu = 132201;  /* fra matrise i global.h */
    cgu = 25675;  cgv = 53279;

    for (i = 0; i < 256; i++)
    {
        crv_tab[i] = (i-128) * crv;
        cbu_tab[i] = (i-128) * cbu;
        cgu_tab[i] = (i-128) * cgu;
        cgv_tab[i] = (i-128) * cgv;
        tab_76309[i] = 76309*(i-16);
    }

    for (i = 0; i < 384; i++)
        clp[i] = 0;
    ind = 384;
    for (i = 0;i < 256; i++)
        clp[ind++] = i;
    ind = 640;
    for (i = 0;i < 384; i++)
        clp[ind++] = 255;
}

// yuv420 to rgb24
void yuv420_to_rgb24(unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width,int height)
{
    int y1, y2, u, v;
    unsigned char *py1, *py2;
    int i, j, c1, c2, c3, c4;
    unsigned char *d1, *d2;
    unsigned char *src_u, *src_v;

    long RGB_SIZE = 0;
    unsigned char tmp;

    RGB_SIZE = width * height * 3;

    src_u = yuvbuffer + width * height;   // u
    src_v = src_u + width * height / 4;  // v

    py1 = yuvbuffer;   // y
    py2 = py1 + width;
    d1 = rgbbuffer;
    d2 = d1 + 3 * width;

    for (j = 0; j < height; j += 2)
    {
        for (i = 0; i < width; i += 2)
        {
            u = *src_u++;
            v = *src_v++;

            c1 = crv_tab[v];
            c2 = cgu_tab[u];
            c3 = cgv_tab[v];
            c4 = cbu_tab[u];

            //up-left
            y1 = tab_76309[*py1++];
            *d1++ = clp[384+((y1 + c1)>>16)];
            *d1++ = clp[384+((y1 - c2 - c3)>>16)];
            *d1++ = clp[384+((y1 + c4)>>16)];

            //down-left
            y2 = tab_76309[*py2++];
            *d2++ = clp[384+((y2 + c1)>>16)];
            *d2++ = clp[384+((y2 - c2 - c3)>>16)];
            *d2++ = clp[384+((y2 + c4)>>16)];

            //up-right
            y1 = tab_76309[*py1++];
            *d1++ = clp[384+((y1 + c1)>>16)];
            *d1++ = clp[384+((y1 - c2 - c3)>>16)];
            *d1++ = clp[384+((y1 + c4)>>16)];

            //down-right
            y2 = tab_76309[*py2++];
            *d2++ = clp[384+((y2 + c1)>>16)];
            *d2++ = clp[384+((y2 - c2 - c3)>>16)];
            *d2++ = clp[384+((y2 + c4)>>16)];
        }
        d1  += 3*width;
        d2  += 3*width;
        py1 += width;
        py2 += width;
    }
	// rgb->bgr
    for (i = 0; i < RGB_SIZE; i += 3)
	{
		tmp = rgbbuffer[i];
		rgbbuffer[i] = rgbbuffer[i + 2];
		rgbbuffer[i + 2] = tmp;
	}
}

#ifdef YUV_DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// yuv422
int save_bmp1(const char* yuvfile, const char* bmpfile, int width, int height)
{
    FILE* fp1;
    FILE* fp2;
    int nCount=0;
    int frameSize = 0;
    int picSize = 0;
    int ret = 0;
    int i = 0;
    char* framePtr = NULL;
    char* rgbPtr = NULL;
    long rgbSize = 0;

    static int yuv_init = 0;

    BITMAPFILEHEADER bmHeader;
    BITMAPINFOHEADER bmInfo;

    rgbSize = width * height * 3;
    frameSize = width * height;

    // yuv422: w * h * 2
    // yuv420: w * h * 3 / 2
    picSize   = frameSize * 2;;

    framePtr = (char *) malloc(sizeof(char) * picSize);
    if (framePtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(framePtr, '\0', picSize);

    rgbPtr = (char *) malloc(sizeof(char) * rgbSize);
    if (rgbPtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(rgbPtr, '\0', rgbSize);

    if (yuv_init == 0)
    {
        init_yuv422_table();
        yuv_init = 1;
    }

    if ((fp1 = fopen(yuvfile, "rb")) == NULL )
    {
        printf("open yuv file failed.\n");
        return -1;
    }
    if ((fp2 = fopen(bmpfile, "wb")) == NULL)
    {
        printf("open bmp file failed.\n");
        return -1;
    }

    ret = (int)fread(framePtr, 1, picSize, fp1);

    yuv422_to_rgb24((unsigned char*)framePtr, (unsigned char*)rgbPtr, width, height);

    /////////////////////
    bmHeader.bfType = 'MB';
    bmHeader.bfSize = rgbSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmHeader.bfReserved1 = 0;
    bmHeader.bfReserved2 = 0;
    bmHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmInfo.biSize = sizeof(BITMAPINFOHEADER);
    bmInfo.biWidth = width;
    bmInfo.biHeight = height;
    bmInfo.biPlanes = 1;
    bmInfo.biBitCount = 24;
    bmInfo.biCompression = 0;
    bmInfo.biSizeImage = rgbSize;
    bmInfo.biXPelsPerMeter = 0;
    bmInfo.biYPelsPerMeter = 0;
    bmInfo.biClrUsed = 0;
    bmInfo.biClrImportant = 0;

    fwrite(&bmHeader, 1, sizeof(BITMAPFILEHEADER), fp2);
    fwrite(&bmInfo, 1, sizeof(BITMAPINFOHEADER), fp2);
    fwrite(rgbPtr, 1, rgbSize, fp2);
    fclose(fp1);
    fclose(fp2);

    free(framePtr);
    free(rgbPtr);

    printf("done.\n");
    return 0;
}

// yuv420
int save_bmp2(const char* yuvfile, const char* bmpfile, int width, int height)
{
    FILE* fp1;
    FILE* fp2;
    int nCount=0;
    int frameSize = 0;
    int picSize = 0;
    int ret = 0;
    int i = 0;
    char* framePtr = NULL;
    char* rgbPtr = NULL;
    long rgbSize = 0;
    int n;
    unsigned char tmp;

    BITMAPFILEHEADER bmHeader;
    BITMAPINFOHEADER bmInfo;

    rgbSize = width * height * 3;
    frameSize = width * height;

    // yuv422: w * h * 2
    // yuv420: w * h * 3 / 2
    //picSize   = frameSize * 2; // 3 / 2;
    picSize   = frameSize * 3 / 2;

    framePtr = (char *) malloc(sizeof(char) * picSize);
    if (framePtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(framePtr, '\0', picSize);

    rgbPtr = (char *) malloc(sizeof(char) * rgbSize);
    if (rgbPtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(rgbPtr, '\0', rgbSize);

    if ((fp1 = fopen(yuvfile, "rb")) == NULL )
    {
        printf("open yuv file failed.\n");
        return -1;
    }
    if ((fp2 = fopen(bmpfile, "wb")) == NULL)
    {
        printf("open bmp file failed.\n");
        return -1;
    }

    ret = (int)fread(framePtr, 1, picSize, fp1);

    init_yuv420_table();
    yuv420_to_rgb24((unsigned char *)framePtr, (unsigned char *)rgbPtr, width, height);

    /*
    // rgb --> bgr
    for (n = 0; n < width * height * 3; n += 3)
	{
		tmp = rgbPtr[n];
		rgbPtr[n] = rgbPtr[n + 2];
		rgbPtr[n + 2] = tmp;
	}
    */

    /////////////////////
    bmHeader.bfType = 'MB';
    bmHeader.bfSize = rgbSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmHeader.bfReserved1 = 0;
    bmHeader.bfReserved2 = 0;
    bmHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmInfo.biSize = sizeof(BITMAPINFOHEADER);
    bmInfo.biWidth = width;
    bmInfo.biHeight = -height;    // !!
    bmInfo.biPlanes = 1;
    bmInfo.biBitCount = 24;
    bmInfo.biCompression = 0;
    bmInfo.biSizeImage = rgbSize;
    bmInfo.biXPelsPerMeter = 0;
    bmInfo.biYPelsPerMeter = 0;
    bmInfo.biClrUsed = 0;
    bmInfo.biClrImportant = 0;

    fwrite(&bmHeader, 1, sizeof(BITMAPFILEHEADER), fp2);
    fwrite(&bmInfo, 1, sizeof(BITMAPINFOHEADER), fp2);
    fwrite(rgbPtr, 1, rgbSize, fp2);
    fclose(fp1);
    fclose(fp2);

    free(framePtr);
    free(rgbPtr);

    printf("done.\n");
    return 0;
}

int yuv_test(int argc, char* argv[])
{
    //save_bmp1("20111231100113.yuv", "0.bmp", 1600, 1200);
    save_bmp2("akiyo_qcif.yuv", "0.bmp", 176, 144);
    return 0;
}
#endif
