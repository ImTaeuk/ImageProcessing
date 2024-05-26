#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int width = 512, height = 512;
int dx = 100, dy = 80;

int main()
{
    FILE* origin, * result;
    unsigned char** in, ** out, ** buffer;
    int i, j;

    // 메모리 할당
    in = (unsigned char**)malloc(sizeof(unsigned char*) * height);
    out = (unsigned char**)malloc(sizeof(unsigned char*) * height);
    buffer = (unsigned char**)malloc(sizeof(unsigned char*) * height);

    for (i = 0; i < height; i++)
    {
        in[i] = (unsigned char*)malloc(sizeof(unsigned char) * width);
        buffer[i] = (unsigned char*)malloc(sizeof(unsigned char) * width);
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

    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
            out[i][j] = 0;

    // Scaling
    for (i = 0; i < height / 2; i++)
    {
        for (j = 0; j < width / 2; j++)
        {
            buffer[i][j] = in[i * 2][j * 2];
        }
    }

    // Translate
    for (i = 0; i < height / 2; i++)
    {
        for (j = 0; j < width / 2; j++)
        {
            out[i + 100][j + 80] = buffer[i][j];
        }
    }

    // 파일 쓰기
    result = fopen("HW5.img", "wb");
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
        free(buffer[i]);
    }
    free(in);
    free(out);
    free(buffer);

    return 0;
}
