#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

using namespace std;

int width = 256, height = 256;
double pi = 3.141592;

unsigned char** IDFT(unsigned char** F, int N)
{
    unsigned char** f;
    int u, v, m, n;
    double mag, real, imag, max = -1, min = 256;

    f = (unsigned char**)malloc(sizeof(unsigned char*) * N);

    for (u = 0; u < N; u++)
    {
        f[u] = (unsigned char*)malloc(sizeof(unsigned char) * N);
    }

    for (m = 0; m < N; m++)
    {
        for (n = 0; n < N; n++)
        {
            real = 0;
            for (u = 0; u < N; u++)
            {
                for (v = 0; v < N; v++)
                {
                    double theta = 2.0 * pi * (double)(u * m + v * n) / 256.0;
                    real += F[u][v] * cos(theta);
                }
            }
            f[m][n] = real * pow(-1, m + n);
            cout << (double)f[m][n] << '\n';

            if (max < f[m][n])
                max = f[m][n];

            if (min > f[m][n])
                min = f[m][n];
        }
    }

    for (m = 0; m < N; m++)
    {
        for (n = 0; n < N; n++)
        {
            f[m][n] = (f[m][n] - min) * 255 / (max - min);

            if (f[m][n] > 255)
                f[m][n] = 255;
            if (f[m][n] < 0)
                f[m][n] = 0;
        }
    }

    return f;
}

unsigned char** ReadImage()
{
    FILE* origin;

    unsigned char** in;
    int i;

    in = (unsigned char**)malloc(sizeof(unsigned char*) * height);

    for (i = 0; i < height; i++)
    {
        in[i] = (unsigned char*)malloc(sizeof(unsigned char) * width);
    }

    // 파일 읽기
    origin = fopen("HW2-1.img", "rb");
    if (origin == NULL) {
        perror("Error opening file");
    }

    for (i = 0; i < height; i++)
    {
        fread(in[i], sizeof(unsigned char), width, origin);
    }
    fclose(origin);


    return in;
}

int main()
{
    FILE* result;
    unsigned char** in, ** out;
    int i, j;

    double maxOrigin = 0.0, minOrigin = 255.0, maxResult = 0.0, minResult = 256.0;

    out = (unsigned char**)malloc(sizeof(unsigned char*) * height);

    printf("reading..\n");
    in = ReadImage();
    printf("read done\n");

    printf("\nGetting ScalingCoeff\n");


    out = IDFT(in, 256);




    // 파일 쓰기
    result = fopen("HW2-2.img", "wb");
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
