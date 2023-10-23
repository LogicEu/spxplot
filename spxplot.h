/*

Copyright (c) 2023 Eugenio Arteaga A.

Permission is hereby granted, free of charge, to any 
person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice 
shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef SIMPLE_PIXEL_PLOTTER_H
#define SIMPLE_PIXEL_PLOTTER_H

/*********************
****** spxplot *******
 Simple Pixel Plotter
**********************
 @Eugenio Arteaga A.
**********************

Simple pixel plotter header only library in C89. 
Provides right out of the box plotting functions
and algorithms to easily create your own images.

****************************************************/

/* Pixel Plotter */

#include <stdint.h>

#ifndef PX_TYPE_DEFINED
#define PX_TYPE_DEFINED

typedef struct Px {
    uint8_t r, g, b, a;
} Px;

#endif /* PX_TYPE_DEFINED */

#ifndef TEX2D_TYPE_DEFINED
#define TEX2D_TYPE_DEFINED

typedef struct Tex2D {
    Px* pixbuf;
    int width;
    int height;
} Tex2D;

#endif /* TEX2D_TYPE_DEFINED */

#ifndef IVEC2_TYPE_DEFINED
#define IVEC2_TYPE_DEFINED

typedef struct ivec2 {
    int x, y;
} ivec2;

#endif /* IVEC2_TYPE_DEFINED */

#ifndef VEC2_TYPE_DEFINED
#define VEC2_TYPE_DEFINED

typedef struct vec2 {
    float x, y;
} vec2;

#endif /* VEC2_TYPE_DEFINED */

#ifndef VERT2D_TYPE_DEFINED
#define VERT2D_TYPE_DEFINED

typedef struct Vert2D {
    vec2 pos;
    vec2 uv;
} Vert2D;

#endif /* VERT2D_TYPE_DEFINED */

#define pxAbs(n) (((n) >= 0) ? (n) : -(n))
#define pxSign(n) (((n) >= 0) ? 1 : -1)
#define pxMax(n, m) (((n) > (m)) ? (n) : (m))
#define pxMin(n, m) (((n) < (m)) ? (n) : (m))
#define pxClamp(n, min, max) (((n) > (max)) ? (max) : ((n) < (min)) ? (min) : (n))

#define pxSwap(a, b, type) do { type _tmp = a; a = b; b = _tmp; } while (0)
#define pxAt(tex, px, py) tex.pixbuf[(tex.width * (py)) + (px)]
#define pxInside(tex, px, py) ((px) >= 0 && (px) < tex.width &&\
                                (py) >= 0 && (py) < tex.height)

Px      pxLerp(Px a, Px b, float t);
void    pxPlot(const Tex2D texture, int x, int y, Px color);
void    pxMix(const Tex2D texture, int x, int y, Px color);
void    pxBlend(const Tex2D texture, int x, int y, float t, Px color);
void    pxPlotLine(const Tex2D texture, ivec2 p, ivec2 q, const Px color);
void    pxPlotLineSmooth(const Tex2D texture, vec2 p, vec2 q, const Px color);
void    pxPlotBezier2(const Tex2D texture, vec2 a, vec2 b, vec2 c, const Px col);
void    pxPlotBezier2Smooth(const Tex2D texture, vec2 a, vec2 b, vec2 c, const Px col);
void    pxPlotBezier3(const Tex2D texture, vec2 a, vec2 b, vec2 c, vec2 d, const Px col);
void    pxPlotRect(const Tex2D texture, ivec2 p, ivec2 q, const Px color);
void    pxPlotRectRound(const Tex2D texture, ivec2 p, ivec2 q, float t, const Px color);
void    pxPlotTri(const Tex2D texture, ivec2 p0, ivec2 p1, ivec2 p2, const Px color);
void    pxPlotTriSmooth(const Tex2D texture, vec2 p0, vec2 p1, vec2 p2, const Px c);
void    pxPlotCircle(const Tex2D texture, ivec2 p, float r, const Px color);
void    pxPlotCircleSmooth(const Tex2D texture, ivec2 p, float r, const Px color);

#ifdef SPXP_APPLICATION

/*********************
******* pxplot *******
    Pixel Plotter
**********************
*   IMPLEMENTATION   *
*********************/

#ifndef SPXP_SUBSAMPLES 
#define SPXP_SUBSAMPLES 2
#endif /* SPXP_SUBSAMPLES */

#include <math.h>

/* plotting functions */

static uint8_t mix8(uint8_t a, uint8_t b, float t)
{
    return (uint8_t)((float)a + t * (float)(b - a));
}

static float normalize(float min, float max, float n)
{
    n -= min;
    return n == 0.0F ? 0.0F : (max - min) / n;
}

static vec2 vec2_mix(vec2 a, vec2 b, float t)
{
    vec2 p;
    p.x = a.x + t * (b.x - a.x);
    p.y = a.y + t * (b.y - a.y);
    return p;
}

static vec2 vec2_floor(vec2 a)
{
    vec2 p;
    p.x = floor(a.x);
    p.y = floor(a.y);
    return p;
}

static float vec2_determinant(vec2 a, vec2 b, vec2 c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

Px pxLerp(Px a, Px b, float t)
{
    Px px;
    px.r = mix8(a.r, b.r, t);
    px.g = mix8(a.g, b.g, t);
    px.b = mix8(a.b, b.b, t);
    px.a = mix8(a.a, b.a, t);
    return px;
}

void pxPlot(const Tex2D texture, int x, int y, Px color)
{ 
    if (x >= 0 && x < texture.width && y >= 0 && y < texture.height) {
        pxAt(texture, x, y) = color;
    }
}

void pxBlend(const Tex2D texture, int x, int y, float t, Px color)
{
    if (x >= 0 && x < texture.width && y >= 0 && y < texture.height) {
        pxAt(texture, x, y) = pxLerp(pxAt(texture, x, y), color, t);
    }
}

void pxMix(const Tex2D texture, int x, int y, Px color)
{
    float t = (float)color.a / 255.0F;
    color.a = 255;
    pxBlend(texture, x, y, t, color);
}

void pxPlotLine(const Tex2D texture, ivec2 p, ivec2 q, const Px color)
{
    int error;
    ivec2 d, s;

    d.x = q.x - p.x;
    d.y = q.y - p.y;
    s.x = pxSign(d.x);
    s.y = pxSign(d.y);
    d.x = pxAbs(d.x);
    d.y = -pxAbs(d.y);
    error = d.x + d.y;
    
    while (p.x != q.x || p.y != q.y) {
        int e2 = error * 2;
        pxPlot(texture, p.x, p.y, color);
        if (e2 >= d.y) {
            error = error + d.y;
            p.x += s.x;
        }
        if (e2 <= d.x) {
            error = error + d.x;
            p.y +=s.y;
        }
    }
    
    pxPlot(texture, p.x, p.y, color);
}

void pxPlotLineSmooth(const Tex2D texture, vec2 p, vec2 q, const Px color)
{
    const int steep = pxAbs(q.y - p.y) > pxAbs(q.x - p.x);
    int x, xpos1, ypos1, xpos2, ypos2;
    float g, dx, dy, xend, yend, xgap, fpart, rfpart, intery;
    
    if (steep) {
        pxSwap(p.x, p.y, float);
        pxSwap(q.x, q.y, float);
    }
    if (p.x > q.x) {
        pxSwap(p.x, q.x, float);
        pxSwap(p.y, q.y, float);
    }
    
    dx = q.x - p.x;
    dy = q.y - p.y;
    g = (dx != 0.0F) ? dy / dx : 1.0F;

    xend = floor(p.x);
    yend = p.y + g * (xend - p.x);
    xgap = 1.0F - ((p.x + 0.5F) - floor(p.x + 0.5F));
    xpos1 = xend;
    ypos1 = floor(yend);
    fpart = yend - (float)(ypos1);
    rfpart = 1.0F - fpart;

    if (steep) {
        pxBlend(texture, ypos1, xpos1, rfpart * xgap, color);
        pxBlend(texture, ypos1 + 1, xpos1, fpart * xgap, color);
    } else {
        pxBlend(texture, xpos1, ypos1, rfpart * xgap, color);
        pxBlend(texture, xpos1, ypos1 + 1, fpart * xgap, color);
    }

    intery = yend + g;
    xend = floor(q.x);
    yend = q.y + g * (xend - q.x);
    xgap = 1.0F - ((q.x + 0.5F) - floor(q.x + 0.5F));
    xpos2 = xend;
    ypos2 = floor(yend);
    fpart = yend - (float)ypos2;
    rfpart = 1.0F - fpart;

    if (steep) {
        pxBlend(texture, ypos2, xpos2, rfpart * xgap, color);
        pxBlend(texture, ypos2 + 1, xpos2, fpart * xgap, color);
        for (x = xpos1 + 1; x < xpos2; ++x) {
            const int y = floor(intery);
            fpart = intery - (float)y;
            rfpart = 1.0F - fpart;
            pxBlend(texture, y, x, rfpart, color);
            pxBlend(texture, y + 1, x, fpart, color);
            intery += g;
        }
    } else {
        pxBlend(texture, xpos2, ypos2, rfpart * xgap, color);
        pxBlend(texture, xpos2, ypos2 + 1, fpart * xgap, color);
        for (x = xpos1 + 1; x < xpos2; ++x) {
            const int y = floor(intery);
            fpart = intery - (float)y;
            rfpart = 1.0F - fpart;
            pxBlend(texture, x, y, rfpart, color);
            pxBlend(texture, x, y + 1, fpart, color);
            intery += g;
        }
    }
}

void pxPlotBezier2(const Tex2D texture, vec2 a, vec2 b, vec2 c, const Px col)
{
    float t;
    const float dx = pxAbs(b.x - a.x), dy = pxAbs(b.y - a.y);
    const float dxy = dx + dy;
    const float delta = dxy != 0.0F ? 1.0F / dxy : 1.0F;
    
    for (t = 0.0F; t < 1.0F; t += delta) {
        vec2 p, q;
        p = vec2_mix(a, c, t);
        q = vec2_mix(c, b, t);
        p = vec2_mix(p, q, t);
        pxPlot(texture, (int)p.x, (int)p.y, col);
    }
}

void pxPlotBezier2Smooth(const Tex2D texture, vec2 a, vec2 b, vec2 c, const Px col)
{
    float t;
    vec2 q, p = a;
    const float dx = pxAbs(b.x - a.x), dy = pxAbs(b.y - a.y);
    const float dxy = pxMax(dx, dy);
    const float delta = dxy != 0.0F ? 1.0F / dxy : 1.0F;

    for (t = delta; t < 1.0; t += delta) {
        vec2 p1 = vec2_mix(a, c, t);
        vec2 p2 = vec2_mix(c, b, t);
        q = vec2_mix(p1, p2, t);
        pxPlotLineSmooth(texture, p, q, col);
        p = q;
    }
    
    q = b;
    pxPlotLineSmooth(texture, p, q, col);
}

void pxPlotBezier3(
    const Tex2D texture, vec2 a, vec2 b, vec2 c, vec2 d, const Px col)
{
    float t;
    vec2 q, p = a;
    const float dx = pxAbs(d.x - a.x), dy = pxAbs(d.y - a.y);
    const float dxy = dx + dy;
    const float delta = dxy != 0.0F ? 4.0F / dxy : 1.0F;
    
    for (t = delta; t < 1.0F; t += delta) { 
        float u = 1.0F - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;
        float uut = 3.0F * uu * t;
        float utt = 3.0F * u * tt;
        
        q.x = uuu * a.x + uut * b.x + utt * c.x + ttt * d.x;
        q.y = uuu * a.y + uut * b.y + utt * c.y + ttt * d.y;
        pxPlotLineSmooth(texture, vec2_floor(p), vec2_floor(q), col);
        p = q;
    }
    
    q = d;
    pxPlotLineSmooth(texture, p, q, col);
}

void pxPlotRect(const Tex2D texture, ivec2 p, ivec2 q, const Px color)
{
    int x, y;
    const int resx = texture.width - 1, resy = texture.height - 1;
    const int starty = pxClamp(p.y - q.y, 0, resy);
    const int endy = pxClamp(p.y + q.y, 0, resy);
    const int startx = pxClamp(p.x - q.x, 0, resx);
    const int endx = pxClamp(p.x + q.x, 0, resx);
    for (y = starty; y <= endy; ++y) {
        for (x = startx; x <= endx; ++x) {
            pxAt(texture, x, y) = color;
        }
    }
}

void pxPlotRectRound(const Tex2D texture, ivec2 p, ivec2 q, float t, const Px color)
{
    const float min = (float)pxMin(q.x, q.y);
    const float r = floor(min * pxClamp(t, 0.0, 1.0));
    const float d = ceil((q.y - r) + r * 0.5);
    int R = (int)r;
    ivec2 u = q, v = p, w = p;
    
    v.y += d;
    w.y -= d;
    q.y -= R;
    u.x -= R;
    u.y = R / 2;
    pxPlotRect(texture, p, q, color);
    pxPlotRect(texture, v, u, color);
    pxPlotRect(texture, w, u, color);
    
    q.x -= R;
    v.x = p.x - q.x - 1;
    v.y = p.y - q.y - 1;
    w.x = p.x + q.x;
    w.y = p.y - q.y - 1;
    u.x = p.x - q.x - 1;
    u.y = p.y + q.y;
    q.x = p.x + q.x;
    q.y = p.y + q.y;
    
    pxPlotCircleSmooth(texture, u, R, color);
    pxPlotCircleSmooth(texture, v, R, color);
    pxPlotCircleSmooth(texture, w, R, color);
    pxPlotCircleSmooth(texture, q, R, color);
}

static void pxSortTri(ivec2 t[3])
{
    if (t[2].y < t[0].y || (t[2].y == t[0].y && t[2].x < t[0].x))
        pxSwap(t[2], t[0], ivec2);
    if (t[1].y < t[0].y || (t[1].y == t[0].y && t[1].x < t[0].x))
        pxSwap(t[1], t[0], ivec2);
    if (t[2].y < t[1].y || (t[2].y == t[1].y && t[2].x < t[1].x))
        pxSwap(t[2], t[1], ivec2);
}

static void pxSortTrif(vec2 t[3])
{
    if (t[2].y < t[0].y || (t[2].y == t[0].y && t[2].x < t[0].x))
        pxSwap(t[2], t[0], vec2);
    if (t[1].y < t[0].y || (t[1].y == t[0].y && t[1].x < t[0].x))
        pxSwap(t[1], t[0], vec2);
    if (t[2].y < t[1].y || (t[2].y == t[1].y && t[2].x < t[1].x))
        pxSwap(t[2], t[1], vec2);
}

static void pxSortTriVert2D(Vert2D t[3])
{
    if (t[2].pos.y < t[0].pos.y || (t[2].pos.y == t[0].pos.y && t[2].pos.x < t[0].pos.x))
        pxSwap(t[2], t[0], Vert2D);
    if (t[1].pos.y < t[0].pos.y || (t[1].pos.y == t[0].pos.y && t[1].pos.x < t[0].pos.x))
        pxSwap(t[1], t[0], Vert2D);
    if (t[2].pos.y < t[1].pos.y || (t[2].pos.y == t[1].pos.y && t[2].pos.x < t[1].pos.x))
        pxSwap(t[2], t[1], Vert2D);
}

void pxPlotTri(const Tex2D texture, ivec2 p0, ivec2 p1, ivec2 p2, const Px color)
{
    const int resx = texture.width - 1, resy = texture.height - 1;
    int y, n, starty, endy, difx[3];
    float steps[3];
    ivec2 t[3];

    t[0] = p0;
    t[1] = p1;
    t[2] = p2;
    pxSortTri(t);

    starty = pxMax(t[0].y, 0);
    endy = pxMin(t[2].y, resy);
    difx[0] = t[2].x - t[0].x;
    difx[1] = t[1].x - t[0].x;
    difx[2] = t[2].x - t[1].x;
    
    steps[0] = t[2].y - t[0].y ? 1.0F / (float)(t[2].y - t[0].y) : 0.0F;
    steps[1] = t[1].y - t[0].y ? 1.0F / (float)(t[1].y - t[0].y) : 0.0F;
    steps[2] = t[2].y - t[1].y ? 1.0F / (float)(t[2].y - t[1].y) : 0.0F;

    for (y = starty, n = 1; y <= endy; ++y) {
        
        int x, startx, endx;
        float dx, dy, x0, x1;
        
        n += n == 1 && y >= t[1].y;
        dx = steps[0] != 0.0F ? steps[0] * (y - t[0].y) : 1.0F;
        dy = steps[n] != 0.0F ? steps[n] * (y - t[n - 1].y) : 1.0F;
        x0 = t[0].x + (float)difx[0] * dx; 
        x1 = t[n - 1].x + (float)difx[n] * dy;
        
        if (x1 < x0) {
            pxSwap(x0, x1, int);
        }
        
        startx = pxMax(x0, 0);
        endx = pxMin(x1, resx);
        
        for (x = startx; x <= endx; ++x) {
            pxAt(texture, x, y) = color;
        }
    }
}

void pxPlotTriSmooth(const Tex2D texture, vec2 p0, vec2 p1, vec2 p2, const Px color)
{
    static const vec2 P[] = {
        {0.0F, 0.0F}, {1.0F, 0.0F}, {0.0F, -1.0F}, {-1.0F, 0.0F}, {0.0F, 1.0F}
        ,{1.0F, 1.0F}, {1.0F, -1.0F}, {-1.0F, -1.0F}, {-1.0F, 1.0F}
    };
    
    const int resx = texture.width - 1, resy = texture.height - 1;
    const float ni = 1.0F / (sizeof(P) / sizeof(P[0]));
    float difx[3], steps[3];
    int y, n, starty, endy;
    vec2 t[3];
    
    t[0] = p0;
    t[1] = p1;
    t[2] = p2;
    pxSortTrif(t);

    starty = pxMax(t[0].y, 0);
    endy = pxMin(t[2].y, resy);
    
    difx[0] = t[2].x - t[0].x;
    difx[1] = t[1].x - t[0].x;
    difx[2] = t[2].x - t[1].x;
    
    steps[0] = t[2].y - t[0].y ? 1.0F / (t[2].y - t[0].y) : 0.0F;
    steps[1] = t[1].y - t[0].y ? 1.0F / (t[1].y - t[0].y) : 0.0F;
    steps[2] = t[2].y - t[1].y ? 1.0F / (t[2].y - t[1].y) : 0.0F;

    for (y = starty, n = 1; y <= (int)endy; ++y) {
        
        int startx, endx, x;
        float d0, d1, x0, x1;

        n += n == 1 && y >= t[1].y;
        d0 = steps[0] != 0.0F ? steps[0] * ((float)y - t[0].y) : 1.0F;
        d1 = steps[n] != 0.0F ? steps[n] * ((float)y - t[n - 1].y) : 1.0F;
        x0 = t[0].x + difx[0] * pxMax(d0, 0.0F); 
        x1 = t[n - 1].x + difx[n] * pxMax(d1, 0.0F);
        
        if (x1 < x0) {
            pxSwap(x0, x1, float);
        }
        
        startx = pxMax(x0, 0);
        endx = pxMin(x1 + 1, resx);
        
        for (x = startx; x < endx; ++x) {
            
            vec2 p;
            unsigned long j;
            float sum = 0.0F;
            
            p.x = x;
            p.y = y;
            
            for (j = 0; j < sizeof(P) / sizeof(P[0]); ++j) {
                vec2 q;
                q.x = p.x + 0.5F * P[j].x;
                q.y = p.y + 0.5F * P[j].y;
                sum += (float)( vec2_determinant(p1, p2, q) >= 0.0F &&
                                vec2_determinant(p2, p0, q) >= 0.0F &&
                                vec2_determinant(p0, p1, q) >= 0.0F);
            }

            pxAt(texture, x, y) = pxLerp(pxAt(texture, x, y), color, sum * ni);
        }
    }
}

Px pxTexMap(const Tex2D texture, vec2 uv)
{
    float u = uv.x - (float)(int)uv.x, v = uv.y - (float)(int)uv.y;
    float x = (float)texture.width * u, y = (float)texture.height * v;
    return pxAt(texture, (int)x, (int)y);
}

void pxMapTri2D(const Tex2D fb, const Tex2D texture, Vert2D p0, Vert2D p1, Vert2D p2)
{
    static const vec2 P[] = {
        {0.0F, 0.0F}, {1.0F, 0.0F}, {0.0F, -1.0F}, {-1.0F, 0.0F}, {0.0F, 1.0F}
        ,{1.0F, 1.0F}, {1.0F, -1.0F}, {-1.0F, -1.0F}, {-1.0F, 1.0F}
    };

    const int resx = fb.width - 1, resy = fb.height - 1;
    const float ni = 1.0F / (sizeof(P) / sizeof(P[0]));
    float difx[3], difu[3], difv[3], steps[3];
    int y, n, starty, endy;
    Vert2D t[3];

    t[0] = p0;
    t[1] = p1;
    t[2] = p2;
    pxSortTriVert2D(t);

    starty = pxMax(t[0].pos.y, 0);
    endy = pxMin(t[2].pos.y, resy);
    
    difx[0] = t[2].pos.x - t[0].pos.x;
    difx[1] = t[1].pos.x - t[0].pos.x;
    difx[2] = t[2].pos.x - t[1].pos.x;
    
    difu[0] = t[2].uv.x - t[0].uv.x;
    difv[0] = t[2].uv.y - t[0].uv.y;
    difu[1] = t[1].uv.x - t[0].uv.x;
    difv[1] = t[1].uv.y - t[0].uv.y;
    difu[2] = t[2].uv.x - t[1].uv.x;
    difv[2] = t[2].uv.y - t[1].uv.y;

    steps[0] = t[2].pos.y - t[0].pos.y ? 1.0F / (t[2].pos.y - t[0].pos.y) : 0.0F;
    steps[1] = t[1].pos.y - t[0].pos.y ? 1.0F / (t[1].pos.y - t[0].pos.y) : 0.0F;
    steps[2] = t[2].pos.y - t[1].pos.y ? 1.0F / (t[2].pos.y - t[1].pos.y) : 0.0F;

    for (y = starty, n = 1; y <= (int)endy; ++y) {
        
        vec2 uv0, uv1;
        int x, startx, endx;
        float d0, d1, x0, x1;

        n += n == 1 && y >= t[1].pos.y; 
        d0 = steps[0] != 0.0F ? steps[0] * ((float)y - t[0].pos.y) : 1.0F;
        d1 = steps[n] != 0.0F ? steps[n] * ((float)y - t[n - 1].pos.y) : 1.0F;
        x0 = t[0].pos.x + difx[0] * pxMax(d0, 0.0F); 
        x1 = t[n - 1].pos.x + difx[n] * pxMax(d1, 0.0F);
        
        uv0.x = t[0].uv.x + difu[0] * pxMax(d0, 0.0F);
        uv0.y = t[0].uv.y + difv[0] * pxMax(d0, 0.0F);

        uv1.x = t[n - 1].uv.x + difu[n] * pxMax(d1, 0.0F);
        uv1.y = t[n - 1].uv.y + difv[n] * pxMax(d1, 0.0F);

        if (x1 < x0) {
            pxSwap(x0, x1, float);
            pxSwap(uv0, uv1, vec2);
        }
        
        startx = pxMax(x0, 0);
        endx = pxMin(x1 + 1, resx);
        
        for (x = startx; x < endx; ++x) {
            
            Px color;
            vec2 p, uv;
            unsigned long j;
            float t, sum = 0.0F;
            
            t = normalize(x0, x1, (float)x),
            uv = vec2_mix(uv0, uv1, t);
            p.x =(float)x;
            p.y = (float)y;

            for (j = 0; j < sizeof(P) / sizeof(p); ++j) {
                vec2 q;
                q.x = p.x + 0.5F * P[j].x;
                q.y = p.y + 0.5F * P[j].y;
                sum += (float)( vec2_determinant(p1.pos, p2.pos, q) >= 0.0F &&
                                vec2_determinant(p2.pos, p0.pos, q) >= 0.0F &&
                                vec2_determinant(p0.pos, p1.pos, q) >= 0.0F);
            }
            
            color = pxTexMap(texture, uv);
            pxBlend(fb, x, y, ni * sum, color);

        }
    }
}

void pxPlotCircle(const Tex2D texture, ivec2 p, float r, const Px color)
{
    int x, y;
    const float sqr = r * r;
    const int resx = texture.width - 1, resy = texture.height - 1;
    const int startx = pxClamp(p.x - r, 0, resx);
    const int starty = pxClamp(p.y - r, 0, resy);
    const int endx = pxClamp(p.x + r + 1.0F, 0, resx);
    const int endy = pxClamp(p.y + r + 1.0F, 0, resy);
    for (y = starty; y <= endy; ++y) {
        float dy = p.y - y + 0.5F;
        dy *= dy;
        for (x = startx + 1; x <= endx; ++x) {
            float dx = p.x - x + 0.5F;
            if (dx * dx + dy <= sqr) {
                pxAt(texture, x, y) = color;
            }
        }
    }
}

void pxPlotCircleSmooth(const Tex2D texture, ivec2 p, float r, const Px color)
{
    int x, y, sx, sy;
    const float sqr = r * r;
    static const float subscale = 1.0F / (float)SPXP_SUBSAMPLES;
    const int resx = texture.width - 1, resy = texture.height - 1;
    const int startx = pxClamp(p.x - r - 1.0F, 0, resx);
    const int starty = pxClamp(p.y - r - 1.0F, 0, resy);
    const int endx = pxClamp(p.x + r + 1.0F, 0, resx);
    const int endy = pxClamp(p.y + r + 1.0F, 0, resy);
    for (y = starty; y <= endy; ++y) {
        const float dy = p.y - (float)y + 0.5F;
        for (x = startx + 1; x <= endx; ++x) {
            
            int count = 0;
            float dx = p.x - (float)x + 0.5F, n;
            
            for (sy = 0; sy < SPXP_SUBSAMPLES; ++sy) {
                float sdy = dy + (float)sy * subscale;
                sdy *= sdy;
                for (sx = 0; sx < SPXP_SUBSAMPLES; ++sx) {
                    float sdx = dx + (float)sx * subscale;
                    count += (sdx * sdx + sdy) <= sqr;
                }
            }
            
            n = (float)count / (float)(SPXP_SUBSAMPLES * SPXP_SUBSAMPLES);
            pxBlend(texture, x, y, n, color);
        }
    }
}

#endif /* SPXP_APPLICATION */
#endif /* SIMPLE_PIXEL_PLOTTER_H */

