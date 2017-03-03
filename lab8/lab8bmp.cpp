#include <iostream>
#include <fstream>

using namespace std;

//A bitmap file is laid out like:
//BMPFileHeader
//BMPInfoHeader
//Pixel array

struct BMPFileHeader {
	unsigned short type;
	unsigned int size;
	unsigned short reserved1, reserved2;
	unsigned int offset;
} __attribute__((packed));

struct BMPInfoHeader {
	unsigned int size;
	int width;
	int height;
	unsigned short planes;
	unsigned short bitcount;
	unsigned int compression;
	unsigned int imagesize;
	int x_pixels_per_meter;
	int y_pixels_per_meter;
	unsigned int color_used;
	unsigned int color_important;
} __attribute__((packed));


struct Pixel {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
} __attribute__((packed));


int main(int argc, char *argv[])
{
	if (argc < 3) {
		cout << "Usage: " << argv[0] << " <bmp file> <output bmp file>" << endl;
		return -1;
	}


	/*********************************************************************************
         * Write your code below
         *********************************************************************************/
	
	// ***** CLASS INSTANCES ********************

	BMPFileHeader BFH;
	BMPInfoHeader BIH;
	
	// ***** VARIABLES **************************
	Pixel *p, tmp;

	// ***** INPUT FILE *************************

	// Initialize ifstream
	ifstream fin;

	// Open input file
	fin.open(argv[1],ios::binary);

	// Read headers
	fin.read(reinterpret_cast<char*>(&BFH),sizeof(BFH));
	fin.read(reinterpret_cast<char*>(&BIH),sizeof(BIH));

	// Array containing all pixels
	// width = BIH.width
	// height = BIH.height
	int imgSize = BIH.width * BIH.height;
	p = new Pixel[imgSize];

	// Read all pixels
	fin.read(reinterpret_cast<char*>(p),(sizeof(Pixel) * imgSize));

	// Close input file
	fin.close();


	// ***** INVERT IMAGE  **********************

	// Loop through pixels
	for(int i=0;i<(BIH.width*BIH.height);i++){

		// Set R,G,B values to inverse value
		p[i].red = 255-p[i].red;
		p[i].green = 255-p[i].green;
		p[i].blue = 255-p[i].blue;
	
	}

	// ***** FLIP IMAGE *************************
	
	// Loop through rows
	for(int i=0;i<BIH.height;i++){

		// Loop through columns
		for(int j=0;j<BIH.width/2;j++){

			// Set temporary pixel to current 
			// location pixel data
			tmp = p[i*BIH.width+j];

			// Set current location pixel to
			// corresponding Y axis pixel data
			p[i*BIH.width+j] = p[i*BIH.width+BIH.width - 1 -j];

			// Set corresponding Y axis pixel to
			// temporary pixel data
			p[i*BIH.width+BIH.width - 1 -j] = tmp;
		}
	}

	// ***** OUTPUT FILE ************************
	
	// Initialize ofstream
	ofstream fout;

	// Open output file
	fout.open(argv[2],ios::binary);

	// Write headers
	fout.write(reinterpret_cast<char*>(&BFH),sizeof(BFH));
	fout.write(reinterpret_cast<char*>(&BIH),sizeof(BIH));

	// Write all pixels
	fout.write(reinterpret_cast<char*>(p),(sizeof(Pixel) * imgSize));

	// Close output file
	fout.close();

	return 0;
}


