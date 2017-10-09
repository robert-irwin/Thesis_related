/*
 * MatrixOps.h
 *
 *  Created on: Aug 9, 2016
 *      Author: robertirwin
 */

#ifndef MATRIXOPS_H_
#define MATRIXOPS_H_


# include 	<string.h>
# include	<stdlib.h>
# include 	<stdio.h>
# include 	<stdint.h>

typedef enum MyMatType{
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_UINT32,
	TYPE_UINT16,
	TYPE_CHAR,
	TYPE_UCHAR

}MyMatType;

typedef struct MyMat{
	void * matrix;
	void ** rows; // pointer to pointer of each row
	uint16_t numrows;
	uint16_t numcols;
	void ** rows_trans;
	int type;
}MyMat;

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
void MyMatInit(MyMat * mat, uint16_t rows, uint16_t cols, void * elements, MyMatType type);

/* Name: 		displayMatrix
 * Description: Prints the matrix defined in the MyMat structure to stdout.
 */
void displayMatrix(MyMat * mat);

/* Name:		MyMatAppendRow
 * Description:	Appends a new row to the matrix pointed to by the MyMat structure. The
 * 				oldest (top) row is removed from the matrix.
 */
void MyMatAppendRow(MyMat * mat, void * new_row);

/* Name: 		MyMatTranspose
 * Description: Inputs to this function are the pointer to the matrix structure
 * 				that will be transposed, and the pointer to the matrix structure that will
 * 				hold the transposed matrix.
 * ** NOTE ** 	This function allocates space for trans->matrix and trans->rows. The user must
 * 				free this memory or risk having memory leaks in their software.
 */
void MyMatTranspose(MyMat * mat, MyMat * trans);

/* Name:		MyMatMultiply
 * Description:	Inputs are the pointer to the two matrices to be multipled, and the pointer
 * 				to the matrix the result is to be stored in. AB=res. Returns 1 if the operation
 * 				was successful and returns 0 if a dimension mismatch has occured.
 * ** NOTE ** 	This function allocates space for res->matrix and res->rows. The user must
 * 				free this memory or risk having memory leaks in their software.
 */
int MyMatMultiply(MyMat * A, MyMat * B, MyMat * res);

#endif /* MATRIXOPS_H_ */
