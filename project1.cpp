
#include <iostream>
using namespace std;

class CSR
{
protected:
	int n; //The number rows of the original matrix
	int m; //The number of columns of the original matrix
	int nonZeros; //The number of non-zeros in the original matrix
	int* values; //value array assuming all are integers
	int* rowPtr; //Array that contains number of non-zero elements in each row of the original matrix
	int* colPos; //Array that contains the column number from the original matrix for the non-zero values.
	int positionCounter; // keeps track of where in the CSR you are
	int tempRowTracker;
	int rowLength;
	//There may be others you may need
public:
	CSR(); //default or empty constructor
	CSR(CSR& matrixB); //copy constructor
	CSR(int rows, int cols, int numNonZeros);
	int getNumRows();
	void addToCSR(int row, int col, int value);
	void display(); //print the contents of the three arrays. Each array must be on a different line (separated by a
								  //new line character) and you have exactly a single space between each value printed.
	int* matrixVectorMultiply(int* intputVector); //matrix-vector multiplication
	CSR* matrixMultiply(CSR& matrixB); //matrix-matrix multiplication
	int* getColumnVector(int col);
	int* getRowVec(int row);

	~CSR(); //destructor
	//You may have other methods
private:
	void addValue(int value); //add a new value in the values array
	void addColumn(int col);//add a column in the colPos array
	void addRow(int row); //add a row in the rowPtr array
};

//some of the methods – Sridhar Radhakrishnan
CSR::CSR()
{
	n = 0;
	m = 0;
	values = NULL;
	rowPtr = NULL;
	colPos = NULL;
	positionCounter = 0;
	tempRowTracker = -1;
	rowLength = 0;
}
CSR::CSR(int rows, int cols, int numNonZeros)
{
	n = rows;
	m = cols;
	nonZeros = numNonZeros;
	values = new int[nonZeros];
	colPos = new int[nonZeros];
	rowPtr = new int[n];
	positionCounter = 0;
	tempRowTracker = -1;
	rowLength = 0;
}
CSR::CSR(CSR& matrixB)
{
	n = matrixB.n;
	m = matrixB.m;

	nonZeros = matrixB.nonZeros;

	values = matrixB.values;
	rowPtr = matrixB.rowPtr;
	colPos = matrixB.colPos;
	positionCounter = matrixB.positionCounter;
	tempRowTracker = matrixB.tempRowTracker;
	rowLength = matrixB.rowLength;
}
CSR::~CSR()
{
	if (values != NULL) delete[] values;
	if (rowPtr != NULL) delete[] rowPtr;
	if (colPos != NULL) delete[] colPos;
	cout << "CSR Object Destroyed!!" << endl;
	n = 0;
	m = 0;
	nonZeros = 0;
}


int CSR::getNumRows()
{
	return n;
}

//Added this method to make keeping track of the positionCounter smoother and to simplify the code in the main method
void CSR::addToCSR(int row, int col, int value)
{
	addRow(row);
	addColumn(col);
	addValue(value);

	positionCounter++;
}

void CSR::addValue(int value)
{
	values[positionCounter] = value;
}

void CSR::addColumn(int col)
{
	colPos[positionCounter] = col;
}

void CSR::addRow(int row)
{
	//If the row that is selected has not yet been
	if (row != tempRowTracker)
	{
		tempRowTracker = row;
		rowPtr[rowLength] = positionCounter;
		rowLength++;
	}
}

//print the contents of the three arrays. Each array must be on a different line (separated by a
//new line character) and you have exactly a single space between each value printed.
void CSR::display()
{
	cout << endl << "PRINTING MATRIX";
	cout << endl << "rowPtr: ";
	for (int i = 0; i < rowLength; i++)
	{
		cout << rowPtr[i] << " ";
	}

	cout << endl << "colPos: ";
	for (int i = 0; i < positionCounter; i++)
	{
		cout << colPos[i] << " ";
	}

	cout << endl << "value: ";
	for (int i = 0; i < positionCounter; i++)
	{
		cout << values[i] << " ";
	}
}

CSR* CSR::matrixMultiply(CSR& matrixB)
{
	//When you multiply, the number of rows in matrix A
	//has to equal the number of rows in B
	if (!(n == matrixB.m))
	{
		cout << endl << "Error multiplying: Matrix size mismatch";
		return NULL;
	}

	//The resultant matrix has dimensions of
	// rows = rows in matrix A
	// and columns = columns in matrix B
	CSR* matrixC = new CSR(n, matrixB.m, (n * matrixB.m));

	//Variable to hold the sum of a row * a column to be eventually stored
	//in matrix C
	int sum = 0;

	//Variable to hold the row values to be multiplied
	int* rowVals;

	//Variable to hold the column values to be multiplied
	int* colVals;

	//For loop to loop through each row in matrix A
	for (int i = 0; i < n; i++)
	{
		rowVals = getRowVec(i);
		//For loop to loop through each column in matrix B
		for (int j = 0; j < matrixB.m; j++)
		{
			colVals = matrixB.getColumnVector(j);

			//For loop to multiply the values in the current row by current column and sum them
			for (int k = 0; k < n; k++)
			{
				sum = sum + (rowVals[k] * colVals[k]);
			}

			//After you complete a column, store it in the matrix
			matrixC->addToCSR(n, matrixB.m, sum);
		}
	}

	return new CSR(1, 2, 3);
}

/**
 * This method will help you fetch a column of values from the matrix
 *
 * Provided the column number as an argument,
 * the method returns an array which contains the column values.
 *
 */
int* CSR::getColumnVector(int col) //all rows of column col
{
	int* colVector = new int[n];
	int r;
	for (int i = 0; i < n; i++)
		colVector[i] = 0;

	bool found;
	int k, j;

	k = 0;
	for (int i = 0; i < n - 1; i++)
	{
		r = rowPtr[i + 1] - rowPtr[i];
		k = rowPtr[i];
		found = false;
		j = 0;
		while ((j < r) && !found)
		{
			if (colPos[k] == col)
			{
				found = true;
				colVector[i] = values[k];
			}
			k++;
			j++;
		}
	}
	int p = rowPtr[n - 1];
	found = false;
	while ((p < (nonZeros)) && (!found))
	{
		if (colPos[p] == col) {
			colVector[n - 1] = values[p];
			found = true;
		}
		else
			p++;
	}
	return colVector;
}

/**
 * This method will help you fetch a row of values from the matrix
 *
 * Provided the row number as an argument,
 * the method returns an array which contains the row values.
 *
 */

int* CSR::getRowVec(int row)
{

	int* vector = new int[n];
	for (int i = 0; i < n; i++)
		vector[i] = 0;

	if (row < n - 1)
	{

		for (int i = rowPtr[row]; i < rowPtr[row + 1]; i++)
		{
			for (int j = 0; j < m; j++)
			{
				if (colPos[i] == j)
					vector[j] = values[i];
			}
		}
	}
	else {
		for (int i = rowPtr[row]; i < nonZeros; i++)
		{
			for (int j = 0; j < m; j++)
			{
				if (colPos[i] == j)
					vector[j] = values[i];
			}
		}
	}
	return vector;
}


/********************************************************************
An error in Matrix-Vector multiplication, please use the following code
********************************************************************/

int* CSR::matrixVectorMultiply(int* inputVector)
{

	int* outputVector = new int[n];

	for (int i = 0; i < n; i++)
		outputVector[i] = 0;

	int sum = 0;
	int start, end;
	for (int i = 0; i < n; i++)
	{
		sum = 0;
		start = rowPtr[i];
		if ((i + 1) == n)
			end = nonZeros;
		else
			end = rowPtr[i + 1];

		for (int j = start; j < end; j++)
		{
			sum = sum + (values[j] * inputVector[colPos[j]]);
		}
		outputVector[i] = sum;
	}
	return outputVector;
}

int main() {

	CSR* A;
	CSR* B;
	int* aVector;
	int numRows, numColumns, numNonZeros;
	int row, col, value;

	//read in the first matrix
	cin >> numRows >> numColumns;
	cin >> numNonZeros;

	A = new CSR(numRows, numColumns, numNonZeros);
	for (int i = 0; i < numNonZeros; i++) {
		cin >> row >> col >> value;
		(*A).addToCSR(row, col, value); //Calling the addToCSR() method I made to clean up the code for my purposes
	}
	(*A).display();


	CSR* C = new CSR(*A); //calling the copy constructor
	(*C).display();

	//read in the second matrix which is similar to the first into the CSR pointer object B and display; Write code for this

	cin >> numRows >> numColumns;
	cin >> numNonZeros;

	B = new CSR(numRows, numColumns, numNonZeros);
	for (int i = 0; i < numNonZeros; i++) {
		cin >> row >> col >> value;
		(*B).addToCSR(row, col, value); //Calling the addToCSR() method I made to clean up the code for my purposes
	}
	(*B).display();


	//read in the vector
	cin >> numColumns;
	numColumns = 4;
	//
	aVector = new int[numColumns];
	cout << endl << "Vector:" << endl;
	for (int i = 0; i < numColumns; i++)
	{
		cin >> aVector[i];
	}


	cout << endl << "A*vector:" << endl;
	//Matrix-Vector Multiplication
	int* resultVector = (*A).matrixVectorMultiply(aVector);
	for (int i = 0; i < (*A).getNumRows(); i++)
		cout << resultVector[i] << " ";
	cout << endl;
	//Matrix-Matrix Multiplication
	CSR* resultMatrix = (*C).matrixMultiply(*B);
	(*resultMatrix).display();


	// Call the destructors
	delete[] aVector;
	delete[] resultVector;
	delete A;
	delete B;
	delete C;
	delete resultMatrix;


	return 0;
}