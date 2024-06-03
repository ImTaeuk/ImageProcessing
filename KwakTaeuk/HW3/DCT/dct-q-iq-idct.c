#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int size = 512, N = 8;
double pi = 3.14159265358979323846, Qp = 5.0;

double qTable[8][8] = {
        {16.0, 11.0, 10.0, 16.0, 24.0, 40.0, 51.0, 61.0},
        {12.0, 12.0, 14.0, 19.0, 26.0, 58.0, 60.0, 55.0},
        {14.0, 13.0, 16.0, 24.0, 40.0, 57.0, 69.0, 56.0},
        {14.0, 17.0, 22.0, 29.0, 51.0, 87.0, 80.0, 62.0},
        {18.0, 22.0, 37.0, 56.0, 68.0, 109.0, 103.0, 77.0},
        {24.0, 35.0, 55.0, 64.0, 81.0, 104.0, 113.0, 92.0},
        {49.0, 64.0, 78.0, 87.0, 103.0, 121.0, 120.0, 101.0},
        {72.0, 92.0, 95.0, 98.0, 112.0, 100.0, 103.0, 99.0}
};

double alpha(int k) {
    return k == 0 ? 1.0 / sqrt(N) : sqrt(2.0 / N);
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
            dct[u][v] = round((alpha(u) * alpha(v) * sum) / (qTable[u][v] * Qp));
        }
    }
}

void ProcessIDCT(double** dctBlock, double** idctBlock)
{
    double sum = 0;

    // IQ
    for (int x = 0; x < N; x++)
    {
        for (int y = 0; y < N; y++)
        {
            dctBlock[x][y] *= qTable[x][y] * Qp;
        }
    }

    for (int x = 0; x < N; x++)
    {
        for (int y = 0; y < N; y++)
        {
            double sum = 0.0;
            for (int u = 0; u < N; u++)
            {
                for (int v = 0; v < N; v++)
                {
                    sum += alpha(u) * alpha(v) * dctBlock[u][v] * cos((pi * (2 * x + 1) * u) / (2 * N)) * cos((pi * (2 * y + 1) * v) / (2 * N));
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

    for (int x = 0; x < size; x += N)
    {
        for (int y = 0; y < size; y += N)
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

    for (int i = 0; i < size; i++)
    {
        free(idctBlock[i]);
        free(dctBlock[i]);
    }
    free(idctBlock);
    free(dctBlock);

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
    result = fopen("dct-q.img", "wb");
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
    result = fopen("iq-idct.img", "wb");
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
