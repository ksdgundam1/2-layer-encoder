#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

const unsigned char* seven_comma_four_hamming_encoder(const unsigned char*);
const unsigned char* seven_comma_four_hamming_decoder(const unsigned char*);

int main()
{
	const unsigned char* break_condition = "break";

	printf("2-layer encoder & decoder\n\n");
	while (1)
	{
		unsigned char bin_data[5];
		unsigned char bin_encoded_data[8];

		printf("==Transmitter==\n");
		printf("input 4-bits data (either input break to exit program)\n>> ");	//inform to user
		scanf("%s", &bin_data);

		if (strcmp(break_condition, bin_data) == 0)						//check break condition
			break;

		printf("\n(7,4) hamming encoder output\n>>%s", seven_comma_four_hamming_encoder(bin_data));

		printf("\n\n\n==Receiver==\n");
		printf("input 7-bits data (either input break to exit program)\n>> ");	//inform to user
		scanf("%s", &bin_encoded_data);

		if (strcmp(break_condition, bin_encoded_data) == 0)						//check break condition
			break;

		printf("\n(7,4) hamming decoder output\n>>%s", seven_comma_four_hamming_decoder(bin_encoded_data));

		bin_data[0] = NULL;														//initialize character array
		bin_encoded_data[0] = NULL;
		printf("\n\n============================================================\n\n");
	}
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
const unsigned char* seven_comma_four_hamming_decoder(const unsigned char* binary_data)
{
	static unsigned char decoder_output[5];
	const unsigned char parity_check_matrix[3][8] = { "1110100", "0111010", "1011001" };
	int parity_bit[3] = { 0, 0, 0 };
	int syndrome = 0;

	for (int iter = 0; iter < 4; iter++)												//initialize decoder_output
		decoder_output[iter] = binary_data[iter];
	decoder_output[4] = NULL;

	for (int iter = 0; iter < 3; iter++)
	{
		for (int jter = 0; jter < 7; jter++)
			if ((parity_check_matrix[iter][jter] == '1') && (binary_data[jter] == 1))
				parity_bit[iter] += 1;
		
		parity_bit[iter] = parity_bit[iter] % 2;
	}

	syndrome = parity_bit[0] * 100 + parity_bit[1] * 10 + parity_bit[2];

	switch (syndrome)
	{
	case 000:
		break;																			//all bits are seemed normal, do nothing

	case 101:																			//r1 bit error 
		if (decoder_output[0] == '1')
			decoder_output[0] = '0';
		else if (decoder_output[0] == '0')
			decoder_output[0] = '1';

	case 110:																			//r2 bit error
		if (decoder_output[1] == '1')
			decoder_output[1] = '0';
		else if (decoder_output[1] == '0')
			decoder_output[1] = '1';

	case 111:																			//r3 bit error
		if (decoder_output[2] == '1')
			decoder_output[2] = '0';
		else if (decoder_output[2] == '0')
			decoder_output[2] = '1';

	default:;																			//other cases are out of scope, since the real data in clear
	}

	return decoder_output;
}
