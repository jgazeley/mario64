#include <stdio.h>
#include <stdlib.h>

typedef unsigned char BYTE;

BYTE* readBytes(FILE* eep, long offset, int num);
int countSetBits(BYTE byte);
int getChecksum16(FILE** in);

int main(int argc, char* argv[])
{
	if(argc != 2)								// validate user input
	{
		fprintf(stderr, "Usage: %s path_to_savefile\n", argv[0]);
		exit(1);
	}
	
	FILE* eep = fopen(argv[1], "rb");			// open *.eep file
	if(!eep)
	{
		char errmsg[500];
		sprintf(errmsg, "Cannot open %s for reading", argv[1]);
		perror(errmsg);
		exit(1);
	}

	fseek(eep, 0, SEEK_END);
	size_t fsize = ftell(eep);					// get file size
	if(fsize != 512)
	{
		printf("Invalid file size!");
		exit(1);
	}
	rewind(eep);

	char* levels[] = { "Bomb-omb Battlefield", "Whomps Fortress", "Jolly Roger Bay", 
          "Cool Cool Mountain", "Big Boo's Haunt", "Hazy Maze Cave", 
          "Lethal Lava Land", "Shifting Sand Land", "Dire Dire Docks", 
          "Snowman's Land", "Wet Dry World", "Tall Tall Mountain", 
          "Tiny Huge Island", "Tick Tock Clock", "Rainbow Ridge", 
          "Secret 1", "Secret 2" };

	int starCount = 0;							// total stars
	BYTE* coins = readBytes(eep, 0x25, 15);		// coin values per level in hex
	BYTE* stars = readBytes(eep, 0xc, 15);		// stars acquired per level

	for(int i = 0; i < 15; i++)					// display level coin and star counts
	{
		int curStars = countSetBits(stars[i]);
		printf("%-25s Coins: %03d\tStars: %d\n",levels[i], coins[i], curStars);
		starCount += curStars;
	}
	printf("Total Stars: %d\n", starCount);		// display total star count
	printf("Checksum: 0x%04X\n", 
			getChecksum16(&eep));				// display the checksum

	printf("Press ENTER when done...");		// program runs until user hits enter
	char exit[30];
	fgets(exit, 30, stdin);

	free(coins);
	free(stars);								
	fclose(eep);								// close *.eep file
}

// returns a string of bytes from a file starting from a given offset
BYTE* readBytes(FILE* eep, long offset, int count)
{
	BYTE* bytes = (BYTE*) malloc(count * sizeof(BYTE));
	fseek(eep, offset, SEEK_SET);
	fread(bytes, count, 1, eep);
	rewind(eep);
	return bytes;
}

// counts the number of '1' bits in an 8-bit unsigned integer
int countSetBits(BYTE b)
{
	int count = 0;

	for(int i = 0; i < 7; i++) // ignore the MSB
		if(((b >> i) & 1) == 1)
			count++;
		
	return count;
}

// calculates the 16-bit checksum for the save file
int getChecksum16(FILE** in)
{
	BYTE buffer[0x36];
	fread(buffer, 0x36, 1, *in);

	int sum = 0x0;
	for(int i = 0; i <= 0x35; i++)
	{
		sum += buffer[i];
	}
	return sum;
}