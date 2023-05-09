#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int main()
{
	while(1){
	unsigned char bin_data[17];			//16 bits input data and 1 NULL character
	unsigned char crc_output[25];		//24 bits output data and 1 NULL character
	unsigned char divisor[10] = { '1', '0', '0', '0', '0', '0', '1', '1', '1', NULL};	//save divisor
	unsigned char temporal[25];			
	crc_output[24] = NULL;				//last character of crc_output is NULL

	printf("input 16-bits data\n>>");	//inform to user
	scanf("%s", bin_data);				//get data from keyboard
	printf("\n");

	for (int iter = 0; iter < 16; iter++)		//upper k bits of crc_output is same as input data
		crc_output[iter] = bin_data[iter];
	for (int iter = 0; iter < 8; iter++)		//(n-k) LSB is 0 (shifted)
		crc_output[iter + 16] = '0';
	crc_output[24] = NULL;

	for (int iter = 0; iter + 8 < 24; iter++)	//divide (n-k) times
	{
		if (crc_output[iter] == '1')				//divide by 1 * divisor
		{
			for (int jter = 0; jter < 9; jter++)
			{
				if ((crc_output[iter + jter] ^ divisor[jter]) == 1)			//do modulo 2 division
					crc_output[iter + jter] = '1';							//type casting from integer output to character
				else if ((crc_output[iter + jter] ^ divisor[jter]) == 0)		//same as above
					crc_output[iter + jter] = '0';
			}
		}
		else if (crc_output[iter] == '0');	//nothing happend because we xor by 000...00
	}
	for (int iter = 0; iter < 16; iter++)									//(n-k) LSB of crc_output is same as remainder
		crc_output[iter] = bin_data[iter];

	printf("CRC incoder output\n>> ");							//prinit crc_output 4 bits as a time
	for (int iter = 0; iter < 6; iter++)
	{
		printf("%c%c%c%c ", crc_output[iter * 4], crc_output[iter * 4 + 1], crc_output[iter * 4 + 2], crc_output[iter * 4 + 3]);
	}
	printf("\n\n");

	}

	return 0;
}