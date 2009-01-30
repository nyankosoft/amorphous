
#include "LWO2_Common.hpp"


UINT4 ReadVLIndex(FILE* fp, int *pIndexSize)
{
	UINT2 wRead, wRead2;
	UINT4 uiRead;
	wRead = ReadBE2BytesIntoLE(fp);
	if(wRead < 0xFF00)  //then, the index is written as an unsigned 2-byte integer
	{
		*pIndexSize = 2;
		return (UINT4)wRead;
	}
	else  //the index is an unsigned 4-byte integer with bits 24-31 set
	{
		*pIndexSize = 4;
		uiRead = (UINT4)wRead;  //As this will be the higher 2 bytes,
		uiRead = uiRead << 0xFFFF;  //let's shift these 2 bytes to the left by 2 bytes
		wRead2 = ReadBE2BytesIntoLE(fp);  //this will be the lower 2 bytes
		uiRead = uiRead | (UINT4)wRead2;
		uiRead = uiRead & 0x00FFFFFF;  //Mask out the first byte
		return uiRead;
	}
}


int ReadName(char* pName, FILE* fp)
{
	int i = 0;
	while(1)
	{
		fread(pName + i, sizeof(char), 1, fp);
		if(pName [i] == '\0') //reached the end of this string
		{
			if(i % 2 == 0)
			{
				// the string is of an odd length (including the terminating NULL),
				// and there is an extra null-byte follwing this.
				fread(pName + i, sizeof(char), 1, fp); //has no specific purposes.
				i++;
			}
			
			return i+1;	 //Returns how many bytes are read from the file
		}
		
		i++;
	}
}


void AdvanceFP(FILE *fp, UINT4 count)
{
	char p;
	UINT4 i, j;
	for(i=0; i<count; i++)
	{
		j = fread(&p, 1, 1, fp);
		if(j != 1)
			return;
	}
}

