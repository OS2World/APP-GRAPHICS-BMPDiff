/*****************************************************************************/
/*                                                                           */
/*  Programmname  : BMPDiff.c                                                */
/*  Verwendung    : Calculates the difference between 2 BMP's                */
/*  Bibliotheken  :                                                          */
/*  Autor         : JÅrgen Dittmer                                           */
/*  Speichermodell: FLAT                                                     */
/*  Compiler      : WATCOM C/C++ v. 10.0                                     */
/*  System        : OS/2 4.0                                                 */
/*  Erstellung    : 19 Sep 1999                                              */
/*  énderung      : 23 Jul 2002 - Remove all OS/2 MM-APIs                    */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmpdiff.h"


/****************************************************************************/
/*                                                                          */
/*  FUNCTION   : main(int argc, char* const argv[])                         */
/*                                                                          */
/****************************************************************************/

int main(int argc, char* argv[])
{
BMPHEADER BMPHdr1, BMPHdr2;
char *pImage1, *pImage2; 
int  i;
int  rc;
char achBMP1[128] = ""; /* Filename Source BMP 1 */
char achBMP2[128] = ""; /* Filename Source BMP 2 */
char achDiff[128] = ""; /* Filename Result BMP   */
int  iWid = 0;          /* Blurr Width           */
int  SaveDiff = 0;      /* Save Diff BMP         */
FILE *fp;


	if (argc>1) 
		for (i=0; i<argc; i++)
	    {
			if ((argv[i][0]=='-') || (argv[i][0] == '/'))
	        {
		        switch (argv[i][1])
				{
				case '1': 
					strcpy(achBMP1,&(argv[i][3]));
					if (!strrchr(achBMP1,'.')) strcat(achBMP1,".BMP");
					break;
				case '2':
					strcpy(achBMP2,&(argv[i][3]));
					if (!strrchr(achBMP2,'.')) strcat(achBMP2,".BMP");
					break;
				case 'd':
					strcpy(achDiff,&(argv[i][3]));
					if (!strrchr(achDiff,'.')) strcat(achDiff,".BMP");
					SaveDiff = 1;   
					break;
				case 'b':
					iWid  = atoi(&(argv[i][3]));
					break;
				}
			}
	    }
	else 
	{
		printf("BMPDiff v2.0, J. Dittmer 23 Jul 2002, adapted from ICOMPARE c't 19/1999\n"
			   "Useage: BMPDiff -1:<Image1> -2:<Image2> [-d:<DiffImage>] [-b:<Blurrwidth>]\n\n"
			   "  <Image1> and <Image2> are the filenames of the images to compare\n"			   
			   "  <DiffImage> is the filename of the result image\n"
			   "  <Blurrwidth> width of the blurr filter.\n"
			   "  - Blurrwidth has to be odd and between 3 and 15.\n"
			   "  - Blurrwidth and DiffImage are optional parameters.\n");
	    return (0);
	}



    memset(&BMPHdr1, '\0', sizeof(BMPHEADER));
    fp = fopen(achBMP1, "rb");
    if (fp == NULL) {
        printf("Can not open File!\n");
        return (-100);
    } /* endif */
    fread(&BMPHdr1, sizeof(BMPHEADER), 1, fp);
    fclose(fp);

    memset(&BMPHdr2, '\0', sizeof(BMPHEADER));
    fp = fopen(achBMP2, "rb");
    if (fp == NULL) {
        printf("Can not open File!\n");
        return (-100);
    } /* endif */
    fread(&BMPHdr2, sizeof(BMPHEADER), 1, fp);
    fclose(fp);

    printf("%s, %s: %d*%d Pixel\n", achBMP1, achBMP2, BMPHdr2.width, BMPHdr2.height);

    if ((BMPHdr1.type != 'MB') || (BMPHdr2.type != 'MB')) {
        printf("No Bitmap format!\n");
        return (-100);
    } /* endif */

    if ((BMPHdr1.width != BMPHdr2.width) || (BMPHdr1.height != BMPHdr2.height)) {
        printf("Both Images must have same format!\n");
        return (-200);
    } /* endif */

    if ((BMPHdr1.bits != 24) || (BMPHdr2.bits != 24)) {
        printf("Images must have 24 BitsPerPixel!\n");
        return (-300);
    } /* endif */

    if (BMPHdr1.compression || BMPHdr2.compression) {
        printf("Compressed Bitmaps not supported!\n");
        return (-400);
    } /* endif */

    /* Read 1st Bitmap Data */
    pImage1 = malloc(BMPHdr1.width * BMPHdr1.height * 3);
    if (pImage1 == NULL) {
        printf("Malloc error!\n");
        return (-500);
    } /* endif */
    fp = fopen(achBMP1, "rb");
    fseek(fp, BMPHdr1.offset, SEEK_SET);
    rc = fread(pImage1, BMPHdr1.width * BMPHdr1.height * 3, 1, fp);
    fclose(fp);
    if (rc < 1) {
        printf("Read error!\n");
        return (-600);
    } /* endif */

    /* Read 2nd Bitmap data */
    pImage2 = malloc(BMPHdr2.width * BMPHdr2.height * 3);
    if (pImage2 == NULL) {
        printf("Malloc error!\n");
        return (-500);
    } /* endif */
    fp = fopen(achBMP2, "rb");
    fseek(fp, BMPHdr2.offset, SEEK_SET);
    rc = fread(pImage2, BMPHdr2.width * BMPHdr2.height * 3, 1, fp);
    fclose(fp);
    if (rc < 1) {
        printf("Read error!\n");
        return (-600);
    } /* endif */

    if (iWid) {
       printf("Blurring...\n");
	   rc = ApplyBlurr(pImage1, BMPHdr1.width, BMPHdr1.height, iWid);
	   if (rc < 0) return (-100 + rc);
	   rc = ApplyBlurr(pImage2, BMPHdr2.width, BMPHdr2.height, iWid);
	   if (rc < 0) return (-200 + rc);
    }

	rc = CalcDifference (pImage1, pImage2, BMPHdr1.width, BMPHdr1.height);

    printf("Difference=%d\n", rc);

    if (SaveDiff){
        printf("Writing Diff Image: %s\n", achDiff);
        fp = fopen(achDiff, "wb");
        fwrite(&BMPHdr2, sizeof(BMPHEADER), 1, fp);
        fwrite(pImage2, BMPHdr2.width * BMPHdr2.height * 3, 1, fp);
        fclose(fp);
    }

    free(pImage1);
    free(pImage2);

    if (rc < 0) printf("Error, RC=%d\n", rc);
    return (rc);
}


/******************************************************************************/
/*                                                                            */
/*  FUNCTION   : ApplyBlurr(PBYTE pImage, ULONG cx, ULONG cy, INT iBwid)      */
/*                                                                            */
/*  PURPOSE    : Given a Pointer to a DIB structure it will apply a blurr     */
/*               Filter to the DIB. NOTE : Only 24Bit DIBS are supported.     */
/*                                                                            */
/*  RETURNS    : The maximum brightness in the resulting DIB                  */
/*               Negative Results mark errors.								  */
/*                                                                            */
/******************************************************************************/

int ApplyBlurr (char *pImage, unsigned int cx, unsigned int cy, int iBwid)
{
int  dwX, dwY;
int  iNoIdx;
int  i,j;
int  iHalfBwid;
long rgbwtAve[3];
int  maxVal = 0;
int  tVal;

    if (!(iBwid & 1)) iBwid++;  /* Muss ungerade sein */       
	if (iBwid > 15) iBwid = 15; /* Max 15 */
    iNoIdx = iBwid * iBwid;
    iHalfBwid = iBwid / 2;

    for (dwY = 0; dwY < cy; dwY++){
        for (dwX = 0; dwX < cx; dwX++){
            rgbwtAve[0] = 0;
		    rgbwtAve[1] = 0;
			rgbwtAve[2] = 0;

 	 		for (i = 0; i < iBwid; i++){
                for (j = 0; j < iBwid; j++){
				    if ((dwX+i-iHalfBwid < 0) || (dwX+i-iHalfBwid >= cx) ||
					    (dwY+j-iHalfBwid < 0) || (dwY+j-iHalfBwid >= cy)){
							rgbwtAve[0] += 127L;
							rgbwtAve[1] += 127L;
							rgbwtAve[2] += 127L;
					}else{
							rgbwtAve[0] += *(pImage + 3 * (dwX+i-iHalfBwid + cx*(dwY+j-iHalfBwid)));
							rgbwtAve[1] += *(pImage + 3 * (dwX+i-iHalfBwid + cx*(dwY+j-iHalfBwid)) + 1);
							rgbwtAve[2] += *(pImage + 3 * (dwX+i-iHalfBwid + cx*(dwY+j-iHalfBwid)) + 2);
					}
				}
				*(pImage + 3 * (dwX + cx * dwY))     = (rgbwtAve[0] / iNoIdx ) % 256;
				*(pImage + 3 * (dwX + cx * dwY) + 1) = (rgbwtAve[1] / iNoIdx ) % 256;
				*(pImage + 3 * (dwX + cx * dwY) + 2) = (rgbwtAve[2] / iNoIdx ) % 256;
				tVal = *(pImage + 3 * (dwX + cx * dwY)) +
                       *(pImage + 3 * (dwX + cx * dwY) + 1) +
                       *(pImage + 3 * (dwX + cx * dwY) + 2);
				if (tVal > maxVal) maxVal = tVal;
            }
        }
    }
    return maxVal;
}


int CalcDifference (char *pImage1, char *pImage2, unsigned int cx, unsigned int cy)
{
unsigned long isize = cx * cy * 3;  /* Image size in bytes */
unsigned long i;
int   maxDiff = 0;
int   minDiff = 1000;
int   tDiff;
int   actDiff;

    for (i = 0; i < isize; i+=3) {
        actDiff=0;
        tDiff = *(pImage1 + i) - *(pImage2 + i);
        if (tDiff < 0) actDiff -= tDiff; else actDiff += tDiff;
        tDiff = *(pImage1 + i + 1) - *(pImage2 + i + 1);
        if (tDiff < 0) actDiff -= tDiff; else actDiff += tDiff;
        tDiff = *(pImage1 + i + 2) - *(pImage2 + i + 2);
        if (tDiff < 0) actDiff -= tDiff; else actDiff += tDiff;

        if (actDiff < minDiff) minDiff = actDiff;
        if (actDiff > maxDiff) maxDiff = actDiff;

        *(pImage2 + i)     = actDiff/3;
        *(pImage2 + i + 1) = actDiff/3;
        *(pImage2 + i + 2) = actDiff/3;
    } /* endfor */
    return (maxDiff-minDiff);
}

