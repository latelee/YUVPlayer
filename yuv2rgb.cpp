/**
代码使用p[0]这类的形式，是因为这类的表现方式便于分析格式
*/
#include "stdafx.h" // for MFC

#include <stdio.h>
#include "yuv2rgb.h"


#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static long U[256], V[256], Y1[256], Y2[256];

void init_yuv422p_table(void)
{
    int i;
    static int init = 0;
    if (init == 1) return;
    // Initialize table
    for (i = 0; i < 256; i++)
    { 
        V[i]  = 15938 * i - 2221300;
        U[i]  = 20238 * i - 2771300;
        Y1[i] = 11644 * i;
        Y2[i] = 19837 * i - 311710;
    }

    init = 1;
}

/**
内存分布
                    w
            +--------------------+
            |Y0Y1Y2Y3...         |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
            |U0U1      |
            |...       |   h
            |...       |
            |          |
            +----------+
            |V0V1      |
            |...       |  h
            |...       |
            |          |
            +----------+
                w/2
*/
void yuv422p_to_rgb24(YUV_TYPE type, unsigned char* yuv422p, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb;

    p_y = yuv422p;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 2;

    if (type == FMT_YV16)
    {
        p_v = p_y + width * height;
        p_u = p_u + width * height / 2;
    }
    p_rgb = rgb;

    init_yuv422p_table();

    for (i = 0; i < width * height / 2; i++)
    {
        y  = p_y[0];
        cb = p_u[0];
        cr = p_v[0];

        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        // 此处可调整RGB排序，BMP图片排序为BGR
        // 默认排序为：RGB
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        y  = p_y[1];
        cb = p_u[0];
        cr = p_v[0];
        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        p_y += 2;
        p_u += 1;
        p_v += 1;
        p_rgb += 6;
    }
}

/**
内存分布
                    w
            +---------------------+
            |Y0U0V0Y1U1V1Y2U2V2...|
            |...                  |   h
            |...                  |
            |                     |
            |YxUxVxYxUxVx         |
            |...                  |   h
            |...                  |
            |                     |
            +---------------------+
                
转换：
u0y0v0 ->r0 g0 b0
u0y1v0 -> r1 g1 b1
其它类型类似...
*/
void yuv422packed_to_rgb24(YUV_TYPE type, unsigned char* yuv422p, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p;
    unsigned char* p_rgb;

    p = yuv422p;

    p_rgb = rgb;

    init_yuv422p_table();

    for (i = 0; i < width * height / 2; i++)
    {
        switch(type)
        {
        case FMT_YUYV:
            y  = p[0];
            cb = p[1];
            cr = p[3];
            break;
        case FMT_YVYU:
            y  = p[0];
            cr = p[1];
            cb = p[3];
            break;
        case FMT_UYVY:
            cb = p[0];
            y  = p[1];
            cr = p[2];
            break;
        case FMT_VYUY:
            cr = p[0];
            y  = p[1];
            cb = p[2];
            break;
        default:
            break;
        }

        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        // 此处可调整RGB排序，BMP图片排序为BGR
        // 默认排序为：RGB
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        switch(type)
        {
        case FMT_YUYV:
        case FMT_YVYU:
            y = p[2];
            break;
        case FMT_UYVY:
        case FMT_VYUY:
            y = p[3];
            break;
        default:
            break;
        }

        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        p += 4;
        p_rgb += 6;
    }
}

/**
内存分布
                    w
            +--------------------+
            |Y0Y1Y2Y3...         |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
            |U0V0U1V1            |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
                w/2
UV交织为NV16，VU交织为NV61
可以与上一函数合并，但方便查看，还是不合并
*/
void yuv422sp_to_rgb24(YUV_TYPE type, unsigned char* yuv422sp, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_uv;
    unsigned char* p_rgb;

    p_y = yuv422sp;
    p_uv = p_y + width * height;    // uv分量在Y后面
    p_rgb = rgb;

    init_yuv422p_table();

    for (i = 0; i < width * height / 2; i++)
    {
        y  = p_y[0];
        if (type ==  FMT_NV16)
        {
            cb = p_uv[0];
            cr = p_uv[1];    // v紧跟u，在u的下一个位置
        }
        if (type == FMT_NV61)
        {
            cr = p_uv[0];
            cb = p_uv[1];    // u紧跟v，在v的下一个位置
        }

        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        // 此处可调整RGB排序，BMP图片排序为BGR
        // 默认排序为：RGB
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        y  = p_y[1];
        if (type ==  FMT_NV16)
        {
            cb = p_uv[0];
            cr = p_uv[1];
        }
        if (type ==  FMT_NV61)
        {
            cr = p_uv[0];
            cb = p_uv[1];
        }

        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        p_y += 2;
        p_uv += 2;
        p_rgb += 6;
    }
}
////////////////////////////////////////////////////////////////////////////

static long int crv_tab[256];   
static long int cbu_tab[256];   
static long int cgu_tab[256];   
static long int cgv_tab[256];   
static long int tab_76309[256]; 
static unsigned char clp[1024];   //for clip in CCIR601   

void init_yuv420p_table() 
{   
    long int crv,cbu,cgu,cgv;   
    int i,ind;      
    static int init = 0;

    if (init == 1) return;

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

    init = 1;
}

/**
内存分布
                    w
            +--------------------+
            |Y0Y1Y2Y3...         |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
            |U0U1      |
            |...       |   h/2
            |...       |
            |          |
            +----------+
            |V0V1      |
            |...       |  h/2
            |...       |
            |          |
            +----------+
                w/2
 */
void yuv420p_to_rgb24(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width,int height)   
{
    int y1, y2, u, v;    
    unsigned char *py1, *py2;   
    int i, j, c1, c2, c3, c4;   
    unsigned char *d1, *d2;   
    unsigned char *src_u, *src_v;
    static int init_yuv420p = 0;
    
    src_u = yuvbuffer + width * height;   // u
    src_v = src_u + width * height / 4;  //  v

    if (type == FMT_YV12)
    {
        src_v = yuvbuffer + width * height;   // v
        src_u = src_u + width * height / 4;  //  u
    }
    py1 = yuvbuffer;   // y
    py2 = py1 + width;   
    d1 = rgbbuffer;   
    d2 = d1 + 3 * width;   

    init_yuv420p_table();

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
}

// 失真很严重，不懂为什么
void yuv420p_to_rgb24_bad(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width,int height)   
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb;

    p_y = yuvbuffer;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 4;

    if (type == FMT_YV12)
    {
        p_v = p_y + width * height;
        p_u = p_u + width * height / 4;
    }
    p_rgb = rgbbuffer;

    init_yuv422p_table();

    for (i = 0; i < width * height / 4; i++)
    {
        y  = p_y[0];
        cb = p_u[0];
        cr = p_v[0];

        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        ////////////////////////////////////////////
        y  = p_y[1];
        cb = p_u[0];
        cr = p_v[0];
        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        ////////////////////////////////////////////
        y  = p_y[2];
        cb = p_u[0];
        cr = p_v[0];

        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        p_rgb[6] = r;
        p_rgb[7] = g;
        p_rgb[8] = b;

        ////////////////////////////////////////////
        y  = p_y[3];
        cb = p_u[0];
        cr = p_v[0];
        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        p_rgb[9] = r;
        p_rgb[10] = g;
        p_rgb[11] = b;

        p_y += 4;
        p_u += 1;
        p_v += 1;
        p_rgb += 12;
    }
}

void yuv420sp_to_rgb24(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width,int height)   
{
    int y1, y2, u, v;    
    unsigned char *py1, *py2;   
    int i, j, c1, c2, c3, c4;   
    unsigned char *d1, *d2;   
    unsigned char *src_u;
    static int init_yuv420p = 0;

    src_u = yuvbuffer + width * height;   // u

    py1 = yuvbuffer;   // y
    py2 = py1 + width;   
    d1 = rgbbuffer;   
    d2 = d1 + 3 * width;   

    init_yuv420p_table();

    for (j = 0; j < height; j += 2)    
    {    
        for (i = 0; i < width; i += 2)    
        {
            if (type ==  FMT_NV12)
            {
                u = *src_u++;   
                v = *src_u++;      // v紧跟u，在u的下一个位置
            }
            if (type == FMT_NV21)
            {
                v = *src_u++;   
                u = *src_u++;      // u紧跟v，在v的下一个位置
            }

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
}

/**
yuv444
-->
u0y0v0 ->r0 g0 b0
u1y1v1 -> r1 g1 b1

...
*/
void yuv4444_to_rgb24(unsigned char *yuv, unsigned char *rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb;

    p_y = yuv;
    p_u = yuv+width*height;
    p_v = yuv+2*width*height;
    p_rgb = rgb;

    init_yuv422p_table();

    for (i = 0; i < width * height; i++)
    {
        y  = p_y[0];
        cb = p_u[0];
        cr = p_v[0];
        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value

        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;;
        p_rgb += 3;
        p_y++;
        p_u++;
        p_v++;
    }
}

/**
只支持平面、半平面的格式
交织的不支持，因为不知道如何获取Y
*/
void y_to_rgb24(unsigned char *yuv, unsigned char *rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_rgb;

    p_y = yuv;

    p_rgb = rgb;

    // 只转换Y的，U、V也需要，其值固定为128
    cb = 128;
    cr = 128;
    init_yuv422p_table();

    for (i = 0; i < width * height; i++)
    {
        y  = p_y[0];
        r = MAX (0, MIN (255, (V[cr] + Y1[y])/10000));   //R value
        b = MAX (0, MIN (255, (U[cb] + Y1[y])/10000));   //B value
        g = MAX (0, MIN (255, (Y2[y] - 5094*(r) - 1942*(b))/10000)); //G value
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        p_rgb += 3;
        p_y++;
    }
}

// 对外接口
int yuv_to_rgb24(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width, int height)
{
    int ret = 0;

    switch (type)
    {
    case FMT_YUV420:
    case FMT_YV12:
        yuv420p_to_rgb24(type, yuvbuffer, rgbbuffer, width, height);
        break;
    case FMT_YUV422:
    case FMT_YV16:
        yuv422p_to_rgb24(type, yuvbuffer, rgbbuffer, width, height);
        break;
    case FMT_YUYV:
    case FMT_YVYU:
    case FMT_UYVY:
    case FMT_VYUY:
        yuv422packed_to_rgb24(type, yuvbuffer, rgbbuffer, width, height);
        break;
    case FMT_NV12:
    case FMT_NV21:
        yuv420sp_to_rgb24(type, yuvbuffer, rgbbuffer, width, height);
        break;
    case FMT_NV16:
    case FMT_NV61:
        yuv422sp_to_rgb24(type, yuvbuffer, rgbbuffer, width, height);
        break;
    case FMT_YUV444:
        yuv4444_to_rgb24(yuvbuffer, rgbbuffer, width, height);
        break;
    case FMT_Y:
        y_to_rgb24(yuvbuffer, rgbbuffer, width, height);
        break;
    default:
        printf("unsupported yuv type!\n");
        ret = -1;
        break;
    }

    return ret;
}

// yuv之间的转换
//===================================================================

/**
yyyy yyyy
uv    uv
->
yyyy yyyy
uu
vv
*/
void yuv422sp_to_yuv422p(YUV_TYPE type, unsigned char* yuv422sp, unsigned char* yuv422p, int width, int height)
{
    int i, j;
    int y_size;
    int uv_size;
    unsigned char* p_y1;
    unsigned char* p_uv;

    unsigned char* p_y2;
    unsigned char* p_u;
    unsigned char* p_v;

    y_size = uv_size = width * height;

    p_y1 = yuv422sp;
    p_uv = yuv422sp + y_size;

    p_y2 = yuv422p;
    p_u  = yuv422p + y_size;
    p_v  = p_u + width * height / 2;

    memcpy(p_y2, p_y1, y_size);

    for (j = 0, i = 0; j < uv_size; j+=2, i++)
    {
        if (type == FMT_NV16)
        {
            p_u[i] = p_uv[j];
            p_v[i] = p_uv[j+1];
        }
        else if (type == FMT_NV61)
        {
            p_v[i] = p_uv[j];
            p_u[i] = p_uv[j+1];
        }
    }
}

/**
yyyy yyyy
uu
vv
->
yyyy yyyy
uv    uv
*/
void yuv422p_to_yuv422sp(YUV_TYPE type, unsigned char* yuv422p, unsigned char* yuv422sp, int width, int height)
{
    int i, j;
    int y_size;
    int uv_size;
    unsigned char* p_y1;
    unsigned char* p_uv;

    unsigned char* p_y2;
    unsigned char* p_u;
    unsigned char* p_v;

    y_size = uv_size = width * height;

    p_y1 = yuv422p;

    p_y2 = yuv422sp;
    p_u  = p_y1 + y_size;
    p_v  = p_u + width * height / 2;

    p_uv = p_y2 + y_size;

    memcpy(p_y2, p_y1, y_size);

    for (j = 0, i = 0; j < uv_size; j+=2, i++)
    {
	// 此处可调整U、V的位置，变成NV16或NV61
        if (type == FMT_NV16)
        {
            p_uv[j] = p_u[i];
            p_uv[j+1] = p_v[i];
        }
        else if (type == FMT_NV61)
        {
            p_uv[j] = p_v[i];
            p_uv[j+1] = p_u[i];
        }
    }
}

/**
yyyy yyyy
uv    uv
->
yyyy yyyy
uu
vv
*/
void yuv420sp_to_yuv420p(YUV_TYPE type, unsigned char* yuv420sp, unsigned char* yuv420p, int width, int height)
{
    int i, j;
    int y_size = width * height;

    unsigned char* y = yuv420sp;
    unsigned char* uv = yuv420sp + y_size;

    unsigned char* y_tmp = yuv420p;
    unsigned char* u_tmp = yuv420p + y_size;
    unsigned char* v_tmp = yuv420p + y_size * 5 / 4;

    // y
    memcpy(y_tmp, y, y_size);

    // u
    for (j = 0, i = 0; j < y_size/2; j+=2, i++)
    {
        if (type == FMT_NV12)
        {
            u_tmp[i] = uv[j];
            v_tmp[i] = uv[j+1];
        }
        else if (type == FMT_NV21)
        {
            v_tmp[i] = uv[j];
            u_tmp[i] = uv[j+1];
        }
        
    }
}

/**
yyyy yyyy
uu
vv
->
yyyy yyyy
uv    uv
*/
void yuv420p_to_yuv420sp(YUV_TYPE type, unsigned char* yuv420p, unsigned char* yuv420sp, int width, int height)
{
    int i, j;
    int y_size = width * height;

    unsigned char* y = yuv420p;
    unsigned char* u = yuv420p + y_size;
    unsigned char* v = yuv420p + y_size * 5 / 4;

    unsigned char* y_tmp = yuv420sp;
    unsigned char* uv_tmp = yuv420sp + y_size;

    // y
    memcpy(y_tmp, y, y_size);

    // u
    for (j = 0, i = 0; j < y_size/2; j+=2, i++)
    {
        if (type == FMT_NV12)
        {
            uv_tmp[j] = u[i];
            uv_tmp[j+1] = v[i];
        }
        else if (type == FMT_NV21)
        {
            uv_tmp[j] = v[i];
            uv_tmp[j+1] = u[i];
        }
    }
}

void yu_to_yv(YUV_TYPE type, unsigned char* yu, unsigned char* yv, int width, int height)
{
    int y_size;

    unsigned char* p_y1;
    unsigned char* p_u1;
    unsigned char* p_v1;

    unsigned char* p_y2;
    unsigned char* p_u2;
    unsigned char* p_v2;

    y_size = width * height;

    p_y1 = yu;
    p_y2 = yv;

    if (type == FMT_YUV422)
    {
        p_u1  = p_y1 + y_size;
        p_v1  = p_u1 + y_size / 2;

        p_u2  = p_y2 + y_size;
        p_v2  = p_u2 + y_size / 2;

        memcpy(p_y2, p_y1, y_size);
        memcpy(p_v2, p_u1, y_size/2);
        memcpy(p_u2, p_v1, y_size/2);
    }
    if (type == FMT_YUV420)
    {
        p_u1  = p_y1 + y_size;
        p_v1  = p_u1 + y_size / 4;

        p_u2  = p_y2 + y_size;
        p_v2  = p_u2 + y_size / 4;

        memcpy(p_y2, p_y1, y_size);
        memcpy(p_v2, p_u1, y_size/4);
        memcpy(p_u2, p_v1, y_size/4);
    }
}

void yv_to_yu(YUV_TYPE type, unsigned char* yv, unsigned char* yu, int width, int height)
{
    int y_size;

    unsigned char* p_y1;
    unsigned char* p_u1;
    unsigned char* p_v1;

    unsigned char* p_y2;
    unsigned char* p_u2;
    unsigned char* p_v2;

    y_size = width * height;

    p_y1 = yv;
    p_y2 = yu;

    if (type == FMT_YV16)
    {
        p_u1  = p_y1 + y_size;
        p_v1  = p_u1 + y_size / 2;

        p_u2  = p_y2 + y_size;
        p_v2  = p_u2 + y_size / 2;

        memcpy(p_y2, p_y1, y_size);
        memcpy(p_v2, p_u1, y_size/2);
        memcpy(p_u2, p_v1, y_size/2);
    }
    if (type == FMT_YV12)
    {
        p_u1  = p_y1 + y_size;
        p_v1  = p_u1 + y_size / 4;

        p_u2  = p_y2 + y_size;
        p_v2  = p_u2 + y_size / 4;

        memcpy(p_y2, p_y1, y_size);
        memcpy(p_v2, p_u1, y_size/4);
        memcpy(p_u2, p_v1, y_size/4);
    }
}

void yuv422p_to_yuv422packed(YUV_TYPE type1, YUV_TYPE type2, unsigned char* yuv422p, unsigned char* yuv, int width, int height)
{
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_yuv;

    p_y = yuv422p;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 2;

    if (type1 == FMT_YV16)
    {
        p_v = p_y + width * height;
        p_u = p_u + width * height / 2;
    }
    p_yuv = yuv;

    for (i = 0; i < width * height / 2; i++)
    {
        switch(type2)
        {
        case FMT_YUYV:
            p_yuv[0] = p_y[0];
            p_yuv[1] = p_u[0];
            p_yuv[2] = p_y[1];
            p_yuv[3] = p_v[0];
            break;
        case FMT_YVYU:
            p_yuv[0] = p_y[0];
            p_yuv[1] = p_v[0];
            p_yuv[2] = p_y[1];
            p_yuv[3] = p_u[0];
            break;
        case FMT_UYVY:
            p_yuv[0] = p_u[0];
            p_yuv[1] = p_y[0];
            p_yuv[2] = p_v[0];
            p_yuv[3] = p_y[1];
            break;
        case FMT_VYUY:
            p_yuv[0] = p_v[0];
            p_yuv[1] = p_y[0];
            p_yuv[2] = p_u[0];
            p_yuv[3] = p_y[1];
            break;
        default:
            break;
        }

        p_y += 2;
        p_u += 1;
        p_v += 1;
        p_yuv += 4;
    }
}

void yuv422packed_to_yuv422p(YUV_TYPE type1, YUV_TYPE type2, unsigned char* yuv, unsigned char* yuv422p, int width, int height)
{
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_yuv;

    p_yuv = yuv;

    p_y = yuv422p;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 2;

    if (type2 == FMT_YV16)
    {
        p_v = p_y + width * height;
        p_u = p_u + width * height / 2;
    }
    

    for (i = 0; i < width * height / 2; i++)
    {
        switch(type1)
        {
        case FMT_YUYV:
            p_y[0] = p_yuv[0];
            p_u[0] = p_yuv[1];
            p_y[1] = p_yuv[2];
            p_v[0] = p_yuv[3];
            break;
        case FMT_YVYU:
            p_y[0] = p_yuv[0];
            p_v[0] = p_yuv[1];
            p_y[1] = p_yuv[2];
            p_u[0] = p_yuv[3];
            break;
        case FMT_UYVY:
            p_u[0] = p_yuv[0];
            p_y[0] = p_yuv[1];
            p_v[0] = p_yuv[2];
            p_y[1] = p_yuv[3];
            break;
        case FMT_VYUY:
            p_v[0] = p_yuv[0];
            p_y[0] = p_yuv[1];
            p_u[0] = p_yuv[2];
            p_y[1] = p_yuv[3];
            break;
        default:
            break;
        }

        p_y += 2;
        p_u += 1;
        p_v += 1;
        p_yuv += 4;
    }
}

//// 原内存修改
// 暂时失败
void yuv420sp_to_yuv420p_1(unsigned char* yuv420sp, int width, int height)
{
    int i, j;
    int y_size = width * height;
    int v_size = width * height / 2;

    unsigned char* uv = yuv420sp + y_size;

    unsigned char* v_tmp = uv+v_size;
    unsigned char* v_buff = (unsigned char*)malloc(v_size);
    
    // uv
    for (j = 0, i = 0; j < y_size/2; j+=4, i++)
    {
        v_buff[i] = uv[j+1];
        uv[i+1] = uv[j+2];
    }

    memcpy(v_tmp, v_buff, v_size);

    free(v_buff);
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// 自己研究的

int rgb2yuv(int r, int g, int b, int* Y, int* Cb, int* Cr)
{
    int y, cb, cr;

    y = (int)( 0.299    * r + 0.587   * g + 0.114   * b);
    cb = (int)(-0.16874 * r - 0.33126 * g + 0.50000 * b + 128);
    if (cb < 0)
        cb = 0;
    cr = (int)( 0.50000 * r - 0.41869 * g - 0.08131 * b + 128);
    if (cr < 0)
        cr = 0;

    *Y = y;
    *Cb = cb;
    *Cr = cr;

    return 0;
}

// TODO：改为整数运算
int yuv2rgb(int Y, int Cb, int Cr, int* r, int* g, int* b)
{
    unsigned int r_tmp, g_tmp, b_tmp;
    unsigned char r_t, g_t, b_t;

    // OK
    // 图片表面有“白纱”
#if 0
    // YUV(0~255) JFIF
    //r_t = (unsigned char)(Y + 1.402 * (Cr - 128));
    //g_t = (unsigned char)(Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128));
    //b_t = (unsigned char)(Y + 1.772 * (Cb - 128));

    r_tmp = (Y + 1.402 * (Cr - 128));
    g_tmp = (Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128));
    b_tmp = (Y + 1.772 * (Cb - 128));

    r_t =  MAX (0, MIN (255, r_tmp));
    g_t =  MAX (0, MIN (255, g_tmp));
    b_t =  MAX (0, MIN (255, b_tmp));

#endif

    // YCbCr(16~235)
    // 有个别颜色出错。但无“白纱”
#if 01
    //r_t = (unsigned char)(1.164 * (Y - 16) + 1.596 * (Cr - 128));
    //g_t = (unsigned char)(1.164 * (Y - 16) - 0.813 * (Cr - 128) - 0.391 * (Cb - 128));
    //b_t = (unsigned char)(1.164 * (Y - 16) + 2.018 * (Cb - 128));

    r_tmp = (unsigned char)(1.164 * (Y - 16) + 1.596 * (Cr - 128));
    g_tmp = (unsigned char)(1.164 * (Y - 16) - 0.813 * (Cr - 128) - 0.391 * (Cb - 128));
    b_tmp = (unsigned char)(1.164 * (Y - 16) + 2.018 * (Cb - 128));

    r_t =  MAX (0, MIN (255, r_tmp));
    g_t =  MAX (0, MIN (255, g_tmp));
    b_t =  MAX (0, MIN (255, b_tmp));
#endif
    // not OK
#if 0
    r_t = (unsigned char)(Y + 1.403*Cr);
    g_t = (unsigned char)(Y - 0.344*Cb - 0.714*Cr);
    b_t = (unsigned char)(Y + 1.770*Cb);
#endif

    *r = r_t;
    *g = g_t;
    *b = b_t;

    return 0;
}

#define RANGE(val, min, max)    (((val) > (min)) ? (((val) <= (max)) ? (val) : (max)) : (min))
#define SHR_P(d, c)             (((d ) & (1<< ((c) - 1 ))) ? ((d) >> (c)) + 1 : ((d) >> (c)))
#define SHR_N(d, c)             (-(((-(d)) & (1 << ((c) - 1))) ? ((-(d)) >> (c)) + 1 : ((-(d)) >> (c))))
#define SHR(d, c)               (((d)>=0) ? SHR_P(d, c) : SHR_N(d, c))

void yuv2rgb_1(int y, int cb, int cr, int* r, int* g, int* b)  
{  
    int r_tmp = 0;  
    int g_tmp = 0;  
    int b_tmp = 0;  

    r_tmp = (((int)y) << 14) + 22970*(((int)cr) - 128);  
    g_tmp = (((int)y) << 14) -  5638*(((int)cb) - 128) - 11700*(((int)cr) - 128);  
    b_tmp = (((int)y) << 14) + 29032*(((int)cb) - 128);  

    r_tmp = SHR(r_tmp, 14);  
    g_tmp = SHR(g_tmp, 14);  
    b_tmp = SHR(b_tmp, 14);  

    *r = (int)RANGE(r_tmp, 0, 255);  
    *g = (int)RANGE(g_tmp, 0, 255);  
    *b = (int)RANGE(b_tmp, 0, 255);  
}

/*
y - y
u - cb
v - cr
可在此函数中调整RGB的排序
TODO：这种方法转换得到的图片比查表法得到的效果差一些
*/
void yuv422_to_rgb24_1(unsigned char* yuv422, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb;
    
    p_y = yuv422;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 2;
    p_rgb = rgb;

    for (i = 0; i < width * height / 2; i++)
    {
        y  = p_y[0];
        cb = p_u[0];
        cr = p_v[0];
        //yuv2rgb(y, cb, cr, &r, &g, &b);
        yuv2rgb_1(y, cb, cr, &r, &g, &b);
        // 此处可调整RGB排序，BMP图片排序为BGR
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        y  = p_y[1];
        cb = p_u[0];
        cr = p_v[0];
        //yuv2rgb(y, cb, cr, &r, &g, &b);
        yuv2rgb_1(y, cb, cr, &r, &g, &b);
        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        p_y += 2;
        p_u += 1;
        p_v += 1;
        p_rgb += 6;
    }
}

/**
uyvy
-->
u0y0v0 ->r0 g0 b0
u0y1v0 -> r1 g1 b1

...
*/
void yuv422packed_to_rgb24_1(YUV_TYPE type, unsigned char *yuv, unsigned char *rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p;
    unsigned char* p_rgb;

    p = yuv;
    p_rgb = rgb;
    for (i = 0; i < width * height / 2; i++)
    {
        switch(type)
        {
        case FMT_YUYV:
            y  = p[0];
            cb = p[1];
            cr = p[3];
            break;
        case FMT_YVYU:
            y  = p[0];
            cr = p[1];
            cb = p[3];
            break;
        case FMT_UYVY:
            cb = p[0];
            y  = p[1];
            cr = p[2];
            break;
        case FMT_VYUY:
            cr = p[0];
            y  = p[1];
            cb = p[2];
            break;
        default:
            break;
        }

        //yuv2rgb(y, cb, cr, &r, &g, &b);
        yuv2rgb_1(y, cb, cr, &r, &g, &b);
        // 此处可调整RGB排序，BMP图片排序为BGR
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        switch(type)
        {
        case FMT_YUYV:
        case FMT_YVYU:
            y  = p[2];
            break;
        case FMT_UYVY:
        case FMT_VYUY:
            y  = p[3];
            break;
        default:
            break;
        }

        //yuv2rgb(y, cb, cr, &r, &g, &b);
        yuv2rgb_1(y, cb, cr, &r, &g, &b);
        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;
        p_rgb += 6;
        p += 4;
    }
}

/*
y - y
u - cb
v - cr
Q:为何不是4个Y共同一个U、一个V？
not ok，图片下半部分是黑色的
*/
void yuv420_to_rgb24_1(unsigned char* yuv420, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb;

    p_y = yuv420;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 4;
    p_rgb = rgb;

    for (i = 0; i < width * height / 4; i++)
    {
        y  = p_y[0];
        cb = p_u[0];
        cr = p_v[0];
        yuv2rgb(y, cb, cr, &r, &g, &b);
        //yuv2rgb_1(y, cb, cr, &r, &g, &b);
        // 此处可调整RGB排序，BMP图片排序为BGR
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        y  = p_y[1];
        cb = p_u[0];
        cr = p_v[0];
        yuv2rgb(y, cb, cr, &r, &g, &b);
        //yuv2rgb_1(y, cb, cr, &r, &g, &b);
        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        p_y += 2;
        p_u += 1;
        p_v += 1;
        p_rgb += 6;
    }
}

// OK
// 转换的图片有一层“白纱”(与查表法对比)，应该是Y部分范围不正确导致。
void yuv420_to_rgb24_2(unsigned char *yuv420, unsigned char *rgb24, int width, int height) 
{
    //  int begin = GetTickCount();
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    int R,G,B,Y,U,V;
    int x,y;
    int nWidth = width>>1; //色度信号宽度

    p_y = yuv420;
    p_u = p_y + width * height;
    p_v = p_u + width * height / 4;

    for (y=0;y<height;y++)
    {
        for (x=0;x<width;x++)
        {
            Y = *(p_y + y*width + x);
            U = *(p_u + ((y>>1)*nWidth) + (x>>1));
            V = *(p_v + ((y>>1)*nWidth) + (x>>1));
            /*
            R = Y + 1.402*(V-128);
            G = Y - 0.34414*(U-128) - 0.71414*(V-128);
            B = Y + 1.772*(U-128);
            */

            yuv2rgb(Y, U, V, &R, &G, &B);
            //yuv2rgb_1(Y, U, V, &R, &G, &B);
            //防止越界
            if (R>255)R=255;
            if (R<0)R=0;
            if (G>255)G=255;
            if (G<0)G=0;
            if (B>255)B=255;
            if (B<0)B=0;

            // 图片倒立
            //*(rgb24 + ((height-y-1)*width + x)*3) = R;  //B;
            //*(rgb24 + ((height-y-1)*width + x)*3 + 1) = G;
            //*(rgb24 + ((height-y-1)*width + x)*3 + 2) = B;  //R;
            // 图像正常
            *(rgb24 + (y*width + x)*3) = R; //B;
            *(rgb24 + (y*width + x)*3 + 1) = G;
            *(rgb24 + (y*width + x)*3 + 2) = B; //R;   
        }
    }
}

// not ok
void yuv420_to_rgb24_3(unsigned char* yuv, unsigned char* rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    int j = 0;
    unsigned char* p_y1;
    unsigned char* p_y2;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb1;
    unsigned char* p_rgb2;

    p_u = yuv + width * height;
    p_v = p_u + width * height / 4;

    p_y1 = yuv;
    p_y2 = yuv + width;
    p_rgb1 = rgb;
    p_rgb2 = rgb + 3 * width;

    for (i = 0; i < height; i ++)
    {
        for (j = 0; j < width; j ++)
        {
            cb = *p_u++;
            cr = *p_v++;

            y  = *p_y1++;
            yuv2rgb(y, cb, cr, &r, &g, &b);
            *p_rgb1++ = r;
            *p_rgb1++ = g;
            *p_rgb1++ = b;
        }
    }
#if 0
    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            cb = *p_u++;
            cr = *p_v++;

            y  = *p_y1++;
            yuv2rgb(y, cb, cr, &r, &g, &b);
            *p_rgb1++ = r;
            *p_rgb1++ = g;
            *p_rgb1++ = b;

            y  = *p_y2++;
            yuv2rgb(y, cb, cr, &r, &g, &b);
            *p_rgb2++ = r;
            *p_rgb2++ = g;
            *p_rgb2++ = b;

            y  = *p_y1++;
            yuv2rgb(y, cb, cr, &r, &g, &b);
            *p_rgb1++ = r;
            *p_rgb1++ = g;
            *p_rgb1++ = b;

            y  = *p_y2++;

            yuv2rgb(y, cb, cr, &r, &g, &b);
            *p_rgb2++ = r;
            *p_rgb2++ = g;
            *p_rgb2++ = b;

            //p_y1 += 2;
            //p_y2 += 2;

            //p_rgb1 += 6;
            //p_rgb2 += 6;
        }

        p_y1 += width;
        p_y2 += width;
        p_rgb1 += 3*width;
        p_rgb2 += 3*width;
    }
#endif
}

void yuv_to_rgb24_1(unsigned char* yuv, unsigned char* rgb, int width, int height, YUV_TYPE type)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char* p_y;
    unsigned char* p_u;
    unsigned char* p_v;
    unsigned char* p_rgb;
    int chroma_v;       // 垂直亮度采样
    int chroma_h;       // 水平亮度采样
    int cbcr_width;     // 色度宽
    int cbcr_height;    // 色度高
    int cbcr_size;      // 色度大小

    switch(type)
    {
    case FMT_YUV420:
        chroma_v = 2;
        chroma_h = 2;
        break;
    case FMT_YUV422:
        chroma_v = 4;
        chroma_h = 2;
        break;
    default:
        chroma_v = chroma_h = 2;
        break;
    }
    cbcr_width  = width * (chroma_h / 4.0);
    cbcr_height = height * (chroma_v / 4.0);
    cbcr_size   = cbcr_width * cbcr_height;

    p_y = yuv;
    p_u = p_y + width * height;
    p_v = p_u + cbcr_size;
    p_rgb = rgb;

    for (i = 0; i < cbcr_size; i++)
    {
        y  = p_y[0];
        cb = p_u[0];
        cr = p_v[0];
        yuv2rgb(y, cb, cr, &r, &g, &b);
        p_rgb[0] = r;
        p_rgb[1] = g;
        p_rgb[2] = b;

        y  = p_y[1];
        cb = p_u[0];
        cr = p_v[0];
        yuv2rgb(y, cb, cr, &r, &g, &b);
        p_rgb[3] = r;
        p_rgb[4] = g;
        p_rgb[5] = b;

        p_y += 2;
        p_u += 1;
        p_v += 1;
        p_rgb += 6;
    }
}

////////////////////////////////

void swaprgb(unsigned char* rgb, int len)
{
    BYTE tmp = 0;
    for (int i = 0; i < len; i += 6)
    {
        tmp = rgb[i];
        rgb[i] = rgb[i + 2];
        rgb[i + 2] = tmp;

        tmp = rgb[i+3];
        rgb[i+3] = rgb[i + 5];
        rgb[i + 5] = tmp;
    }
}

/////////////////////////////
// 方便从rgb转为yuv
int rgb2YCbCr(unsigned int rgbColor, int* Y, int* Cb, int* Cr)
{
    unsigned char r, g, b;
    int y, cb, cr;
    r = (rgbColor&0x00ff0000) >> 16;
    g = (rgbColor&0x0000ff00) >> 8;
    b = rgbColor & 0xff;

    y = (int)( 0.299 * r + 0.587 * g + 0.114 * b);
    cb = (int)(-0.16874 * r - 0.33126 * g + 0.50000 * b + 128);
    if (cb < 0)
        cb = 0;
    cr = (int)( 0.50000 * r - 0.41869 * g - 0.08131 * b + 128);
    if (cr < 0)
        cr = 0;

    *Y = y;
    *Cb = cb;
    *Cr = cr;

    return 0;
}
/// 填充YUV数据，支持格式：YUV444 YUYV UYVY VYUY
void init_yuv_buf(YUV_TYPE type, unsigned char* buf, int width, int height)
{
    unsigned char *src = buf;
    int i, j;

    /*
    unsigned int rainbow_rgb[] = {
        0xFF0000, 0xFF6100, 0xFFFF00, 0x00FF00, 0x00FFFF,
        0x0000FF, 0xA020F0, 0x000000, 0xFFFFFF, 0xF4A460};
    */
    // 由上数组转换而成
    unsigned int rainbow_yuv[] = {
        0x4c54ff, 0x8534d6, 0xe10094, 0x952b15, 0xb2ab00,
        0x1dff6b, 0x5dd2af, 0xbb1654, 0x9c4bc5, 0xb450ad};

    unsigned char *p_y = src;
    unsigned char *p_u = src+width*height;
    unsigned char *p_v = src+2*width*height;

    int slice = height / 10;
    for (i = 0; i < height; i++) // h
    {
        int index = i / slice;
        unsigned char y = (rainbow_yuv[index] & 0xff0000 ) >> 16;
        unsigned char u = (rainbow_yuv[index] & 0x00ff00) >> 8;
        unsigned char v = (rainbow_yuv[index] & 0x0000ff);
        if (type == FMT_YUV444)
        {
            for (j=0;j<width;j++) // w
            {
                *p_y++ = y;
                *p_u++ = u;
                *p_v++ = v;
            }
        }
        else
        {
            for (j=0; j<width*2; j+=4) // w
            {
                if (type == FMT_YUYV)
                {
                    src[i*width*2+j+0] = y; // Y0
                    src[i*width*2+j+1] = u; // U
                    src[i*width*2+j+2] = y; // Y1
                    src[i*width*2+j+3] = v; // V
                }
                if (type == FMT_YVYU)
                {
                    src[i*width*2+j+0] = y; // Y0
                    src[i*width*2+j+1] = v; // V
                    src[i*width*2+j+2] = y; // Y1
                    src[i*width*2+j+3] = u; // U
                }
                else if (type == FMT_UYVY)
                {
                    src[i*width*2+j+0] = u; // U
                    src[i*width*2+j+1] = y; // Y0
                    src[i*width*2+j+2] = v; // V
                    src[i*width*2+j+3] = y; // Y1
                }
                else if (type == FMT_VYUY)
                {
                    src[i*width*2+j+0] = v; // V
                    src[i*width*2+j+1] = y; // Y0
                    src[i*width*2+j+2] = u; // U
                    src[i*width*2+j+3] = y; // Y1
                }
            }
        }
    }
}

void save_yuv_file(const char* filename, int width, int height, int type)
{
    FILE* fp;
    int len = width*height*2;
    unsigned char* yuv_buf = NULL;

    if (type == FMT_YUV444)
        len = width*height*3;

    yuv_buf = (unsigned char*)malloc(len);
    memset(yuv_buf, '\0', len);

    init_yuv_buf((YUV_TYPE)type, yuv_buf, width, height);

    fp = fopen(filename, "w");

    fwrite(yuv_buf, 1, len, fp);

    fclose(fp);
    free(yuv_buf);
}

void change_yuv_file(const char* filename, const char* file_out, int width, int height, int type)
{
    FILE* fp;
    int len = width*height*2;
    unsigned char* yuv_buf = NULL;
    unsigned char* yuv_buf_out = NULL;

    if (type == 0 || type == 1)
        len = width*height*3/2;

    yuv_buf = (unsigned char*)malloc(len);
    memset(yuv_buf, '\0', len);

    yuv_buf_out = (unsigned char*)malloc(len);
    memset(yuv_buf_out, '\0', len);

    fp = fopen(filename, "r");
    fread(yuv_buf, 1, len, fp);
    fclose(fp);

    if (type == 0)
        yuv420p_to_yuv420sp(FMT_NV12, yuv_buf, yuv_buf_out, width, height);
        //yuv420sp_to_yuv420p_1(yuv_buf, width, height);
    if (type == 1)
        yuv420sp_to_yuv420p(FMT_NV12, yuv_buf, yuv_buf_out, width, height);
    if (type == 2)
        yuv422p_to_yuv422sp(FMT_NV16, yuv_buf, yuv_buf_out, width, height);
    if (type == 3)
        yuv422sp_to_yuv422p(FMT_NV16, yuv_buf, yuv_buf_out, width, height);

    fp = fopen(file_out, "w");
    fwrite(yuv_buf_out, 1, len, fp);
    //fwrite(yuv_buf, 1, len, fp);
    fclose(fp);

    free(yuv_buf);
    free(yuv_buf_out);
}