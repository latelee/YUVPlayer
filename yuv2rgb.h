/**
 * @file   yuv2rgb.c
 * @author Late Lee
 * @date   2012.01.20
 * 
 * @brief  
 *         YUV420/YUV420转RGB24实现代码，另附测试函数
 *
 * @note & @bug
 *         1、在Windows编译测试
 */

#ifndef _YUV2RGB_H_
#define _YUV2RGB_H_

typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef long            LONG;
typedef unsigned int    DWORD32;

//#define YUV_DEBUG

#ifdef YUV_DEBUG
#pragma pack(push)
// 2字节对齐，共14
#pragma pack(2)
typedef struct tagBITMAPFILEHEADER1 {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

// 40
typedef struct tagBITMAPINFOHEADER1{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void init_yuv422_table(void);
void yuv422_to_rgb24(unsigned char *yuvbuffer, unsigned char *rgbbuffer, int width, int height);

void init_yuv420_table(void);
void yuv420_to_rgb24(unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width, int height);

#ifdef YUV_DEBUG
// yuv422 to rgb24
int save_bmp1(const char* yuvfile, const char* bmpfile, int width, int height);

// yuv420 to rgb24
int save_bmp2(const char* yuvfile, const char* bmpfile, int width, int height);

int yuv_test(int argc, char* argv[]);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _YUV2RGB_H_ */