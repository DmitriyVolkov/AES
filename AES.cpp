#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>

using namespace std;

FILE *i_file;
FILE *k_file;
FILE *o_file;

int i_length;
int k_length;
int o_length;

unsigned char *ui_message;
unsigned char *uk_message;

const int Nb = 4;//число столбцов(32-х битных слов), составляющих
const int Nk = 4;//число 32-х битных слов, составляющих шифроключ.
const int Nr = 10;//число раундов, которое является функцией Nk и Nb. 

int word[Nb][Nb*(Nr + 1)];
int state[Nb][Nb];//массив rcon состоит из битов 32-х разрядного слова и является постоянным для данного раунда
int rcon[Nb][Nr] = { { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
int sbox[] = { 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };
int inv_sbox[] = { 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
//sbox-нелинейная таблица замен, использующаяся в нескольких трансформациях замены байт и в процедуре Key Expansion 
int mul_by_02(int num)
{
	if (num < 0x80)
		return (num << 1) % 0x100;
	else
		return ((num << 1) ^ 0x1b) % 0x100;
}
int mul_by_03(int num)
{
	return mul_by_02(num) ^ num;
}
int mul_by_09(int num)
{
	return mul_by_02(mul_by_02(mul_by_02(num))) ^ num;
}
int mul_by_0b(int num)
{
	return mul_by_02(mul_by_02(mul_by_02(num))) ^ mul_by_02(num) ^ num;
}
int mul_by_0d(int num)
{
	return mul_by_02(mul_by_02(mul_by_02(num))) ^ mul_by_02(mul_by_02(num)) ^ num;
}
int mul_by_0e(int num)
{
	return mul_by_02(mul_by_02(mul_by_02(num))) ^ mul_by_02(mul_by_02(num)) ^ mul_by_02(num);
}

int mix_columns(int inverse);//трансформация при шифровании, которая берет все столбцы State и смешивает их данные (независимо друг от друга), чтобы получить новые столбцы
int sub_bytes(int inverse);//трансформации при шифровании, которые обрабатывают State используя нелинейную таблицу замещения байтов(S-box), применяя её независимо к каждому байту State
int sub_word(int index);//функция, используемая в процедуре Key Expansion, которая берет на входе четырёх-байтное слово и, применяя S-box к каждому из четырёх байтов, выдаёт выходное слово
int shift_rows(int inverse);//трансформации при шифровании, которые обрабатывают State, циклически смещая последние три строки State на разные величины
int shift_word(int index);//функция, использующаяся в процедуре Key Expansion, которая берет 4-х байтное слово и производит над ним циклическую перестановку
int add_rkey(int inverse, int number);//трансформация при шифровании и обратном шифровании, при которой Round Key XOR’ится c State. Длина RoundKey равна размеру State
int key_expansion();
int encryption();
int decryption();

int main(int argc, char *argv[])
{
	int inverse;
	int length;

	if (argc == 2)
	if (strcmp(argv[1], "--help") == 0)
	{
		cout << endl;
		cout << "./aes [encryption key] [input file] [key file] [out file]" << endl;
		cout << endl;		
		cout << "aes-128 | 4-4-10 | encryption key:" << endl;
		cout << "-e\t\tencryption;" << endl;
		cout << "-d\t\tdecryption;" << endl;
		cout << endl;	
		return 0;
	}
	if (argc < 5)
	{
		cout << "error: too few arguments" << endl;
		return -1;
	}
	if (strcmp(argv[1], "-e") == 0) inverse = 0;
	else if (strcmp(argv[1], "-d") == 0) inverse = 1;
	else
	{
		cout << "error: invalid arguments" << endl;
		return -1;
	}

	i_file = fopen(argv[2], "rb");
	if (i_file == NULL)
	{
		cout << "error: can't open input file" << endl;
		return 0;
	}
	k_file = fopen(argv[3], "rb");
	if (k_file == NULL)
	{
		cout << "error: can't open key file" << endl;
		return 0;
	}
	
	fseek(i_file, 0, SEEK_END);
	i_length = ftell(i_file);
	fseek(i_file, 0, SEEK_SET);

	fseek(k_file, 0, SEEK_END);
	k_length = ftell(k_file);
	fseek(k_file, 0, SEEK_SET);

	length = i_length;
	if (i_length % 16 != 0)
	length = 16 * (i_length / 16 + 1);
	
	ui_message = new unsigned char [length]; 
	uk_message = new unsigned char [k_length];

	for (int i = 0; i < i_length; i++)
	fread(&ui_message[i], sizeof(char), 1, i_file);

	for (int i = 0; i < k_length; i++)
	fread(&uk_message[i], sizeof(char), 1, k_file);
	
	fclose(i_file);
	fclose(k_file);

	if (i_length % 16 != 0)
	{
		ui_message[i_length] = 1;
		for (int i = i_length + 1; i < length; i++)
		ui_message[i] = 0;
	}

	for (int i = 0; i < Nb; i++)
	for (int j = 0; j < Nb; j++)
	word[i][j] = (int) uk_message[i + 4 * j];

	key_expansion();

	for (int k = 0; k < length / 16; k++)
	{
		for (int i = 0; i < Nb; i++)
		for (int j = 0; j < Nb; j++)
		state[i][j] = (int) ui_message[i + 4 * j + 16 * k];

		if (inverse == 0) encryption();
		if (inverse == 1) decryption();

		for (int i = 0; i < Nb; i++)
		for (int j = 0; j < Nb; j++)
		ui_message[i + 4 * j + 16 * k] = state[i][j];
	}
	
	o_file = fopen(argv[4], "wb");
	for (int i = 0; i < length; i++)
	fwrite(&ui_message[i], sizeof(char), 1, o_file);
	fclose(o_file);
	return 0;
}

int sub_bytes(int inverse)
{
	int index;
	if (inverse == 0)
	{
		for (int i = 0; i < Nb; i++)
		for (int j = 0; j < Nb; j++)
		{
			index = state[i][j];
			state[i][j] = sbox[index];
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		for (int j = 0; j < Nb; j++)
		{
			index = state[i][j];
			state[i][j] = inv_sbox[index];
		}
	}
	return 0;
}

int shift_rows(int inverse)
{
	int tmp;
	int index = Nb;
	if (inverse == 0)
	{
		while (index != 0)
		{
			for (int k = 0; k < index; k++)
			{
				tmp = state[index][0];
				for (int i = 0; i < Nb - 1; i++)
					state[index][i] = state[index][i + 1];
				state[index][Nb - 1] = tmp;
			}
			index--;
		}
	}
	else
	{
		while (index != 0)
		{
			for (int k = 0; k < index; k++)
			{
				tmp = state[index][Nb - 1];
				for (int i = Nb - 2; i >= 0; i--)
					state[index][i + 1] = state[index][i];
				state[index][0] = tmp;
			}
			index--;
		}
	}
	return 0;
}

int mix_columns(int inverse)
{
	int s0, s1, s2, s3;
	for (int i = 0; i < Nb; i++)
	{
		if (inverse == 0)
		{
			s0 = mul_by_02(state[0][i]) ^ mul_by_03(state[1][i]) ^ state[2][i] ^ state[3][i];
			s1 = state[0][i] ^ mul_by_02(state[1][i]) ^ mul_by_03(state[2][i]) ^ state[3][i];
			s2 = state[0][i] ^ state[1][i] ^ mul_by_02(state[2][i]) ^ mul_by_03(state[3][i]);
			s3 = mul_by_03(state[0][i]) ^ state[1][i] ^ state[2][i] ^ mul_by_02(state[3][i]);
		}
		else
		{
			s0 = mul_by_0e(state[0][i]) ^ mul_by_0b(state[1][i]) ^ mul_by_0d(state[2][i]) ^ mul_by_09(state[3][i]);
			s1 = mul_by_09(state[0][i]) ^ mul_by_0e(state[1][i]) ^ mul_by_0b(state[2][i]) ^ mul_by_0d(state[3][i]);
			s2 = mul_by_0d(state[0][i]) ^ mul_by_09(state[1][i]) ^ mul_by_0e(state[2][i]) ^ mul_by_0b(state[3][i]);
			s3 = mul_by_0b(state[0][i]) ^ mul_by_0d(state[1][i]) ^ mul_by_09(state[2][i]) ^ mul_by_0e(state[3][i]);
		}
		state[0][i] = s0;
		state[1][i] = s1;
		state[2][i] = s2;
		state[3][i] = s3;

	}
	return 0;
}

int key_expansion()
{
	for (int i = Nb; i < Nb*(Nr + 1); i++)
	{
		for (int j = 0; j < Nb; j++)
			word[j][i] = word[j][i - 1];

		if (i % Nk == 0)
		{
			shift_word(i);
			sub_word(i);
		}
		for (int j = 0; j < Nb; j++)
		{
			if (i % Nk == 0)
				word[j][i] ^= rcon[j][i - Nb];
			word[j][i] ^= word[j][i - Nk];
		}
	}
	return 0;
}

int shift_word(int index)
{
	int tmp = word[0][index];
	for (int i = 0; i < Nb - 1; i++)
		word[i][index] = word[i + 1][index];
	word[Nb - 1][index] = tmp;
	return 0;
}

int sub_word(int index)
{
	int count;
	for (int i = 0; i < Nb; i++)
	{
		count = word[i][index];
		word[i][index] = sbox[count];
	}
	return 0;
}

int add_rkey(int inverse, int number)
{
	for (int i = 0; i < Nb; i++)
	for (int j = 0; j < Nb; j++)
	{
		state[i][j] ^= word[i][j + number*Nk];
	}
	if (inverse == 0) return number + 1;
	else return number - 1;
}

int encryption() //кодирование
{
	int number = add_rkey(0, 0);

	for (int i = 0; i < Nr - 1; i++)
	{
		sub_bytes(0);
		shift_rows(0);
		mix_columns(0);
		number = add_rkey(0, number);
	}
	sub_bytes(0);
	shift_rows(0);
	add_rkey(0, number);

	return 0;
}

int decryption() //декодирование
{
	int number = add_rkey(1, Nr);

	for (int i = 0; i < Nr - 1; i++)
	{
		shift_rows(1);
		sub_bytes(1);
		number = add_rkey(1, number);
		mix_columns(1);
	}
	shift_rows(1);
	sub_bytes(1);
	add_rkey(1, number);

	return 0;
}
