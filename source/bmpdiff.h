/****************************/
/* Prototypes for BMPDIFF.C */
/****************************/

typedef struct {
   unsigned short int type;         /* Magic identifier BM       */
   unsigned int fileheadersize;     /* File header size in bytes */
   unsigned short int res1, res2;
   unsigned int offset;             /* Offset to image data      */
   unsigned int bmpheadersize;      /* Header size in bytes      */
   unsigned int width,height;       /* Width and height of image */
   unsigned short int planes;       /* Number of colour planes   */
   unsigned short int bits;         /* Bits per pixel            */
   unsigned int compression;        /* Compression type          */
   unsigned int imagesize;          /* Image size in bytes       */
   int xresolution,yresolution;     /* Pixels per meter          */
   unsigned int ncolours;           /* Number of colours         */
   unsigned int importantcolours;   /* Important colours         */
   char dummy[24];                  /* to keep OS/2 2.0 BMP komp */
} BMPHEADER, *PBMPHEADER;

