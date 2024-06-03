#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int width = 256, height = 256;
double pi = 3.141592;
double ** re, **im;

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

unsigned char** DFT(unsigned char** f, int N)
{
    unsigned char** scalingCoeff;

    int u, v;
    double dcValue = 0;
    scalingCoeff = (unsigned char**)malloc(sizeof(unsigned char*) * N);
    re = (double**)malloc(sizeof(double*) * N);
    im = (double**)malloc(sizeof(double*) * N);

    for (u = 0; u < N; u++)
    {
        scalingCoeff[u] = (unsigned char*)malloc(sizeof(unsigned char) * N);
        re[u] = (double*)malloc(sizeof(double) * N);
        im[u] = (double*)malloc(sizeof(double) * N);
    }

    for (u = 0; u < N; u++)
    {
        for (v = 0; v < N; v++)
        {
            dcValue += pow(-1, u + v) * f[u][v];
        }
    }



    for (u = 0; u < N; u++)
    {
        for (v = 0; v < N; v++)
        {
            double real = 0, imag = 0;

            for (int m = 0; m < N; m++)
            {
                for (int n = 0; n < N; n++)
                {
                    double theta = -2 * pi * (u * m / 256.0 + v * n / 256.0);

                    real += pow(-1, m + n) * f[m][n] * cos(theta) / (N * N);
                    imag += pow(-1, m + n) * f[m][n] * -sin(theta) / (N * N);
                }
            }
            re[u][v] = real;
            im[u][v] = imag;
        }
    }

    dcValue = sqrt(pow(re[128][128], 2) + pow(im[128][128], 2));

    for (u = 0; u < N; u++)
    {
        for (v = 0; v < N; v++)
        {
            double mag = sqrt(pow(re[u][v], 2) + pow(im[u][v], 2));

            scalingCoeff[u][v] = 255.0 * log10(fabs(mag) + 1.0) / log10(dcValue + 1.0);

            if (scalingCoeff[u][v] > 255)
                scalingCoeff[u][v] = 255;
            if (scalingCoeff[u][v] < 0)
                scalingCoeff[u][v] = 0;
        }
    }

    return scalingCoeff;
}

unsigned char** IDFT(unsigned char** F, int N)
{
    unsigned char** f;
    int u, v, m, n;
    double real;

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
                    real += cos(theta) * re[u][v] + sin(theta) * im[u][v];
                }
            }

            double val = (real * pow(-1, m + n));

            if (val > 255.0)
                f[m][n] = 255;
            else if (val < 0.0)
                f[m][n] = 0;
            else
                f[m][n] = (unsigned char)val;
        }
    }

    return f;
}



unsigned char** ReadImage()
{
    FILE* origin;

    unsigned char** in, **resized;
    int i, j;

    in = (unsigned char**)malloc(sizeof(unsigned char*) * 512);
    resized = (unsigned char**)malloc(sizeof(unsigned char*) * 256);

    for (i = 0; i < 512; i++)
    {
        in[i] = (unsigned char*)malloc(sizeof(unsigned char) * 512);
    }
    for (i = 0; i < 256; i++)
        resized[i] = (unsigned char*)malloc(sizeof(unsigned char) * 256);

    // 파일 읽기
    origin = fopen("lena.img", "rb");
    if (origin == NULL) {
        perror("Error opening file");
    }

    for (i = 0; i < height * 2; i++)
    {
        fread(in[i], sizeof(unsigned char), width * 2, origin);
    }
    fclose(origin);

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
            resized[i][j] = in[i * 2][j * 2];
    }

    for (i = 0; i < height * 2; i++)
        free(in[i]);

    free(in);

    return resized;
}

int main()
{
    FILE* result;
    unsigned char** in, ** out, ** idft;
    int i;

    out = (unsigned char**)malloc(sizeof(unsigned char*) * height);
    idft = (unsigned char**)malloc(sizeof(unsigned char*) * height);

    in = ReadImage();

    out = DFT(in, 256);

    idft = IDFT(out, 256);


    // 파일 쓰기
    result = fopen("dft.img", "wb");
    if (result == NULL) {
        perror("Error opening file");
        return 1;
    }
    for (i = 0; i < height; i++)
    {
        fwrite(out[i], sizeof(unsigned char), width, result);
    }
    fclose(result);

    result = fopen("idft.img", "wb");
    if (result == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    for (i = 0; i < height; i++)
    {
        fwrite(idft[i], sizeof(unsigned char), width, result);
    }


    // 메모리 해제
    for (i = 0; i < height; i++)
    {
        free(in[i]);
        free(out[i]);
        free(re[i]);
        free(im[i]);
    }
    free(in);
    free(out);
    free(re);
    free(im);

    return 0;
}
