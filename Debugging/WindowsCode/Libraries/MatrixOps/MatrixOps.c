/*
 * MatrixOps.c
 *
 *  Created on: Aug 9, 2016
 *      Author: robertirwin
 */

#include 	"MatrixOps.h"


/* Name: 		MyMatInit
 * Description: Initializes the structure MyMat. Takes a pointer to the MyMat
 * 				structure, the number of desired rows, the number of desired columns,
 * 				a void pointer to the array containing all elements of the desired matrix,
 * 				and the type of the elements in the matrix. The type of the elements
 * 				must be defined according to the types in the MyMatType enum.
 * 	***NOTE***: The elements array must adhere to the following format.
 * 	--       --
 * 	| a b c d | ------\       [a, b, c, d, e, f, g, h]
 * 	| e f g h | ------/
 * 	--       --
 */
void MyMatInit(MyMat * mat, uint16_t rows, uint16_t cols, void * elements, MyMatType type)
{
	mat->numrows = rows;
	mat->numcols = cols;
	mat->type = type;

	switch(type){
	case TYPE_INT:
		// initialize the pointer to array of pointers
		//
		mat->rows = malloc(rows * sizeof(int *));

		// allocate memory and fill the matrix
		//
		mat->matrix = malloc(rows * cols * sizeof(int));

		// copy the elements vector into the matrix vector
		//
		memmove(mat->matrix, elements, rows * cols * sizeof(int));

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < rows; i++)
		{
			mat->rows[i] = (int *) (mat->matrix + (i * cols * sizeof(int)));
		}
		break;
	case TYPE_FLOAT:
		// initialize the pointer to array of pointers
		//
		mat->rows = malloc(rows * sizeof(float *));

		// allocate memory and fill the matrix
		//
		mat->matrix = malloc(rows * cols * sizeof(float));

		// copy the elements vector into the matrix vector
		//
		memmove(mat->matrix, elements, rows * cols * sizeof(float));

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < rows; i++)
		{
			mat->rows[i] = (float *) (mat->matrix + (i * cols * sizeof(float)));
		}
		break;
	case TYPE_UINT32:
		// initialize the pointer to array of pointers
		//
		mat->rows = malloc(rows * sizeof(uint32_t *));

		// allocate memory and fill the matrix
		//
		mat->matrix = malloc(rows * cols * sizeof(uint32_t));

		// copy the elements vector into the matrix vector
		//
		memmove(mat->matrix, elements, rows * cols * sizeof(uint32_t));

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < rows; i++)
		{
			mat->rows[i] = (uint32_t *) (mat->matrix + (i * cols * sizeof(uint32_t)));
		}
		break;
	case TYPE_UINT16:
		// initialize the pointer to array of pointers
		//
		mat->rows = malloc(rows * sizeof(uint16_t *));

		// allocate memory and fill the matrix
		//
		mat->matrix = malloc(rows * cols * sizeof(uint16_t));

		// copy the elements vector into the matrix vector
		//
		memmove(mat->matrix, elements, rows * cols * sizeof(uint16_t));

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < rows; i++)
		{
			mat->rows[i] = (uint16_t *) (mat->matrix + (i * cols * sizeof(uint16_t)));
		}
		break;
	case TYPE_CHAR:
		// initialize the pointer to array of pointers
		//
		mat->rows = malloc(rows * sizeof(char *));

		// allocate memory and fill the matrix
		//
		mat->matrix = malloc(rows * cols * sizeof(char));

		// copy the elements vector into the matrix vector
		//
		memmove(mat->matrix, elements, rows * cols * sizeof(char));

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < rows; i++)
		{
			mat->rows[i] = (char *) (mat->matrix + (i * cols * sizeof(char)));
		}
		break;
	case TYPE_UCHAR:
		// initialize the pointer to array of pointers
		//
		mat->rows = malloc(rows * sizeof(uint8_t *));

		// allocate memory and fill the matrix
		//
		mat->matrix = malloc(rows * cols * sizeof(uint8_t));

		// copy the elements vector into the matrix vector
		//
		memmove(mat->matrix, elements, rows * cols * sizeof(uint8_t));

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < rows; i++)
		{
			mat->rows[i] = (uint8_t *) (mat->matrix + (i * cols * sizeof(uint8_t)));
		}
		break;
	}
}

/* Name: 		displayMatrix
 * Description: Prints the matrix defined in the MyMat structure to stdout.
 */
void displayMatrix(MyMat * mat)
{
	int j = 0;
	switch(mat->type)
	{
	case(TYPE_INT):
		for (int i = 0; i < mat->numrows; i++)
		{
			for (j = 0; j < mat->numcols; j++)
			{
				// this line goes to the pointer to thecurrent row being
				// printed (mat->rows[i]), moves to the correct column in the row
				// (+ j * sizeof(int)), casts the pointer as an int pointer, (rows is
				// actually a void pointer) and obtains the value at that location in
				// memory.
				//
				fprintf(stdout,"%d ", *((int *) (mat->rows[i] + j * sizeof(int))));
			}
			j = 0;
			printf("\n");
		}
		break;
	case(TYPE_FLOAT):
		for (int i = 0; i < mat->numrows; i++)
		{
			for (j = 0; j < mat->numcols; j++)
			{
				// this line goes to the pointer to thecurrent row being
				// printed (mat->rows[i]), moves to the correct column in the row
				// (+ j * sizeof(int)), casts the pointer as an int pointer, (rows is
				// actually a void pointer) and obtains the value at that location in
				// memory.
				//
				fprintf(stdout,"%f ", *((float *) (mat->rows[i] + j * sizeof(float))));
			}
			j = 0;
			printf("\n");

		}
		break;
	case(TYPE_UINT32):
		for (int i = 0; i < mat->numrows; i++)
		{
			for (j = 0; j < mat->numcols; j++)
			{
				// this line goes to the pointer to thecurrent row being
				// printed (mat->rows[i]), moves to the correct column in the row
				// (+ j * sizeof(int)), casts the pointer as an int pointer, (rows is
				// actually a void pointer) and obtains the value at that location in
				// memory.
				//
				fprintf(stdout,"%u ", *((uint32_t *) (mat->rows[i] + j * sizeof(uint32_t))));
			}
			j = 0;
			printf("\n");
		}
		break;
	case(TYPE_UINT16):
		for (int i = 0; i < mat->numrows; i++)
		{
			for (j = 0; j < mat->numcols; j++)
			{
				// this line goes to the pointer to thecurrent row being
				// printed (mat->rows[i]), moves to the correct column in the row
				// (+ j * sizeof(int)), casts the pointer as an int pointer, (rows is
				// actually a void pointer) and obtains the value at that location in
				// memory.
				//
				fprintf(stdout,"%u ", *((uint16_t *) (mat->rows[i] + j * sizeof(uint16_t))));
			}
			j = 0;
			printf("\n");
		}
		break;
	case(TYPE_CHAR):
		for (int i = 0; i < mat->numrows; i++)
		{
			for (j = 0; j < mat->numcols; j++)
			{
				// this line goes to the pointer to the current row being
				// printed (mat->rows[i]), moves to the correct column in the row
				// (+ j * sizeof(int)), casts the pointer as an int pointer, (rows is
				// actually a void pointer) and obtains the value at that location in
				// memory.
				//
				fprintf(stdout,"%c ", *((char *) (mat->rows[i] + j * sizeof(char))));
			}
			j = 0;
			printf("\n");
		}
		break;
	case(TYPE_UCHAR):
		for (int i = 0; i < mat->numrows; i++)
		{
			for (j = 0; j < mat->numcols; j++)
			{
				// this line goes to the pointer to thecurrent row being
				// printed (mat->rows[i]), moves to the correct column in the row
				// (+ j * sizeof(int)), casts the pointer as an int pointer, (rows is
				// actually a void pointer) and obtains the value at that location in
				// memory.
				//
				fprintf(stdout,"%u ", *((uint8_t *) (mat->rows[i] + j * sizeof(uint8_t))));
			}
			j = 0;
			printf("\n");
		}
		break;
	}
}

/* Name:		MyMatAppendRow
 * Description:	Appends a new row to the matrix pointed to by the MyMat structure. The
 * 				oldest (top) row is removed from the matrix.
 */
void MyMatAppendRow(MyMat * mat, void * new_row)
{
	void * temp; // temporary buffer to hold the old matrix
	switch(mat->type){
	case TYPE_INT:
		// allocate memory for the temporary buffer
		//
		temp = malloc((mat->numrows - 1) * mat->numcols * sizeof(int));

		// move everything except for the first row (oldest) to the temporary matrix
		//
		memmove(temp, (mat->rows[1]), (mat->numrows - 1) * mat->numcols * sizeof(int));

		// move the temp matrix back
		//
		memmove(mat->matrix, temp, (mat->numrows - 1) * mat->numcols * sizeof(int));

		// move the new row to the end of the matrix
		//
		memmove(mat->rows[mat->numrows-1], new_row, mat->numcols * sizeof(int));
		free(temp);
		break;
	case TYPE_FLOAT:
		// allocate memory for the temporary buffer
		//
		temp = malloc((mat->numrows - 1) * mat->numcols * sizeof(float));

		// move everything except for the first row (oldest) to the temporary matrix
		//
		memmove(temp, (mat->rows[1]), (mat->numrows - 1) * mat->numcols * sizeof(float));

		// move the temp matrix back
		//
		memmove(mat->matrix, temp, (mat->numrows - 1) * mat->numcols * sizeof(float));

		// move the new row to the end of the matrix
		//
		memmove(mat->rows[mat->numrows-1], new_row, mat->numcols * sizeof(float));
		free(temp);
		break;
	case TYPE_UINT32:
		// allocate memory for the temporary buffer
		//
		temp = malloc((mat->numrows - 1) * mat->numcols * sizeof(uint32_t));

		// move everything except for the first row (oldest) to the temporary matrix
		//
		memmove(temp, (mat->rows[1]), (mat->numrows - 1) * mat->numcols * sizeof(uint32_t));

		// move the temp matrix back
		//
		memmove(mat->matrix, temp, (mat->numrows - 1) * mat->numcols * sizeof(uint32_t));

		// move the new row to the end of the matrix
		//
		memmove(mat->rows[mat->numrows-1], new_row, mat->numcols * sizeof(uint32_t));
		free(temp);
		break;
	case TYPE_UINT16:
			// allocate memory for the temporary buffer
			//
			temp = malloc((mat->numrows - 1) * mat->numcols * sizeof(uint16_t));

			// move everything except for the first row (oldest) to the temporary matrix
			//
			memmove(temp, (mat->rows[1]), (mat->numrows - 1) * mat->numcols * sizeof(uint16_t));

			// move the temp matrix back
			//
			memmove(mat->matrix, temp, (mat->numrows - 1) * mat->numcols * sizeof(uint16_t));

			// move the new row to the end of the matrix
			//
			memmove(mat->rows[mat->numrows-1], new_row, mat->numcols * sizeof(uint16_t));
			free(temp);
			break;
	case TYPE_CHAR:
		// allocate memory for the temporary buffer
		//
		temp = malloc((mat->numrows - 1) * mat->numcols * sizeof(char));

		// move everything except for the first row (oldest) to the temporary matrix
		//
		memmove(temp, (mat->rows[1]), (mat->numrows - 1) * mat->numcols * sizeof(char));

		// move the temp matrix back
		//
		memmove(mat->matrix, temp, (mat->numrows - 1) * mat->numcols * sizeof(char));

		// move the new row to the end of the matrix
		//
		memmove(mat->rows[mat->numrows-1], new_row, mat->numcols * sizeof(char));
		free(temp);
		break;
	case TYPE_UCHAR:
		// allocate memory for the temporary buffer
		//
		temp = malloc((mat->numrows - 1) * mat->numcols * sizeof(uint8_t));

		// move everything except for the first row (oldest) to the temporary matrix
		//
		memmove(temp, (mat->rows[1]), (mat->numrows - 1) * mat->numcols * sizeof(uint8_t));

		// move the temp matrix back
		//
		memmove(mat->matrix, temp, (mat->numrows - 1) * mat->numcols * sizeof(uint8_t));

		// move the new row to the end of the matrix
		//
		memmove(mat->rows[mat->numrows-1], new_row, mat->numcols * sizeof(uint8_t));
		free(temp);
		break;
	}
}

/* Name: 		MyMatTranspose
 * Description: Inputs to this function are the pointer to the matrix structure
 * 				that will be transposed, and the pointer to the matrix structure that will
 * 				hold the transposed matrix.
 * ** NOTE ** 	This function allocates space for trans->matrix and trans->rows. The user must
 * 				free this memory or risk having memory leaks in their software.
 */
void MyMatTranspose(MyMat * mat, MyMat * trans)
{
	// initialize some information about the transposed matrix
	//
	trans->numrows = mat->numcols;
	trans->numcols = mat->numrows;
	trans->type = mat->type;
	int i = 0;
	switch(trans->type){
	case TYPE_INT:
		// initialize the pointer to array of pointers
		//
		trans->rows = malloc(trans->numrows * sizeof(int *));

		// allocate space for the matrix.
		//
		trans->matrix = malloc(trans->numrows * trans->numcols * sizeof(int));

		// begin transposing the matrix
		//
		i = 0;
		for (int rows = 0; rows < trans->numrows; rows++)
		{
			for (int cols = 0; cols < trans->numcols; cols++)
			{
				memcpy((int *) trans->matrix+i, (int *) mat->matrix + rows + (trans->numrows * cols), sizeof(int));
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < trans->numrows; i++)
		{
			trans->rows[i] = (int *) (trans->matrix + (i * trans->numcols * sizeof(int)));
		}
		break;
	case TYPE_FLOAT:
		// initialize the pointer to array of pointers
		//
		trans->rows = malloc(trans->numrows * sizeof(float *));

		// allocate space for the matrix. To avoid memory leaks, use realloc instead of malloc
		// in case the user uses the same transpose structure to hold transposes on multiple iterations
		// of code.
		//
		trans->matrix = malloc(trans->numrows * trans->numcols * sizeof(float));

		// begin transposing the matrix
		//
		i = 0;
		for (int rows = 0; rows < trans->numrows; rows++)
		{
			for (int cols = 0; cols < trans->numcols; cols++)
			{
				memcpy((float *) trans->matrix+i, (float *) mat->matrix + rows + (trans->numrows * cols), sizeof(float));
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < trans->numrows; i++)
		{
			trans->rows[i] = (float *) (trans->matrix + (i * trans->numcols * sizeof(float)));
		}
		break;
	case TYPE_UINT32:
		// initialize the pointer to array of pointers
		//
		trans->rows = malloc(trans->numrows * sizeof(uint32_t *));

		// allocate space for the matrix.
		//
		trans->matrix = malloc(trans->numrows * trans->numcols * sizeof(uint32_t));

		// begin transposing the matrix
		//
		i = 0;
		for (int rows = 0; rows < trans->numrows; rows++)
		{
			for (int cols = 0; cols < trans->numcols; cols++)
			{
				memcpy((uint32_t *) trans->matrix+i, (uint32_t *) mat->matrix + rows + (trans->numrows * cols), sizeof(uint32_t));
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < trans->numrows; i++)
		{
			trans->rows[i] = (uint32_t *) (trans->matrix + (i * trans->numcols * sizeof(uint32_t)));
		}
		break;
	case TYPE_UINT16:
		// initialize the pointer to array of pointers
		//
		trans->rows = malloc(trans->numrows * sizeof(uint16_t *));

		// allocate space for the matrix.
		//
		trans->matrix = malloc(trans->numrows * trans->numcols * sizeof(uint16_t));

		// begin transposing the matrix
		//
		i = 0;
		for (int rows = 0; rows < trans->numrows; rows++)
		{
			for (int cols = 0; cols < trans->numcols; cols++)
			{
				memcpy((uint16_t *) trans->matrix+i, (uint16_t *) mat->matrix + rows + (trans->numrows * cols), sizeof(uint16_t));
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < trans->numrows; i++)
		{
			trans->rows[i] = (uint16_t *) (trans->matrix + (i * trans->numcols * sizeof(uint16_t)));
		}
		break;
	case TYPE_CHAR:
		// initialize the pointer to array of pointers
		//
		trans->rows = malloc(trans->numrows * sizeof(char *));

		// allocate space for the matrix.
		//
		trans->matrix = malloc(trans->numrows * trans->numcols * sizeof(char));

		// begin transposing the matrix
		//
		i = 0;
		for (int rows = 0; rows < trans->numrows; rows++)
		{
			for (int cols = 0; cols < trans->numcols; cols++)
			{
				memcpy((char *) trans->matrix+i, (char *) mat->matrix + rows + (trans->numrows * cols), sizeof(char));
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < trans->numrows; i++)
		{
			trans->rows[i] = (char *) (trans->matrix + (i * trans->numcols * sizeof(char)));
		}
		break;
	case TYPE_UCHAR:
		// initialize the pointer to array of pointers
		//
		trans->rows = malloc(trans->numrows * sizeof(uint8_t *));

		// allocate space for the matrix.
		//
		trans->matrix = malloc(trans->numrows * trans->numcols * sizeof(uint8_t));

		// begin transposing the matrix
		//
		i = 0;
		for (int rows = 0; rows < trans->numrows; rows++)
		{
			for (int cols = 0; cols < trans->numcols; cols++)
			{
				memcpy((uint8_t *) trans->matrix+i, (uint8_t *) mat->matrix + rows + (trans->numrows * cols), sizeof(uint8_t));
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < trans->numrows; i++)
		{
			trans->rows[i] = (uint8_t *) (trans->matrix + (i * trans->numcols * sizeof(uint8_t)));
		}
		break;
	}

}

/* Name:		MyMatMultiply
 * Description:	Inputs are the pointer to the two matrices to be multipled, and the pointer
 * 				to the matrix the result is to be stored in. AB=res. Returns 1 if the operation
 * 				was successful and returns 0 if a dimension mismatch has occured.
 * ** NOTE ** 	This function allocates space for res->matrix and res->rows. The user must
 * 				free this memory or risk having memory leaks in their software.
 * 				Also, A and B should be of the same type. The result will be of the same type as A.
 */
int MyMatMultiply(MyMat * A, MyMat * B, MyMat * res)
{
	int i = 0;
	int sumint = 0;
	float sumfloat = 0.0;
	uint32_t sum32 = 0;
	uint16_t sum16 = 0;
	char sumchar = 0;
	uint8_t sum8 = 0;
	// Ensure the dimensions of the matrices are compatible.
	//
	if (A->numcols != B->numrows)
	{
		printf("Error: Dimension Mismatch.\nColumns in A = %d\nRows in B = %d.\n", A->numcols, B->numrows);
		return(0);
	}

	// Assign the size of the new matrix
	//
	res->numrows = A->numrows;
	res->numcols = B->numcols;

	// Determine the variable type of the matrices
	//
	res->type = A->type;
	switch(res->type){
	case TYPE_INT:
		// initialize the pointer to array of pointers
		//
		res->rows = malloc(res->numrows * sizeof(int *));

		// allocate space for the matrix.
		//
		res->matrix = malloc(res->numrows * res->numcols * sizeof(int));

		// begin multiplying the matrices
		//
		for (int rowA = 0; rowA < A->numrows; rowA++)
		{
			for (int colB = 0; colB < B->numcols; colB++)
			{
				for (int colA = 0; colA < A->numcols; colA++)
				{
					sumint += (int) *(int *)(A->rows[rowA] + (colA * sizeof(int))) * (int) *(int *)(B->rows[colA] + (colB * sizeof(int)));
				}
				// move the sum to the current place in the  result matrix
				//
				memcpy(res->matrix + (i * sizeof(int)), (int *) &sumint, sizeof(int));

				sumint = (int) 0; // reset sum

				//move to the next place in the result matrix for the next iteration
				//
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < res->numrows; i++)
		{
			res->rows[i] = (int *) (res->matrix + (i * res->numcols * sizeof(int)));
		}
		break;
	case TYPE_FLOAT:
		// initialize the pointer to array of pointers
		//
		res->rows = malloc(res->numrows * sizeof(float *));

		// allocate space for the matrix.
		//
		res->matrix = malloc(res->numrows * res->numcols * sizeof(float));

		// begin multiplying the matrices
		//
		for (int rowA = 0; rowA < A->numrows; rowA++)
		{
			for (int colB = 0; colB < B->numcols; colB++)
			{
				for (int colA = 0; colA < A->numcols; colA++)
				{
					sumfloat += (float) *(float *)(A->rows[rowA] + (colA * sizeof(float))) * (float) *(float *)(B->rows[colA] + (colB * sizeof(float)));
				}
				// move the sum to the current place in the  result matrix
				//
				memcpy(res->matrix + (i * sizeof(float)), (float *) &sumfloat, sizeof(float));

				sumfloat = (float) 0.0; // reset sum

				//move to the next place in the result matrix for the next iteration
				//
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < res->numrows; i++)
		{
			res->rows[i] = (float *) (res->matrix + (i * res->numcols * sizeof(float)));
		}
		break;
	case TYPE_UINT32:
		// initialize the pointer to array of pointers
		//
		res->rows = malloc(res->numrows * sizeof(uint32_t *));

		// allocate space for the matrix.
		//
		res->matrix = malloc(res->numrows * res->numcols * sizeof(uint32_t));

		// begin multiplying the matrices
		//
		for (int rowA = 0; rowA < A->numrows; rowA++)
		{
			for (int colB = 0; colB < B->numcols; colB++)
			{
				for (int colA = 0; colA < A->numcols; colA++)
				{
					sum32 += (uint32_t) *(uint32_t *)(A->rows[rowA] + (colA * sizeof(uint32_t))) * (uint32_t) *(uint32_t *)(B->rows[colA] + (colB * sizeof(uint32_t)));
				}
				// move the sum to the current place in the  result matrix
				//
				memcpy(res->matrix + (i * sizeof(uint32_t)), (uint32_t *) &sum32, sizeof(uint32_t));

				sum32 = (uint32_t) 0; // reset sum

				//move to the next place in the result matrix for the next iteration
				//
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < res->numrows; i++)
		{
			res->rows[i] = (uint32_t *) (res->matrix + (i * res->numcols * sizeof(uint32_t)));
		}
		break;
	case TYPE_UINT16:
		// initialize the pointer to array of pointers
		//
		res->rows = malloc(res->numrows * sizeof(uint16_t *));

		// allocate space for the matrix.
		//
		res->matrix = malloc(res->numrows * res->numcols * sizeof(uint16_t));

		// begin multiplying the matrices
		//
		for (int rowA = 0; rowA < A->numrows; rowA++)
		{
			for (int colB = 0; colB < B->numcols; colB++)
			{
				for (int colA = 0; colA < A->numcols; colA++)
				{
					sum16 += (uint16_t) *(uint16_t *)(A->rows[rowA] + (colA * sizeof(uint16_t))) * (uint16_t) *(uint16_t *)(B->rows[colA] + (colB * sizeof(uint16_t)));
				}
				// move the sum to the current place in the  result matrix
				//
				memcpy(res->matrix + (i * sizeof(uint16_t)), (uint16_t *) &sum16, sizeof(uint16_t));

				sum16 = (uint16_t) 0; // reset sum

				//move to the next place in the result matrix for the next iteration
				//
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < res->numrows; i++)
		{
			res->rows[i] = (uint16_t *) (res->matrix + (i * res->numcols * sizeof(uint16_t)));
		}
		break;
	case TYPE_CHAR:
		// initialize the pointer to array of pointers
		//
		res->rows = malloc(res->numrows * sizeof(char *));

		// allocate space for the matrix.
		//
		res->matrix = malloc(res->numrows * res->numcols * sizeof(char));

		// begin multiplying the matrices
		//
		for (int rowA = 0; rowA < A->numrows; rowA++)
		{
			for (int colB = 0; colB < B->numcols; colB++)
			{
				for (int colA = 0; colA < A->numcols; colA++)
				{
					sumchar += (char) *(char*)(A->rows[rowA] + (colA * sizeof(char))) * (char) *(char *)(B->rows[colA] + (colB * sizeof(char)));
				}
				// move the sum to the current place in the  result matrix
				//
				memcpy(res->matrix + (i * sizeof(char)), (char *) &sumchar, sizeof(char));

				sumchar = (char) 0; // reset sum

				//move to the next place in the result matrix for the next iteration
				//
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < res->numrows; i++)
		{
			res->rows[i] = (char *) (res->matrix + (i * res->numcols * sizeof(char)));
		}
		break;
	case TYPE_UCHAR:
		// initialize the pointer to array of pointers
		//
		res->rows = malloc(res->numrows * sizeof(uint8_t *));

		// allocate space for the matrix.
		//
		res->matrix = malloc(res->numrows * res->numcols * sizeof(uint8_t));

		// begin multiplying the matrices
		//
		for (int rowA = 0; rowA < A->numrows; rowA++)
		{
			for (int colB = 0; colB < B->numcols; colB++)
			{
				for (int colA = 0; colA < A->numcols; colA++)
				{
					sum8 += (uint8_t) *(uint8_t *)(A->rows[rowA] + (colA * sizeof(uint8_t))) * (uint8_t) *(uint8_t *)(B->rows[colA] + (colB * sizeof(uint8_t)));
				}
				// move the sum to the current place in the  result matrix
				//
				memcpy(res->matrix + (i * sizeof(uint8_t)), (uint8_t *) &sum8, sizeof(uint8_t));

				sum8 = (uint8_t) 0; // reset sum

				//move to the next place in the result matrix for the next iteration
				//
				i++;
			}
		}

		// assign the row pointers to the proper place in the matrix
		//
		for (int i = 0; i < res->numrows; i++)
		{
			res->rows[i] = (uint8_t *) (res->matrix + (i * res->numcols * sizeof(uint8_t)));
		}
		break;
	}
	return(1);
}

