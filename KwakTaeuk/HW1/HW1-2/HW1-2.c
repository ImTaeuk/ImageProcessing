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

	//read
	in = (unsigned char**)malloc(sizeof(unsigned char*) * height);
	out = (unsigned char**)malloc(sizeof(unsigned char*) * height * 2);

	for (i = 0; i < height; i++)
	{
		in[i] = (unsigned char*)malloc(sizeof(unsigned char) * width);
	}

	for (i = 0; i < height * 2; i++)
	{
		out[i] = (unsigned char*)malloc(sizeof(unsigned char) * (width * 2));
	}

	origin = fopen("lena.img", "rb");
	for (i = 0; i < height; i++)
	{
		fread(in[i], sizeof(unsigned char), width, origin);
	}

	fclose(origin);


	// Process
	for (i = 0; i < height * 2; i++)
	{
		for (j = 0; j < width * 2; j++)
		{
			out[i][j] = in[i / 2][j / 2];
		}
	}
	//

	result = fopen("HW1-2.img", "wb");

	for (i = 0; i < height * 2; i++)
	{
		fwrite(out[i], sizeof(unsigned char), width * 2, result);
	}

	fclose(result);


	// Memory free
	for (i = 0; i < height; i++)
	{
		free(in[i]);
	}
	free(in);

	for (i = 0; i < height * 2; i++)
	{
		free(out[i]);
	}
	free(out);

	// value가 0~255 값 범위에 존재하지 않을 가능성은 0

	return 0;
}