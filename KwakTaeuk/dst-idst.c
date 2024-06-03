#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int size = 512, N = 8;
double pi = 3.14159265358979323846;

unsigned char** ReadImage()
{
    FILE* origin;

    unsigned char** in;
    int i;

    in = (unsigned char**)malloc(sizeof(unsigned char*) * size);

    for (i = 0; i < size; i++)
    {
        in[i] = (unsigned char*)malloc(sizeof(unsigned char) * size);
    }

    // 파일 읽기
    origin = fopen("lena.img", "rb");
    if (origin == NULL) {
        perror("Error opening file");
    }

    for (i = 0; i < size; i++)
    {
        fread(in[i], sizeof(unsigned char), size, origin);
    }
    fclose(origin);


    return in;
}

void ProcessDST(double** block, double** dst)
{
    int u, v, x, y;

    for (u = 0; u < N; u++)
    {
        for (v = 0; v < N; v++)
        {
            double sum = 0.0;
            for (x = 0; x < N; x++)
            {
                for (y = 0; y < N; y++)
                {
                    sum += block[x][y] * sin(pi * (x + 1.0) * (u + 1.0) / (N + 1.0)) * sin(pi * (y + 1.0) * (v + 1.0) / (N + 1));
                }
            }
            dst[u][v] = sum * (2.0 / (N + 1.0));
        }
    }
}

void ProcessIDST(double** dstBlock, double** idstBlock)
{
    double sum = 0;

    for (int x = 0; x < N; x++)
    {
        for (int y = 0; y < N; y++)
        {
            double sum = 0.0;
            for (int u = 0; u < N; u++)
            {
                for (int v = 0; v < N; v++)
                {
                    sum += dstBlock[u][v] * sin(pi * (x + 1.0) * (u + 1.0) / (N + 1.0)) * sin(pi * (y + 1.0) * (v + 1.0) / (N + 1.0));
                }
            }
            idstBlock[x][y] = sum * (2.0 / (N + 1.0));
        }
    }
}



double** GetDST(unsigned char** in)
{
    double** dst = (double**)malloc(sizeof(double*) * size);

    double** block, ** dstBuffer;

    //init
    for (int i = 0; i < size; i++)
        dst[i] = (double*)malloc(sizeof(double) * size);

    block = (double**)malloc(sizeof(double*) * N);
    dstBuffer = (double**)malloc(sizeof(double*) * N);

    for (int i = 0; i < N; i++)
    {
        block[i] = (double*)malloc(sizeof(double) * N);
        dstBuffer[i] = (double*)malloc(sizeof(double) * N);
    }
    //

    for (int row = 0; row < size; row += N)
    {
        for (int col = 0; col < size; col += N)
        {
            // copy origin image
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    block[i][j] = in[row + i][col + j];
                }
            }
            //

            ProcessDST(block, dstBuffer);

            // copy result
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    dst[row + i][col + j] = dstBuffer[i][j];
                }
            }

        }
    }

    for (int i = 0; i < N; i++)
    {
        free(block[i]);
        free(dstBuffer[i]);
    }

    free(block);
    free(dstBuffer);

    return dst;
}



unsigned char** GetIDST(double** dst)
{
    unsigned char** idst = (unsigned char**)malloc(sizeof(unsigned char*) * size);
    double** idstBlock = (double**)malloc(sizeof(double*) * size);
    double** dstBlock = (double**)malloc(sizeof(double*) * size);

    for (int i = 0; i < size; i++)
    {
        idst[i] = (unsigned char*)malloc(sizeof(unsigned char) * size);
        idstBlock[i] = (double*)malloc(sizeof(double) * size);
        dstBlock[i] = (double*)malloc(sizeof(double) * size);
    }

    for (int x = 0; x < size; x += N)
    {
        for (int y = 0; y < size; y += N)
        {
            // dstBlock Copy
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    dstBlock[i][j] = dst[x + i][y + j];
                }
            }

            ProcessIDST(dstBlock, idstBlock);

            for (int u = 0; u < N; u++)
            {
                for (int v = 0; v < N; v++)
                {
                    idst[x + u][y + v] = idstBlock[u][v];

                }
            }
        }
    }

    for (int i = 0; i < size; i++)
    {
        free(idstBlock[i]);
        free(dstBlock[i]);
    }

    return idst;
}

int main()
{
    FILE* result;
    unsigned char** in, ** dst, ** idst;
    double** dstVals;
    int i;

    dst = (unsigned char**)malloc(sizeof(unsigned char*) * size);
    for (int i = 0; i < size; i++)
        dst[i] = (unsigned char*)malloc(sizeof(unsigned char) * size);


    in = ReadImage();

    dstVals = GetDST(in);

    idst = GetIDST(dstVals);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (dstVals[i][j] > 255)
                dstVals[i][j] = 255;
            else if (dstVals[i][j] < 0)
                dstVals[i][j] = 0;
            dst[i][j] = (unsigned char)dstVals[i][j];
        }
    }

    // 파일 쓰기
    result = fopen("dst.img", "wb");
    if (result == NULL) {
        perror("Error opening file");
        return 1;
    }
    for (i = 0; i < size; i++)
    {
        fwrite(dst[i], sizeof(unsigned char), size, result);
    }
    fclose(result);


    // 파일 쓰기
    result = fopen("idst.img", "wb");
    if (result == NULL) {
        perror("Error opening file");
        return 1;
    }
    for (i = 0; i < size; i++)
    {
        fwrite(idst[i], sizeof(unsigned char), size, result);
    }
    fclose(result);

    // 메모리 해제
    for (i = 0; i < size; i++)
    {
        free(in[i]);
        free(dst[i]);
        free(idst[i]);
        free(dstVals[i]);
    }
    free(in);
    free(dst);
    free(idst);
    free(dstVals);

    return 0;
}
