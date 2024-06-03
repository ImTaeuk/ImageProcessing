#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int size = 512, N = 8;
double pi = 3.14159265358979323846;

double alpha(int k) {
    return k == 0 ? 1.0/sqrt(N) : sqrt(2.0 / N);
}


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

void ProcessDCT(double** block, double** dct)
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
                    sum += block[x][y] * cos((pi * (2 * x + 1) * u) / (2 * N)) * cos((pi * (2 * y + 1) * v) / (2 * N));
                }
            }
            dct[u][v] = alpha(u) * alpha(v) * sum;
        }
    }
}

void ProcessIDCT(double** dctBlock, double** idctBlock)
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
                    sum += alpha(u) * alpha(v) * dctBlock[u][v] * cos((pi * (2 * x + 1) * u) / (2 * N)) *  cos((pi * (2 * y + 1) * v) / (2 * N));
                }
            }
            idctBlock[x][y] = sum;
        }
    }
}



double** GetDCT(unsigned char** in)
{
    double** dct = (double**)malloc(sizeof(double*) * size);

    double** block, ** dctBuffer;

    //init
    for (int i = 0; i < size; i++)
        dct[i] = (double*)malloc(sizeof(double) * size);

    block = (double**)malloc(sizeof(double*) * N);
    dctBuffer = (double**)malloc(sizeof(double*) * N);

    for (int i = 0; i < N; i++)
    {
        block[i] = (double*)malloc(sizeof(double) * N);
        dctBuffer[i] = (double*)malloc(sizeof(double) * N);
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

            ProcessDCT(block, dctBuffer);

            // copy result
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    dct[row + i][col + j] = dctBuffer[i][j];
                }
            }

        }
    }

    for (int i = 0; i < N; i++)
    {
        free(block[i]);
        free(dctBuffer[i]);
    }

    free(block);
    free(dctBuffer);

    return dct;
}



unsigned char** GetIDCT(double** dct)
{
    unsigned char** idct = (unsigned char**)malloc(sizeof(unsigned char*) * size);
    double** idctBlock = (double**)malloc(sizeof(double*) * size);
    double** dctBlock = (double**)malloc(sizeof(double*) * size);

    for (int i = 0; i < size; i++)
    {
        idct[i] = (unsigned char*)malloc(sizeof(unsigned char) * size);
        idctBlock[i] = (double*)malloc(sizeof(double) * size);
        dctBlock[i] = (double*)malloc(sizeof(double) * size);
    }

    for (int x = 0; x < size; x+=N)
    {
        for (int y = 0; y < size; y+=N)
        {
            // dctBlock Copy
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    dctBlock[i][j] = dct[x + i][y + j];
                }
            }

            ProcessIDCT(dctBlock, idctBlock);

            for (int u = 0; u < N; u++)
            {
                for (int v = 0; v < N; v++)
                {
                    idct[x + u][y + v] = idctBlock[u][v];
                    
                }
            }
        }
    }

    return idct;
}

int main()
{
    FILE* result;
    unsigned char** in, ** dct, ** idct;
    double** dctVals;
    int i;

    dct = (unsigned char**)malloc(sizeof(unsigned char*) * size);
    for (int i = 0; i < size; i++)
        dct[i] = (unsigned char*)malloc(sizeof(unsigned char) * size);


    printf("reading..\n");
    in = ReadImage();
    printf("read done\n");


    dctVals = GetDCT(in);

    idct = GetIDCT(dctVals);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (dctVals[i][j] > 255)
                dctVals[i][j] = 255;
            else if (dctVals[i][j] < 0)
                dctVals[i][j] = 0;
            dct[i][j] = (unsigned char)dctVals[i][j];
        }
    }

    // 파일 쓰기
    result = fopen("dct.img", "wb");
    if (result == NULL) {
        perror("Error opening file");
        return 1;
    }
    for (i = 0; i < size; i++)
    {
        fwrite(dct[i], sizeof(unsigned char), size, result);
    }
    fclose(result);


    // 파일 쓰기
    result = fopen("idct.img", "wb");
    if (result == NULL) {
        perror("Error opening file");
        return 1;
    }
    for (i = 0; i < size; i++)
    {
        fwrite(idct[i], sizeof(unsigned char), size, result);
    }
    fclose(result);

    // 메모리 해제
    for (i = 0; i < size; i++)
    {
        free(in[i]);
        free(dct[i]);
        free(idct[i]);
        free(dctVals[i]);
    }
    free(in);
    free(dct);
    free(idct);
    free(dctVals);

    return 0;
}
