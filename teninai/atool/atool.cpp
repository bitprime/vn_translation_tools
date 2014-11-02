#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include "lzss.h"

using namespace std;

struct AEntry
{
	char filename[0x17];
	unsigned char compressed; // ?
	size_t size;
	int offset; // relative to start of data
};

struct AFile
{
	unsigned short magic;
	int fileCount;
	vector<AEntry*> files;
};

void DecompressArchive(char*, char*);
void CompressArchive(char*, char*, int);

int main(int argc, char **argv)
{
	if(argc != 4)
	{
		cout << "usage: " << argv[0] << " [inputs]\n\t-x [file.a] [output_folder] - Extraction mode\n\t-c [input_folder] [output.a] - Creation mode (Compressed)\n\t-z [input_folder] [output.a] - Creation mode (Raw)" << endl;
		return 0;
	}

	if(argv[1][0] == '-' && argv[1][1] == 'c')
	{
		CompressArchive(argv[2], argv[3], 1);
	}
	else if(argv[1][0] == '-' && argv[1][1] == 'x')
	{
		DecompressArchive(argv[2], argv[3]);
	}
	else if(argv[1][0] == '-' && argv[1][1] == 'z')
	{
		CompressArchive(argv[2], argv[3], 0);
	}
	else
	{
		cout << "Unknown flag: " << argv[1] << endl;
	}

	return 0;
}

AFile *ReadAHeader(ifstream &infile)
{
	AFile *file = new AFile();

	infile.read((char*)&file->magic, 2);
	infile.read((char*)&file->fileCount, 2);

	for(int i = 0; i < file->fileCount; i++)
	{
		AEntry *entry = new AEntry();

		infile.read(entry->filename, sizeof(entry->filename));
		infile.read((char*)&entry->compressed, 1);
		infile.read((char*)&entry->size, 4);
		infile.read((char*)&entry->offset, 4);

		file->files.push_back(entry);

	}

	return file;
}

void DecompressArchive(char *input, char *output)
{
	ifstream infile(input, ifstream::binary);

	CreateDirectory(output, NULL);

	if(SetCurrentDirectory(output) == false)
	{
		cout << "Could not set output directory to " << output << endl;
		exit(-1);
	}

	// parse file information
	AFile *file = ReadAHeader(infile);
	int dataBegin = file->fileCount * 0x20 + 4;

	// write file listing
	ofstream log("file.lst", ofstream::binary);
	log.write((char*)&file->fileCount, 4);

	for(int i = 0; i < file->fileCount; i++)
	{
		log.write(file->files[i]->filename, 0x17);
	}
	log.close();

	// extract files
	for(int i = 0; i < file->fileCount; i++)
	{
		int absOffset = dataBegin + file->files[i]->offset;
		size_t dataLen = file->files[i]->size;
		char *data = new char[file->files[i]->size];

		// read file data
		infile.seekg(absOffset, ios_base::beg);
		infile.read((char*)data, file->files[i]->size);

		if(file->files[i]->compressed == 1 && file->files[i]->size > 0)
		{
			size_t decompLen = *(int*)data;
			char *decompData = new char[decompLen];

			LZDecode(data + 4, dataLen - 4, decompData, &decompLen);

			delete data;
			data = decompData;
			dataLen = decompLen;
		}
		
		// write file data
		ofstream outfile(file->files[i]->filename, ofstream::binary);
		outfile.write(data, dataLen);
		outfile.close();

		// write file info
		cout << "[" << setw(4) << setfill('0') << i << "] ";
		cout << setw(0x17) << setfill(' ') << left << file->files[i]->filename << " " << right;
		cout << "offset[" << setw(8) << setfill('0') << hex << dataBegin + file->files[i]->offset << "] ";
		cout << "size[" << setw(8) << setfill('0') << hex << file->files[i]->size << "]" << endl;

		// clean up per-entry data
		delete data;
		delete file->files[i];
	}

	infile.close();

	delete file;
}

void CompressArchive(char *input, char *output, int compressionFlag)
{
	ofstream outfile(output, ofstream::binary);

	if(!outfile.is_open())
	{
		cout << "Could not open " << output << " for writing" << endl;
		exit(-2);
	}

	AFile *file = new AFile();

	file->magic = 0xaf1e;
	file->fileCount = 0;

	// parse file listing. the game expects the scripts to be in a certain order so we must repack it in that order
	string listPath = string(input) + "\\file.lst";
	ifstream listfile(listPath, ifstream::binary);

	listfile.read((char*)&file->fileCount, 4);

	for(int i = 0; i < file->fileCount; i++)
	{
		AEntry *entry = new AEntry();
		
		entry->offset = 0;
		entry->size = 0;
		entry->compressed = 0;
		listfile.read(entry->filename, 0x17);

		file->files.push_back(entry);
	}
	listfile.close();

	// write blank header
	outfile.write((char*)&file->magic, 2);
	outfile.write((char*)&file->fileCount, 2);

	for(int i = 0; i < file->fileCount; i++)
	{
		int b = 0;
		for(int x = 0; x < 0x20 / 4; x++)
			outfile.write((char*)&b,4);
	}

	// write compressed file data
	int baseOffset = 0, dataBegin = file->fileCount * 0x20 + 4;
	for(int i = 0; i < file->fileCount; i++)
	{
		string path = string(input) + "\\" + file->files[i]->filename;

		ifstream infile(path, ifstream::binary);

		infile.seekg(0, ios_base::end);
		size_t origSize = infile.tellg();
		infile.seekg(0, ios_base::beg);

		char *inputData = new char[origSize];
		infile.read(inputData, origSize);

		file->files[i]->compressed = compressionFlag;
		file->files[i]->offset = baseOffset;

		if(file->files[i]->compressed == 1)
		{
			size_t compSize = 0;
			char *compData = LZEncode(inputData, origSize, &compSize);
			file->files[i]->size = compSize + 4;
			outfile.write((char*)&origSize, 4);
			outfile.write(compData, compSize);
			delete compData;
		}
		else
		{
			file->files[i]->size = origSize;
			outfile.write(inputData, origSize);
		}

		baseOffset += file->files[i]->size;

		cout << "[" << setw(4) << setfill('0') << i << "] ";
		cout << setw(0x17) << setfill(' ') << left << file->files[i]->filename << " " << right;
		cout << "offset[" << setw(8) << setfill('0') << hex << dataBegin + file->files[i]->offset << "] ";
		cout << "size[" << setw(8) << setfill('0') << hex << file->files[i]->size << "]" << endl;

		infile.close();

		delete inputData;
	}

	outfile.seekp(4, ios_base::beg);

	for(int i = 0; i < file->fileCount; i++)
	{
		outfile.write(file->files[i]->filename, 0x17);
		outfile.write((char*)&file->files[i]->compressed, 0x01);
		outfile.write((char*)&file->files[i]->size, 0x04);
		outfile.write((char*)&file->files[i]->offset, 0x04);
	}

	delete file;
}