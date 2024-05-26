#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int width = 512, height = 512;

int main()
{
    FILE* origin, * result;
    unsigned char** in, ** out;
    int i, j;

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

    // Sobel 연산
    for (i = 1; i < height - 1; i++)
    {
        for (j = 1; j < width - 1; j++)
        {
            int val;

            val = in[i - 1][j - 1] + 2 * in[i - 1][j] + in[i - 1][j + 1];
            val -= (in[i + 1][j - 1] + 2 * in[i + 1][j] + in[i + 1][j + 1]);

            // 클리핑
            if (val > 255) val = 255;
            if (val < 0) val = 0;

            out[i][j] = (unsigned char)val;
        }
    }

    // 파일 쓰기
    result = fopen("HW4.img", "wb");
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
