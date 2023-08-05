#define SPXP_APPLICATION
#include <spxplot.h>
#include <stdlib.h>
#include <stdio.h>

static int pxImageSavePpm(const Tex2D img, const char* path)
{
    int i, x, y;
    FILE* file;

    file = fopen(path, "wb");
    if (!file) {
        fprintf(stderr, "pxplot could not write file: '%s'\n", path);
        return EXIT_FAILURE;
    }

    fprintf(file, "P6 %d %d 255\n", img.width, img.height);
    for (y = 0, i = 0; y < img.height; ++y) {
        for (x = 0; x < img.width; ++x, ++i) {
            fwrite(img.pixbuf + i, 1, 3, file);
        }
    }
    
    return fclose(file);
}

Tex2D spxTextureCreate(int width, int height)
{
    Tex2D tex;
    tex.width = width;
    tex.height = height;
    tex.pixbuf = (Px*)calloc(width * height, sizeof(Px));
    return tex;
}

void spxTextureFree(Tex2D* tex)
{
    if (tex->pixbuf) {
        free(tex->pixbuf);
        tex->pixbuf = NULL;
        tex->width = 0;
        tex->height = 0;
    }
}

int main(void)
{
    const Px red = {255, 0, 0, 255};
    const ivec2 p = {200, 200};
    
    Tex2D tex = spxTextureCreate(400, 400);
    pxPlotCircleSmooth(tex, p, 50.0F, red);
    pxImageSavePpm(tex, "output.ppm");
    
    spxTextureFree(&tex);
    return EXIT_SUCCESS;
}

