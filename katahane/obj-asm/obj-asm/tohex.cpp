#include "tohex.h"

unsigned int GetInt(string input)
{
	return ToHex(input);
}

unsigned short GetShort(string input)
{
	return ToHex(input);
}

unsigned char GetByte(string input)
{
	return ToHex(input);
}

unsigned int ToHex(string str)
{
	unsigned int output = 0;

	for(int i=0; i < str.length(); i++)
	{
		char hex[6] = { 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
		int shift = 4 * (str.length() - i - 1);

		if(str[i] > 0x60)
		{
			str[i] -= 0x20;
		}

		if(str[i] > 0x40)
		{
			str[i] -= 0x41;
			output |= hex[str[i]] << shift;
		}
		else
		{
			str[i] -= 0x30;
			output |= str[i] << shift;
		}
	}

	return output;
}
