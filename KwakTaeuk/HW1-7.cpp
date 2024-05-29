#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int width = 512, height = 512;
int dx = 200, dy = 20;
double theta = -0.5;

void Initialize(unsigned char** mat, int w, int h)
{
    int i, j;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            mat[i][j] = 0;
        }
    }

}

void InitHistoVals(double *vals, int num)
{
    for (int i = 0; i < num; i++)
    {
        vals[i] = 0;
    }
}

int main()
{
    FILE* origin, * result;
    unsigned char** in, ** out;
    int i, j;

    double maxOrigin = 0.0, minOrigin = 255.0, maxResult = 0.0, minResult = 256.0;

    // 메모리 할당
    in = (unsigned char**)malloc(sizeof(unsigned char*) * height);
    out = (unsigned char**)malloc(sizeof(unsigned char*) * height);

    for (i = 0; i < height; i++)
    {
        in[i] = (unsigned char*)malloc(sizeof(unsigned char) * width);
        out[i] = (unsigned char*)malloc(sizeof(unsigned char) * width);
    }

    // 파일 읽기
    origin = fopen("lena.img", "rb");
    if (origin == NULL) {
        perror("Error opening file");
        return 1;
    }

    for (i = 0; i < height; i++)
    {
        fread(in[i], sizeof(unsigned char), width, origin);
    }
    fclose(origin);

    // Find min max value
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (minOrigin > in[i][j])
                minOrigin = in[i][j];
            if (maxOrigin < in[i][j])
                maxOrigin = in[i][j];
        }
    }

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            out[i][j] = (in[i][j] - minOrigin) / (maxOrigin - minOrigin) * 255;

            if (out[i][j] > 256)
                out[i][j] = 255;

            if (out[i][j] < 0)
                out[i][j] = 0;

            if (maxResult < out[i][j])
                maxResult = out[i][j];

            if (minResult > out[i][j])
                minResult = out[i][j];
        }
    }

    // 파일 쓰기
    result = fopen("HW7.img", "wb");
    if (result == NULL) {
        perror("Error opening file");
        return 1;
    }
    for (i = 0; i < height; i++)
    {
        fwrite(out[i], sizeof(unsigned char), width, result);
    }
    fclose(result);

    // 메모리 해제
    for (i = 0; i < height; i++)
    {
        free(in[i]);
        free(out[i]);
    }
    free(in);
    free(out);

    return 0;
}
