// #ifdef __cplusplus
// extern "C" {
// #endif

//========================================================================================================================================================================================================200
//======================================================================================================================================================150
//====================================================================================================100
//==================================================50

//========================================================================================================================================================================================================200
//	INCLUDE/DEFINE
//========================================================================================================================================================================================================200

//======================================================================================================================================================150
//	LIBRARIES
//======================================================================================================================================================150

#include <stdio.h>									// (in path known to compiler)	needed by printf
#include <stdlib.h>									// (in path known to compiler)	needed by malloc, free
#include <string.h>

//======================================================================================================================================================150
//	HEADER
//======================================================================================================================================================150

#include "./main.h"									// (in current path)

//======================================================================================================================================================150
//	UTILITIES
//======================================================================================================================================================150

#include "./util/graphics/graphics.h"				// (in specified path)
#include "./util/graphics/resize.h"					// (in specified path)
#include "./util/timer/timer.h"						// (in specified path)

//======================================================================================================================================================150
//	KERNEL
//======================================================================================================================================================150

#include "./kernel/kernel_gpu_opencl_wrapper.h"

//======================================================================================================================================================150
//	End
//======================================================================================================================================================150

//========================================================================================================================================================================================================200
//	MAIN FUNCTION
//========================================================================================================================================================================================================200

int platform_id_inuse = 0;            // platform id in use (default: 0)
int device_id_inuse = 0;              // device id in use (default : 0)

int 
main(	int argc, 
		char* argv []){
  printf("WG size of kernel = %d \n", NUMBER_THREADS);
	//======================================================================================================================================================150
	// 	VARIABLES
	//======================================================================================================================================================150

	// inputs image, input paramenters
	fp* image_ori;																// originalinput image
	int image_ori_rows;
	int image_ori_cols;
	long image_ori_elem;

	// inputs image, input paramenters
	fp* image;															// input image
	int Nr,Nc;													// IMAGE nbr of rows/cols/elements
	long Ne;

	// algorithm parameters
	int niter;																// nbr of iterations
	fp lambda;															// update step size

	// size of IMAGE
	int r1,r2,c1,c2;												// row/col coordinates of uniform ROI
	long NeROI;														// ROI nbr of elements

	// surrounding pixel indicies
	int* iN;
	int* iS;
	int* jE;
	int* jW;

	// counters
	int iter;   // primary loop
	long i;    // image row
	long j;    // image col

	// memory sizes
	int mem_size_i;
	int mem_size_j;

	//======================================================================================================================================================150
	//	INPUT ARGUMENTS
	//======================================================================================================================================================150

    int cur_arg;
	for (cur_arg = 1; cur_arg<argc; cur_arg++) {
        if (strcmp(argv[cur_arg], "-n") == 0) {
            if (argc >= cur_arg + 1) {
		        niter = atoi(argv[cur_arg+1]);
                cur_arg++;
            }
        }
        else if (strcmp(argv[cur_arg], "-l") == 0) {
            if (argc >= cur_arg + 1) {
		        lambda = atoi(argv[cur_arg+1]);
                cur_arg++;
            }
        }
        else if (strcmp(argv[cur_arg], "-s") == 0) {
            if (argc >= cur_arg + 3) {
                Nr = atoi(argv[cur_arg+1]);			// it is 502 in the original image
        		Nc = atoi(argv[cur_arg+2]);			// it is 458 in the original image
                cur_arg += 2;
            }
        }
        else if (strcmp(argv[cur_arg], "-p") == 0) {
            if (argc >= cur_arg + 1) {
                platform_id_inuse = atoi(argv[cur_arg+1]);
                cur_arg++;
            }
        }
        else if (strcmp(argv[cur_arg], "-d") == 0) {
            if (argc >= cur_arg + 1) {
                device_id_inuse = atoi(argv[cur_arg+1]);
                cur_arg++;
            }
        }
    }

	//======================================================================================================================================================150
	// 	READ INPUT FROM FILE
	//======================================================================================================================================================150

	//====================================================================================================100
	// 	READ IMAGE (SIZE OF IMAGE HAS TO BE KNOWN)
	//====================================================================================================100

	image_ori_rows = 502;
	image_ori_cols = 458;
	image_ori_elem = image_ori_rows * image_ori_cols;

	image_ori = (fp*)malloc(sizeof(fp) * image_ori_elem);

	read_graphics(	"../../data/srad/image.pgm",
					image_ori,
					image_ori_rows,
					image_ori_cols,
					1);

	//====================================================================================================100
	// 	RESIZE IMAGE (ASSUMING COLUMN MAJOR STORAGE OF image_orig)
	//====================================================================================================100

	Ne = Nr*Nc;

	image = (fp*)malloc(sizeof(fp) * Ne);

	resize(	image_ori,
				image_ori_rows,
				image_ori_cols,
				image,
				Nr,
				Nc,
				1);

	//====================================================================================================100
	// 	End
	//====================================================================================================100

	//======================================================================================================================================================150
	// 	SETUP
	//======================================================================================================================================================150

	// variables
	r1     = 0;											// top row index of ROI
	r2     = Nr - 1;									// bottom row index of ROI
	c1     = 0;											// left column index of ROI
	c2     = Nc - 1;									// right column index of ROI

	// ROI image size
	NeROI = (r2-r1+1)*(c2-c1+1);											// number of elements in ROI, ROI size

	// allocate variables for surrounding pixels
	mem_size_i = sizeof(int) * Nr;											//
	iN = (int *)malloc(mem_size_i) ;										// north surrounding element
	iS = (int *)malloc(mem_size_i) ;										// south surrounding element
	mem_size_j = sizeof(int) * Nc;											//
	jW = (int *)malloc(mem_size_j) ;										// west surrounding element
	jE = (int *)malloc(mem_size_j) ;										// east surrounding element

	// N/S/W/E indices of surrounding pixels (every element of IMAGE)
	for (i=0; i<Nr; i++) {
		iN[i] = i-1;														// holds index of IMAGE row above
		iS[i] = i+1;														// holds index of IMAGE row below
	}
	for (j=0; j<Nc; j++) {
		jW[j] = j-1;														// holds index of IMAGE column on the left
		jE[j] = j+1;														// holds index of IMAGE column on the right
	}

	// N/S/W/E boundary conditions, fix surrounding indices outside boundary of image
	iN[0]    = 0;															// changes IMAGE top row index from -1 to 0
	iS[Nr-1] = Nr-1;														// changes IMAGE bottom row index from Nr to Nr-1 
	jW[0]    = 0;															// changes IMAGE leftmost column index from -1 to 0
	jE[Nc-1] = Nc-1;														// changes IMAGE rightmost column index from Nc to Nc-1

	//======================================================================================================================================================150
	// 	KERNEL
	//======================================================================================================================================================150

	kernel_gpu_opencl_wrapper(	image,										// input image
								Nr,											// IMAGE nbr of rows
								Nc,											// IMAGE nbr of cols
								Ne,											// IMAGE nbr of elem
								niter,										// nbr of iterations
								lambda,										// update step size
								NeROI,										// ROI nbr of elements
								iN,
								iS,
								jE,
								jW,
								iter,										// primary loop
								mem_size_i,
								mem_size_j);

	//======================================================================================================================================================150
	// 	WRITE OUTPUT IMAGE TO FILE
	//======================================================================================================================================================150

	write_graphics(	"./output/image_out.pgm",
					image,
					Nr,
					Nc,
					1,
					255);


	//======================================================================================================================================================150
	// 	FREE MEMORY
	//======================================================================================================================================================150

	free(image_ori);
	free(image);
	free(iN); 
	free(iS); 
	free(jW); 
	free(jE);

	//======================================================================================================================================================150
	//	DISPLAY TIMING
	//======================================================================================================================================================150
}

//========================================================================================================================================================================================================200
//	END
//========================================================================================================================================================================================================200
