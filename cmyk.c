/**
 * Calculating quantity of each color of CMYK in bmp
 * ./cmyk name.bmp
 * build 0.0010
 * 
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"
#include "lcms2.h"


int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./cmyk infile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[1];
	/*char *outfile ="cmykout.txt";*/
    

       
    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }


    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4; 
    


   
    //littleCMS start
    cmsHPROFILE hInProfile, hOutProfile; //input and output profiles
    cmsHTRANSFORM hTransform; 
    
    //hInProfile = cmsOpenProfileFromFile("/home/ab/Documents/cmyk/colorProfiles/sRGBColorSpaceProfile.icm", "r");
	hInProfile = cmsCreate_sRGBProfile();
	if (hInProfile==NULL){
		printf ("Cannot open in profile \n");
		return 5;
	}


	hOutProfile = cmsOpenProfileFromFile("/home/ab/Documents/cmyk/colorProfiles/WebCoatedSWOP2006Grade5.icc", "r");
    
	if (hOutProfile==NULL){
		printf ("Cannot open out profile \n");
		return 6;
	}
	
    hTransform = cmsCreateTransform(hInProfile, TYPE_BGR_8, hOutProfile, TYPE_CMYK_8, INTENT_PERCEPTUAL, 0);
	//TYPE_BGR_8 - type of windows colors, windows does store images as B,G,R and not as R,G,B.
	// TYPE_CMYK_8 = type of output
    
	if (hTransform==NULL){
		printf ("Transform was not made \n");
		return 7;
	}
    cmsCloseProfile(hInProfile);
    cmsCloseProfile(hOutProfile);

 	int pixels = bi.biHeight*bi.biWidth;

	int bytesinscanline = 3*bi.biWidth;

	unsigned char scanline[3*pixels], outputline[4*pixels];
	
	// iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++){

		fread(&scanline[i*bytesinscanline], bytesinscanline, 1, inptr);

	// skip over padding, if any 
		fseek(inptr, padding, SEEK_CUR);
    }
    

 	cmsDoTransform(hTransform, scanline, outputline, pixels); //transforming from one to other
	cmsDeleteTransform(hTransform);
	
	double pixelstotal = (double)(pixels);
	
	int cmykCtotal=0, cmykMtotal=0,cmykYtotal=0, cmykKtotal=0;

	for (int f = 0, g=0; f<pixels; f++, g=g+4){
		cmykCtotal = cmykCtotal + outputline [g]; 
		cmykMtotal = cmykMtotal + outputline [g+1];
		cmykYtotal = cmykYtotal + outputline [g+2];
		cmykKtotal = cmykKtotal + outputline [g+3];
	}
	
	printf(" \n TOTAL \n Cyan %f\n Mageta %f\n Yellow %f\n Black %f\n ", 
		   							(double) cmykCtotal*100/(pixelstotal*255), 
		   							(double)cmykMtotal*100/(pixelstotal*255), 
		   							(double)cmykYtotal*100/(pixelstotal*255), 
		   							(double) cmykKtotal*100/(pixelstotal*255)); //C M Y K
	

	
	printf ("\n pixels total %.0f\n", pixelstotal);
    return 0;
}

/**
results             

 ab@ab-VirtualBox:~/Documents/cmyk/005$ ./cmyk Xerox2.bmp
 
 TOTAL 
 Cyan 41.959450
 Mageta 48.977966
 Yellow 18.761509
 Black 8.844136
 
 pixels total 343730

 
 
 
 
*/