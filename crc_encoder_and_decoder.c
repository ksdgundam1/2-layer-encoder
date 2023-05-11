#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

const unsigned char* crc_encoder(const unsigned char*);
const unsigned char* crc_decoder(const unsigned char*);

int main()
{
	const char* break_condition = "break";

	printf("CRC encoder & decoder\n\n");
	while (1) {
		unsigned char bin_data[17];														//16 bits input data and 1 NULL character
		unsigned char bin_encoded_data[25];												//24 bits encoded input data and 1 NULL character
		unsigned char* crc_output;														//24 bits output data and 1 NULL character

		printf("==Transmitter==\n");
		printf("input 16-bits data (either input 'break' to exit this program)\n>>");	//inform to user
		scanf("%s", &bin_data);															//get data from keyboard
		
		if (strcmp(break_condition, bin_data) == 0)										//check break condition
			break;

		crc_output = crc_encoder(bin_data);

		printf("\nCRC encoder output\n>> ");											//prinit crc_output 4 bits as a time
		for (int iter = 0; iter < 6; iter++)
		{
			printf("%c%c%c%c ", crc_output[iter * 4], crc_output[iter * 4 + 1], crc_output[iter * 4 + 2], crc_output[iter * 4 + 3]);
		}

		printf("\n\n\n==Receiver==\n");
		printf("input 24-bits encoded data (either input 'break' to exit this program)\n>>");
		scanf("%s", &bin_encoded_data);													//get data from keyboard

		if (strcmp(break_condition, bin_encoded_data) == 0)								//check break condition
			break;

		printf("\nCRC decoder output\n>>");												//print crc_output
		crc_output = crc_decoder(bin_encoded_data);										//input corrected data to crc_decoder

		if (strcmp(crc_output, "ERROR!!") != 0)											//if CRC can not detect the error
			for (int iter = 0; iter < 4; iter++)										//print it as 4 bits at a time
			{
				printf("%c%c%c%c ", crc_output[iter * 4], crc_output[iter * 4 + 1], crc_output[iter * 4 + 2], crc_output[iter * 4 + 3]);
			}
		else																			//if error detected
			printf("%s", crc_output);													//print error message

		bin_data[0] = NULL;																//initialize character array
		printf("\n\n============================================================\n\n");
	}

	return 0;
}

const unsigned char* crc_encoder(const unsigned char* binary_data)
{
	static unsigned char encoded_data[25];

	unsigned char divisor[10] = { '1', '0', '0', '0', '0', '0', '1', '1', '1', NULL };	//save divisor

	for (int iter = 0; iter < 16; iter++)		//upper k bits of crc_output is same as input data
		encoded_data[iter] = binary_data[iter];
	for (int iter = 0; iter < 8; iter++)		//(n-k) LSB is 0 (shifted)
		encoded_data[iter + 16] = '0';
	encoded_data[24] = NULL;

	for (int iter = 0; iter + 8 < 24; iter++)	//divide (n-k) times
	{
		if (encoded_data[iter] == '1')				//divide by 1 * divisor
		{
			for (int jter = 0; jter < 9; jter++)
			{
				if ((encoded_data[iter + jter] ^ divisor[jter]) == 1)			//do modulo 2 division
					encoded_data[iter + jter] = '1';							//type casting from integer output to character
				else if ((encoded_data[iter + jter] ^ divisor[jter]) == 0)		//same as above
					encoded_data[iter + jter] = '0';
			}
		}
		else if (encoded_data[iter] == '0');	//nothing happend because we xor by 000...00
	}
	for (int iter = 0; iter < 16; iter++)									//(n-k) LSB of crc_output is same as remainder
		encoded_data[iter] = binary_data[iter];

	return encoded_data;
}
const unsigned char* crc_decoder(const unsigned char* binary_data)
{
	const unsigned char* errorCode = "ERROR!!";
	static unsigned char decoded_data[17];
	unsigned char* temporal = binary_data;

	unsigned char divisor[10] = { '1', '0', '0', '0', '0', '0', '1', '1', '1', NULL };	//save divisor

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