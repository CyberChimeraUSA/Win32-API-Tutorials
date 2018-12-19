// PE_ParserVideo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <time.h>
#include <sstream>      // std::ostringstream
#include <fstream>      // std::fstream

#define DOS_Header "MZ"
#define DOS_Header_Size 2
using namespace std;


int  pe_file_info_dbg(void);
bool pe_file_sanity(char *buf);
bool pe_file_parser(char *buf, int size);
bool pe_map_values(char  *buf, int size);
bool pe_section_info(char *buf, int size, unsigned int sections);


static char *buffer_top = 0;

struct section_info
{
	int virt_size;
	int virt_address;
	int size_raw_buffer;
	int fp_raw_data;
	int fp_reloc_table;
	int flags;
};

struct text_info
{
	int virt_size;
	int virt_address;
	int size_raw_buffer;
	int fp_raw_data;
	int fp_reloc_table;
	int flags;
};


int main()
{
	pe_file_info_dbg();
}

int pe_file_info_dbg(void)
{
	printf("\nPE File Information dbg\n");
	string input_path;
	ostringstream stream;
	bool result;

	char *buffer = nullptr;
	cout << "Enter Path to Input File to Fuzz (max 256 characters, file will be malloc'd)" << endl;
	cin >> input_path;

	ifstream fs(input_path, ios::in | ios::binary | ios::ate);

	if (fs.is_open())
	{
		std::cout << "Operation successfully performed\n";
		unsigned long start, end;
		unsigned long int size = 0;
		char *buffer = nullptr;
		char * memblock;

		start = fs.tellg();
		buffer = (char *)malloc(sizeof(char)*start);

		memblock = new char[start];
		fs.seekg(0, ios::beg);
		fs.read(buffer, start);
		fs.close();

		buffer_top = buffer;
		result = pe_file_sanity(buffer);
		if (result == false)
		{
			printf("Not Proper PE \n");
			return 1;
		}
		result = pe_file_parser(buffer, start);
		if (result == false)
		{
			printf("Parsing PE fail  \n");
			return 1;
		}
		cout << start << endl;
	}
	else
	{
		std::cout << "Error opening file";
	}
	fs.close();
	free(buffer);


	return 0;
}

bool pe_map_values(char * buf, int size)
{
	char *file_ptr = buf;
	char holder_4[4] = { 0 };
	char holder_2[2] = { 0 };
	unsigned short holder_short[1] = { 0 };
	unsigned int   holder_int[1] = { 0 };
	unsigned int  total_sections = 0;


	file_ptr = file_ptr - 1;
	cout << "***********************" << endl;
	cout << "******COFF HEADER******" << endl;
	cout << "***********************" << endl;

	memcpy_s(holder_int, 4, file_ptr, 4);
	printf("Signature Value 0x%x \n", *holder_int);

	file_ptr = file_ptr + 4;
	memcpy_s(holder_short, 2, file_ptr, 2);
	printf("Machine Type 0x%x \n", *holder_short);


	file_ptr = file_ptr + 2;
	memcpy_s(holder_short, 2, file_ptr, 2);
	total_sections = *holder_short;
	printf("Sections 0x%x \n", *holder_short);


	file_ptr = file_ptr + 2;
	memcpy_s(holder_int, 4, file_ptr, 4);
	printf("Timestamp 0x%x \n", *holder_int);

	file_ptr = file_ptr + 4;
	memcpy_s(holder_int, 4, file_ptr, 4);
	printf("Pointer To SymbolTable (depreciated) 0x%x \n", *holder_int);


	file_ptr = file_ptr + 4;
	memcpy_s(holder_int, 4, file_ptr, 4);
	printf(" Number of Symbols (depreciated) 0x%x \n", *holder_int);


	file_ptr = file_ptr + 4;
	memcpy_s(holder_short, 2, file_ptr, 2);
	printf(" Size of Optional Header 0x%x \n", *holder_short);


	file_ptr = file_ptr + 2;
	memcpy_s(holder_short, 2, file_ptr, 2);
	printf(" Characteristics 0x%x \n", *holder_short);

	printf("\n");
	cout << "***************************" << endl;
	cout << "****Standad COFF Fields****" << endl;
	cout << "***************************" << endl;

	file_ptr = file_ptr + 2;
	memcpy_s(holder_short, 2, file_ptr, 2);
	printf(" Magic Value 0x%x \n", *holder_short);


	*holder_short = { 0 };
	file_ptr = file_ptr + 2;
	memcpy_s(holder_short, 1, file_ptr, 1);
	printf(" Major Linker 0x%x \n", *holder_short);

	file_ptr = file_ptr + 1;
	memcpy_s(holder_short, 1, file_ptr, 1);
	printf(" Minor Linker 0x%x \n", *holder_short);


	file_ptr = file_ptr + 1;
	memcpy_s(holder_int, 4, file_ptr, 4);
	printf(" Size of Code 0x%x \n", *holder_int);


	file_ptr = file_ptr + 4;
	memcpy_s(holder_int, 4, file_ptr, 4);
	printf(" Size of Initalized Data 0x%x \n", *holder_int);


	file_ptr = file_ptr + 4;
	memcpy_s(holder_int, 4, file_ptr, 4);
	printf(" Size of Unitalized Data 0x%x \n", *holder_int);

	file_ptr = file_ptr + 4;
	memcpy_s(holder_int, 4, file_ptr, 4);
	printf(" Address of Entry Point 0x%x \n", *holder_int);


	file_ptr = file_ptr + 4;
	memcpy_s(holder_int, 4, file_ptr, 4);
	printf(" Base of Code 0x%x \n", *holder_int);


	file_ptr = file_ptr + 4;
	memcpy_s(holder_int, 4, file_ptr, 4);
	printf(" Base of Data 0x%x \n", *holder_int);



	printf("\n");
	cout << "********************************" << endl;
	cout << "****Windows Specific Fields*****" << endl;
	cout << "********************************" << endl;


	printf("\n");
	cout << "********************************" << endl;
	cout << "******Section Information*******" << endl;
	cout << "********************************" << endl;

	pe_section_info(file_ptr, size, total_sections);

	return true;

}

bool pe_section_info(char *buf, int size, unsigned int sections)
{
	printf("Size of file %d \n", size);
	printf("Buffer Top %x \n", buffer_top);
	int offset = 0;
	int ctr = 0;
	int section_num = 0;
	int buffer[4] = { 0 };
	char sec_name[25] = { 0 };
	struct section_info sec_info[100];  // TODO- ideally want this to be number of sections argument, needs to be const
	struct text_info text_info;
	buf = buf - 1;

	while (section_num != sections)
	{

		if (*buf == 0x2E)
		{
			printf("%s \n", buf);
			section_num++;

			if (strcmp(buf, ".text") == 0)
			{
				printf(".text section found \n");
				strncpy(sec_name, buf, sizeof(sec_name));
			}


			buf = buf + 8;
			memcpy_s(buffer, 4, buf, 4);
			sec_info[ctr].virt_size = *buffer;
			printf("\tVirtual Size : 0x%x \n", sec_info[ctr].virt_size);
			if (strcmp(sec_name, ".text") == 0)
				text_info.virt_size = *buffer;

			buf = buf + 4;
			memcpy_s(buffer, 4, buf, 4);
			sec_info[ctr].virt_address = *buffer;
			printf("\tVirtual Address : 0x%x \n", sec_info[ctr].virt_address);
			if (strcmp(sec_name, ".text") == 0)
				text_info.virt_address = *buffer;

			buf = buf + 4;
			memcpy_s(buffer, 4, buf, 4);
			sec_info[ctr].size_raw_buffer = *buffer;
			printf("\tSize of Raw Data : 0x%x \n", sec_info[ctr].size_raw_buffer);
			if (strcmp(sec_name, ".text") == 0)
				text_info.size_raw_buffer = *buffer;

			buf = buf + 4;
			memcpy_s(buffer, 4, buf, 4);
			sec_info[ctr].fp_raw_data = *buffer;
			printf("\tFile Pointer to Raw Data : 0x%x \n", sec_info[ctr].fp_raw_data);
			if (strcmp(sec_name, ".text") == 0)
				text_info.fp_raw_data = *buffer;

			buf = buf + 4;
			memcpy_s(buffer, 4, buf, 4);
			sec_info[ctr].fp_reloc_table = *buffer;
			printf("\tFile Pointer to Relocation Table : 0x%x \n", sec_info[ctr].fp_reloc_table);
			if (strcmp(sec_name, ".text") == 0)
				text_info.fp_reloc_table = *buffer;

			buf = buf + 12;
			memcpy_s(buffer, 4, buf, 4);
			sec_info[ctr].flags = *buffer;
			printf("\tflags : 0x%x \n", sec_info[ctr].flags);
			if (strcmp(sec_name, ".text") == 0)
				text_info.flags = *buffer;


			ctr++;
			buf = buf + 4;

			while (section_num != sections)
			{

				printf("%s \n", buf);
				section_num++;

				buf = buf + 8;
				memcpy_s(buffer, 4, buf, 4);
				sec_info[ctr].virt_size = *buffer;
				printf("\tVirtual Size : 0x%x \n", sec_info[ctr].virt_size);

				buf = buf + 4;
				memcpy_s(buffer, 4, buf, 4);
				sec_info[ctr].virt_address = *buffer;
				printf("\tVirtual Address : 0x%x \n", sec_info[ctr].virt_address);

				buf = buf + 4;
				memcpy_s(buffer, 4, buf, 4);
				sec_info[ctr].size_raw_buffer = *buffer;
				printf("\tSize of Raw Data : 0x%x \n", sec_info[ctr].size_raw_buffer);

				buf = buf + 4;
				memcpy_s(buffer, 4, buf, 4);
				sec_info[ctr].fp_raw_data = *buffer;
				printf("\tFile Pointer to Raw Data : 0x%x \n", sec_info[ctr].fp_raw_data);

				buf = buf + 4;
				memcpy_s(buffer, 4, buf, 4);
				sec_info[ctr].fp_reloc_table = *buffer;
				printf("\tFile Pointer to Relocation Table : 0x%x \n", sec_info[ctr].fp_reloc_table);

				buf = buf + 12;
				memcpy_s(buffer, 4, buf, 4);
				sec_info[ctr].flags = *buffer;
				printf("\tflags : 0x%x \n", sec_info[ctr].flags);

				ctr++;
				buf = buf + 4;

			}
		}
		buf++;
	}
	return true;
}

bool pe_file_parser(char *buf, int size)
{
	int sig_matched_0 = 0x50;
	int sig_matched_1 = 0x45;

	for (int i = 0; i < size; i++)
	{
		if (*buf == sig_matched_0)
		{
			buf++;
			if (*buf == sig_matched_1)
			{

				pe_map_values(buf, size);
				return true;
			}
		}
		else
		{
			buf++;
		}
	}



	return false;
}

bool pe_file_sanity(char *buf)
{
	int sig_match_0 = 0x4D;
	int sig_match_1 = 0x5A;
	bool result = false;

	if (buf[0] == sig_match_0)
	{
		if (buf[1] == sig_match_1)
		{
			return true;
		}
	}

	return false;
}