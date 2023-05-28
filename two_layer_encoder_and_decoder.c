#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

const unsigned char* crc_encoder(const unsigned char*);
const unsigned char* seven_comma_four_hamming_encoder(const unsigned char*);
const unsigned char* crc_decoder(const unsigned char*);
const unsigned char* seven_comma_four_hamming_decoder(const unsigned char*);

int main()
{
	const unsigned char *break_condition = "break";

	printf("2-layer encoder & decoder\n\n");

	while (1) {
		unsigned char bin_data[17];												//16 bits input data and 1 NULL character
		unsigned char* crc_output;												//24 bits output data and 1 NULL character
		unsigned char* hamming_output;											//final output from (7, 4) hamming encoder
		unsigned char bin_encoded_data[43];										//42 bits encoded data
		unsigned char temp[5];													//temporal string to save part of crc_output
		unsigned char part_of_encoded_data[8];									//to sepearte each 7 bits from 42 bits decoder input
		unsigned char hamming_decoder_output[25];

		temp[4] = NULL;						
		bin_data[16] = NULL;
		part_of_encoded_data[7] = NULL;
		hamming_decoder_output[24] = NULL;

		printf("==Transmitter==\n");
		printf("input 16-bits data (either input break to exit program)\n>> ");	//inform to user
		scanf("%s", &bin_data);													//get data from keyboard

		if (strcmp(break_condition, bin_data) == 0)								//check break condition
			break;

		crc_output = crc_encoder(bin_data);										//get crc_output

		printf("\n2-layer encoder output\n>> ");								//print final output

		for (int iter = 0; iter < 6; iter++)									//do 6 times cause we have 24bit data from crc encoder
		{
			temp[0] = crc_output[iter * 4];
			temp[1] = crc_output[iter * 4 + 1];
			temp[2] = crc_output[iter * 4 + 2];
			temp[3] = crc_output[iter * 4 + 3];

			hamming_output = seven_comma_four_hamming_encoder(temp);			//part of final data

			printf("%s ", hamming_output);
		}

		printf("\n\n\n==Receiver==\n");
		printf("input 42-bits data (either input break to exit program)\n>> ");	//inform to user
		scanf("%s", &bin_encoded_data);											//get 42-bits encoded data from keyboard
		
		if (strcmp(break_condition, bin_encoded_data) == 0)						//check break condition
			break;

		for (int iter = 0; iter < 6; iter++)									//input to decoder by 7 bits at a time
		{
			part_of_encoded_data[0] = bin_encoded_data[iter * 7];
			part_of_encoded_data[1] = bin_encoded_data[iter * 7 + 1];
			part_of_encoded_data[2] = bin_encoded_data[iter * 7 + 2];
			part_of_encoded_data[3] = bin_encoded_data[iter * 7 + 3];
			part_of_encoded_data[4] = bin_encoded_data[iter * 7 + 4];
			part_of_encoded_data[5] = bin_encoded_data[iter * 7 + 5];
			part_of_encoded_data[6] = bin_encoded_data[iter * 7 + 6];

			hamming_output = seven_comma_four_hamming_decoder(part_of_encoded_data);

			hamming_decoder_output[iter * 4] = hamming_output[0];
			hamming_decoder_output[iter * 4 + 1] = hamming_output[1];
			hamming_decoder_output[iter * 4 + 2] = hamming_output[2];
			hamming_decoder_output[iter * 4 + 3] = hamming_output[3];
		}

		printf("\n2-layer decoder output\n>>");									//print crc_output
		crc_output = crc_decoder(hamming_decoder_output);						//input corrected data to crc_decoder

		if (strcmp(crc_output, "ERROR!!") != 0)
			for (int iter = 0; iter < 4; iter++)
			{
				printf("%c%c%c%c ", crc_output[iter * 4], crc_output[iter * 4 + 1], crc_output[iter * 4 + 2], crc_output[iter * 4 + 3]);
			}
		else
			printf("%s", crc_output);

		bin_data[0] = NULL;														//initialize character array
		printf("\n\n============================================================\n\n");
	}

	return 0;
}

const unsigned char* crc_encoder(const unsigned char* binary_data)
{
	static unsigned char encoded_data[25];

	const unsigned char divisor[10] = { '1', '0', '0', '0', '0', '0', '1', '1', '1', NULL };	//save divisor

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

	const unsigned char generator[4][8] = { "1000101", "0100110", "0010111", "0001011" };		//generator matrix to calculate codeword

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
const unsigned char* crc_decoder(const unsigned char* binary_data)
{
	const unsigned char* errorCode = "ERROR!!";
	static unsigned char decoded_data[17];
	unsigned char* temporal = binary_data;

	const unsigned char divisor[10] = { '1', '0', '0', '0', '0', '0', '1', '1', '1', NULL };	//save divisor

	for (int iter = 0; iter < 16; iter++)												//upper k bits of input data is real 'data'
		decoded_data[iter] = binary_data[iter];
	decoded_data[16] = NULL;

	for (int iter = 0; iter + 8 < 24; iter++)											//divide (n-k) times
	{
		if (binary_data[iter] == '1')													//divide by 1 * divisor
		{
			for (int jter = 0; jter < 9; jter++)
			{
				if ((binary_data[iter + jter] ^ divisor[jter]) == 1)					//do modulo 2 division
					temporal[iter + jter] = '1';										//type casting from integer output to character
				else if ((binary_data[iter + jter] ^ divisor[jter]) == 0)				//same as above
					temporal[iter + jter] = '0';
			}
		}
		else if (decoded_data[iter] == '0');											//nothing happend because we xor by 000...00
	}

	if (strcmp(temporal, "000000000000000000000000") == 0)
		return decoded_data;
	else
		return errorCode;

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
			if ((parity_check_matrix[iter][jter] == '1') && (binary_data[jter] == '1'))
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
		break;

	case 110:																			//r2 bit error
		if (decoder_output[1] == '1')
			decoder_output[1] = '0';
		else if (decoder_output[1] == '0')
			decoder_output[1] = '1';
		break;

	case 111:																			//r3 bit error
		if (decoder_output[2] == '1')
			decoder_output[2] = '0';
		else if (decoder_output[2] == '0')
			decoder_output[2] = '1';
		break;

	case 11:																			//r4 bit error, we use integer, so MSB 0 is discarded
		if (decoder_output[3] == '1')
			decoder_output[3] = '0';
		else if (decoder_output[3] == '0')
			decoder_output[3] = '1';
		break;
			
	default:																			//other cases are out of scope, since the real data in clear
		break;
	}

	return decoder_output;
}
