#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

const unsigned char* crc_encoder(const unsigned char*);
const unsigned char* seven_comma_four_hamming_encoder(const unsigned char*);

int main()
{
	const unsigned char *break_condition = "break";

	while (1) {
		unsigned char bin_data[17];												//16 bits input data and 1 NULL character
		unsigned char* crc_output;												//24 bits output data and 1 NULL character
		unsigned char* hamming_output;											//final output from (7, 4) hamming incoder
		unsigned char temp[5];													//temporal string to save part of crc_output
		temp[4] = NULL;						
		bin_data[16] = NULL;

		printf("input 16-bits data (either input break to exit program)\n>> ");	//inform to user
		scanf("%s", bin_data);													//get data from keyboard

		if (strcmp(break_condition, bin_data) == 0)								//check break condition
			break;

		crc_output = crc_encoder(bin_data);										//get crc_output

		printf("\n2-layer encoder output\n>> ");									//print final output

		for (int iter = 0; iter < 6; iter++)									//do 6 times cause we have 24bit data from crc encoder
		{
			temp[0] = crc_output[iter * 4];
			temp[1] = crc_output[iter * 4 + 1];
			temp[2] = crc_output[iter * 4 + 2];
			temp[3] = crc_output[iter * 4 + 3];

			hamming_output = seven_comma_four_hamming_encoder(temp);			//part of final data

			printf("%s ", hamming_output);
		}
		printf("\n\n============================================================\n\n");
	}

	return 0;
}

const unsigned char* crc_encoder(const unsigned char* binary_data)
{
	static unsigned char encoded_data[25];

	unsigned char divisor[10] = { '1', '0', '0', '0', '0', '0', '1', '1', '1', NULL };	//save divisor

	for (int iter = 0; iter < 16; iter++)												//upper k bits of crc_output is same as input data
		encoded_data[iter] = binary_data[iter];
	for (int iter = 0; iter < 8; iter++)												//(n-k) LSB is 0 (shifted)
		encoded_data[iter + 16] = '0';
	encoded_data[24] = NULL;

	for (int iter = 0; iter + 8 < 24; iter++)											//divide (n-k) times
	{
		if (encoded_data[iter] == '1')													//divide by 1 * divisor
		{
			for (int jter = 0; jter < 9; jter++)
			{
				if ((encoded_data[iter + jter] ^ divisor[jter]) == 1)					//do modulo 2 division
					encoded_data[iter + jter] = '1';									//type casting from integer output to character
				else if ((encoded_data[iter + jter] ^ divisor[jter]) == 0)				//same as above
					encoded_data[iter + jter] = '0';
			}
		}
		else if (encoded_data[iter] == '0');											//nothing happend because we xor by 000...00
	}
	for (int iter = 0; iter < 16; iter++)												//(n-k) LSB of crc_output is same as remainder
		encoded_data[iter] = binary_data[iter];

	return encoded_data;
}
const unsigned char* seven_comma_four_hamming_encoder(const unsigned char* binary_data)
{
	static unsigned char encoded_data[8];												//return value
	unsigned char s_encoded_data[8] = { "0000000" };									//temporal data to calculate

	unsigned char generator[4][8] = { "1000101", "0100110", "0010111", "0001011" };		//generator matrix to calculate codeword

	for (int iter = 0; iter < 4; iter++)
	{
		if (binary_data[iter] == '1')													//product of G^T S is same as the row of generator if the data is 1
		{
			for (int jter = 0; jter < 7; jter++)
			{
				if ((generator[iter][jter] ^ s_encoded_data[jter]) == 1)				//do modulo 2 calculation
					s_encoded_data[jter] = '1';
				else if ((generator[iter][jter] ^ s_encoded_data[jter]) == 0)
					s_encoded_data[jter] = '0';
			}
		}
		else if (binary_data[iter] == '0');												//product of G^T S is zero if the data is 0
	}

	strcpy(encoded_data, s_encoded_data);												//copy temporal data to static data

	return encoded_data;																//return encoded data
}