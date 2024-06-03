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

    Initialize(buffer, width, height);
    Initialize(out, width, height);

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

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            int x = j - width / 2;
            int y = i - height / 2;

            int x_ = x * cos(theta) - y * sin(theta);
            int y_ = x * sin(theta) + y * cos(theta);

            x_ += width / 2 + dx;
            y_ += height / 2 + dy;

            if (y_ < height && y_ >= 0 && x_ < width && x_ >= 0) {
                out[(int)y_][(int)x_] = buffer[i][j];
            }
        }
    }


    // Hole process
    Initialize(buffer, width, height);

    for (i = 1; i < height - 1; i++)
    {
        for (j = 1; j < width - 1; j++)
        {
            int cnt = 0;
            double sum = 0;
            if (out[i][j] <= 0)
            {
                if (out[i][j + 1] > 0) {
                    sum += out[i][j + 1];
                    cnt++;
                }
                if (out[i][j - 1] > 0)
                {
                    sum += out[i][j - 1];
                    cnt++;
                }
                if (out[i + 1][j] > 0)
                {
                    sum += out[i + 1][j];
                    cnt++;
                }
                if (out[i - 1][j] > 0)
                {
                    sum += out[i - 1][j];
                    cnt++;
                }
                buffer[i][j] = sum / (double)cnt;
            }
            else
                buffer[i][j] = out[i][j];

            if (buffer[i][j] > 255)
                buffer[i][j] = 255;
            if (buffer[i][j] < 0)
                buffer[i][j] = 0;
        }
    }


    // 파일 쓰기
    result = fopen("HW1-6.img", "wb");
    if (result == NULL) {
        perror("Error opening file");
        return 1;
    }
    for (i = 0; i < height; i++)
    {
        fwrite(buffer[i], sizeof(unsigned char), width, result);
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
