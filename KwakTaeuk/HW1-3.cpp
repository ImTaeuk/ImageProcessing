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
	double val;

	//read
	in = (unsigned char**)malloc(sizeof(unsigned char*) * height);
	out = (unsigned char**)malloc(sizeof(unsigned char*) * height);

	for (i = 0; i < height; i++)
	{
		in[i] = (unsigned char*)malloc(sizeof(unsigned char) * width);
	}

	for (i = 0; i < height; i++)
	{
		out[i] = (unsigned char*)malloc(sizeof(unsigned char) * width);
	}

	origin = fopen("lena.img", "rb");
	for (i = 0; i < height; i++)
	{
		fread(in[i], sizeof(unsigned char), width, origin);
	}

	fclose(origin);


	/// Process
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
			{
				out[i][j] = in[i][j];
				continue;
			}

			out[i][j] =
				(in[i - 1][j - 1] + in[i - 1][j] + in[i - 1][j + 1] +
				in[i][j - 1] + in[i][j] + in[i][j + 1] +
				in[i + 1][j - 1] + in[i + 1][j] + in[i + 1][j + 1]) / 9;

			if (out[i][j] > 256)
				out[i][j] = 255;
			if (out[i][j] < 0)
				out[i][j] = 0;
				
		}
	}
	///

	result = fopen("HW3.img", "wb");

	for (i = 0; i < height; i++)
	{
		fwrite(out[i], sizeof(unsigned char), width, result);
	}

	fclose(result);


	// Memory free
	for (i = 0; i < height; i++)
	{
		free(in[i]);
	}
	free(in);

	for (i = 0; i < height; i++)
	{
		free(out[i]);
	}
	free(out);

	// value가 0~255 값 범위에 존재하지 않을 가능성은 0

	return 0;
}