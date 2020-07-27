#include "globals.h"


/**********************************************************************
	ModRange		:	Modulo reduction of error.
						val - value to reduct
						range - range
**********************************************************************/
int ModRange(int val, int range)
{
 	if(val < 0)
 	{ 	
   		val = val + range;
 	}
 	if(val >= ((range+1)/2))
 	{
 		val = val - range;
	}
 	return val;
}

/**********************************************************************
	Allocate3D		:	This function allocates memory for 3d array
						of char's.
						width - width of matrix
						height - height of matrix
						return NULL if cannot allocate memory
						return pointer to 2d tables of pointers (3d matrix)
**********************************************************************/
char*** Allocate3D(int width, int height)
{
	char*** out_data;
	out_data = (char***)malloc(3 * sizeof(char**));
	char** red_data = Allocate2D(width, height);
	char** green_data = Allocate2D(width, height);
	char** blue_data = Allocate2D(width, height);
	out_data[0] = red_data;
	out_data[1] = green_data;
	out_data[2] = blue_data;
	return out_data;
}

/**********************************************************************
	Allocate2D		:	This function allocates memory for 2d array
						of char's.
						width - width of matrix
						height - height of matrix
						return NULL if cannot allocate memory
						return pointer to tables of pointers (2d matrix)
**********************************************************************/
char** Allocate2D(int width, int height)
{
	int i;
  	char **prow, *pdata;
  	/* allocate and clean memory for all emenents */
  	pdata = (char*)calloc(width * height, sizeof(char));
  	if (pdata == NULL)
    {
      	printf("ERROR\tError while allocating data for 2d array.\n");
		return NULL;
    }
    /* allocate memory for pointers to rows */    
  	prow = (char**)malloc(height * sizeof(char*));
  	if (prow == NULL)
    {
      	printf("ERROR\tError while allocating data for 2d array.\n");
		return NULL;
    }
    /* assign to each row pointer, pointer to data */
  	for (i = 0; i < height ; i++)
    {
      	prow[i] = pdata;       
      	pdata += sizeof(char) * width;    
    }
    return prow;             
}

/**********************************************************************
	Allocate1D		:	This function allocates and clear
						memory for 1d array of char's.
						lenght - lenght of vector
						return NULL if cannot allocate memory
						return pointer to tables of chars (1d array)
**********************************************************************/
char* Allocate1D(int lenght)
{
	char* data;
	/* allocate and clean memory for all elements */
	data = (char*)calloc(lenght,sizeof(char));
	if (data == NULL)
    {
      	fprintf(stderr,"ERROR\tError while allocating data for 1d array.\n");
		return NULL;
    }
	return data; 
}

/**********************************************************************
	Free3D			:	This function free memory allocated for 3d
						char's array
						data - pointer to table of pointers
**********************************************************************/
void Free3D(char*** data)
{
	Free2D(data[0]);
	Free2D(data[1]);
	Free2D(data[2]);
	free(data);
	data = NULL;
}

/**********************************************************************
	Free2D			:	This function free memory allocated for 2d
						char's array
						data - pointer to table of pointers
**********************************************************************/
void Free2D(char** data)
{
	free(*data);
	free(data);
	data = NULL;
}

/**********************************************************************
	Free1D			:	This function free memory allocated for 1d
						char's array
						data - pointer to table 
**********************************************************************/
void Free1D(char* data)
{
	free(data);
	data = NULL;
}
