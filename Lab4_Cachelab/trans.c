/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void trans(int M, int N, int A[N][M], int B[M][N]);	// Basic transpose function
void zigzag_transpose(int N_start, int M_start,int M, int N, int A[N][M], int B[M][N], int block);
void case_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    case_transpose(M,N,A,B);
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/* 
 * trans_c - A simple collumn-wise transpose function, not optimized for the cache
 */
char trans_c_desc[] = "Simple collumn-wise scan transpose";
void trans_c(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            tmp = A[j][i];
            B[i][j] = tmp;
        }
    }    

}

/* 
 * trans_d - A diagonal zigzag transpose function
 */
char trans_d_desc[] = "Diagonal zigzag scan transpose";
void trans_d(int M, int N, int A[N][M], int B[M][N])
{
    int i = 0, j = 0;
    int dir = 1;   // 1 = down-left, 0 = up-right
    int flag = 0;
    int tmp;

    while (!flag)
    {
	tmp = A[i][j];
        B[j][i] = tmp;

        if (dir == 1)    // down-left
        {
            if (i == N - 1) {
                j++;         // move right
                dir = 0;     // switch
            }
            else if (j == 0) {
                i++;         // move down
                dir = 0;     // switch
            }
            else {
                i++;         // move down-left
                j--;
            }
        }
        else              // up-right
        {
            if (j == M - 1) {
                i++;         // move down
                dir = 1;     // switch
            }
            else if (i == 0) {
                j++;         // move right
                dir = 1;     // switch
            }
            else {
                i--;         // move up-right
                j++;
            }
        }

        if ((i == N - 1) && (j == M - 1))
            flag = 1;
    }
    B[j][i] = A[i][j];
}


/*
*	function block_trans
*	basic blocking function made to run with matricies that are divisible by block
*	block size defined as a macro for testing
*/
char block_trans_desc[] = "block based transpose function";
void block_trans(int M, int N, int A[N][M], int B[M][N])
{
    const int block = BLOCK;
    int i, j, ii, jj;
    for (i = 0; i < N; i += block)
    {
	for (j = 0; j < M; j += block)
	{
	    for (ii = i; ii < i + block; ii++)
	    {
		for (jj = j; jj < j + block; jj++)
		{
		    B[jj][ii] = A[ii][jj];
		}
	    }
	}
    }
}

/*
*	function block_asym_trans
*	basic blocking row-wise function made to run with asymetric matricies with any matrix size
*	best results found when block = 4
*/
char block_asym_trans_desc[] = "block based transpose function";
void block_asym_trans(int M, int N, int A[N][M], int B[M][N])
{
    const int block = 4;
    int i, j, ii, jj;
    int M_tail = M % block;
    int N_tail = N % block;
    M -= M_tail;
    N-= N_tail;
    
    // transpose blocks that divide evenly
    for (i = 0; i < N; i += block)
    {
	for (j = 0; j < M; j += block)
	{
	    for (ii = i; ii < i + block; ii++)
	    {
		for (jj = j; jj < j + block; jj++)
		{
		    B[jj][ii] = A[ii][jj];
		}
	    }
	}
    }

    // transpose tail manually
    for (i = N; i < N+N_tail; i++)
    {
	for (j = 0; j < M+M_tail; j++)
	{
	    B[j][i] = A[i][j];
	}
    }
    for (j = M; j < M+M_tail; j++)
    {
	for (i = 0; i < N+N_tail; i++)
	{
	    B[j][i] = A[i][j];
	}
    }
}




/*
*	function block_diag_trans
*	asymetric blocking function that uses zigzag access
*	block size defined as a macro for testing
*/
char block_zigzag_trans_desc[] = "block based zigzag access transpose function";
void block_zigzag_trans(int M, int N, int A[N][M], int B[M][N])
{
    const int block = BLOCK;
    int i, j;
    int M_tail = M % block;
    int N_tail = N % block;
    M -= M_tail;
    N-= N_tail;
    
    // transpose blocks that divide evenly
    for (i = 0; i < N; i += block)
    {
	for (j = 0; j < M; j += block)
	{
	    zigzag_transpose(i,j,M,N,A,B,block);
	}
    }

    // transpose the tail manually
    for (i = N; i < N+N_tail; i++)
    {
	for (j = 0; j < M+M_tail; j++)
	{
	    B[j][i] = A[i][j];
	}
    }
    for (j = M; j < M+M_tail; j++)
    {
	for (i = 0; i < N+N_tail; i++)
	{
	    B[j][i] = A[i][j];
	}
    }
}

/*
*	function zigzag_transpose
*	transpose function that is given a block start and block size and transposes using zigzag access
*/
void zigzag_transpose(int N_start, int M_start, int M, int N, int A[N][M], int B[M][N], int block)
{
    int i = N_start;
    int j = M_start;
    int dir = 1;   // 1 = down-left, 0 = up-right
    int flag = 0;
    int tmp;

    while (!flag)
    {
	tmp = A[i][j];
	B[j][i] = tmp;

        if (dir == 1)    // down-left
        {
            if (i == N_start + block - 1) {
                j++;         // move right
                dir = 0;     // switch
            }
            else if (j == M_start) {
                i++;         // move down
                dir = 0;     // switch
            }
            else {
                i++;         // move down-left
                j--;
            }
        }
        else              // up-right
        {
            if (j == M_start + block - 1) {
                i++;         // move down
                dir = 1;     // switch
            }
            else if (i == N_start) {
                j++;         // move right
                dir = 1;     // switch
            }
            else {
                i--;         // move up-right
                j++;
            }
        }

        if ((i == N_start + block - 1) && (j == M_start + block - 1))
            flag = 1;
    }
    B[j][i] = A[i][j];
}


/*
*	function block_zigzag_trans_2
*	blocking zigzag access function made to run with asymetric matricies with any matrix size
*	best results found when block = 4
*/
char block_zigzag_trans_2_desc[] = "block based zigzag access transpose function";
void block_zigzag_trans_2(int M, int N, int A[N][M], int B[M][N])
{
    const int block_i = 8;
    const int block_j = 8;
    int i = 0;
    int j = 0;
    int	ii, jj;
    int M_tail = M % block_j;
    int N_tail = N % block_i;
    M -= M_tail;
    N-= N_tail;

    int dir = 1;   // 1 = up-right, 0 = down-left
    int flag = 0;
    int tmp;

    while (!flag)
    {
	for (ii = i; ii < i + block_i; ii++)
	{
	    for (jj = j; jj < j + block_j; jj++)
	    {
		tmp = A[ii][jj];
		B[jj][ii] = tmp;
	    }
	}

        if (dir == 1)    // up-right
        {
            if (i == N - block_i) {
                j += block_j;         // move down
                dir = 0;     // switch
            }
            else if (j == 0) {
                i += block_i;         // move right
                dir = 0;     // switch
            }
            else {
                i += block_i;         // move up-right
                j -= block_j;
            }
        }
        else              // down-left
        {
            if (j == M - block_j) {
                i += block_i;         // move right
                dir = 1;     // switch
            }
            else if (i == 0) {
                j += block_j;         // move down
                dir = 1;     // switch
            }
            else {
                i -= block_i;         // move down-left
                j += block_j;
            }
        }

        if ((i == N - block_i) && (j == M - block_j))
            flag = 1;
    }

    // fill in final block
    for (ii = i; ii < i + block_i; ii++)
    {
	for (jj = j; jj < j + block_j; jj++)
	{
	    tmp = A[ii][jj];
	    B[jj][ii] = tmp;
	}
    } 

    // transpose tail manually
    for (i = N; i < N+N_tail; i++)
    {
	for (j = 0; j < M+M_tail; j++)
	{
	    tmp = A[i][j];
	    B[j][i] = tmp;
	}
    }
    for (j = M; j < M+M_tail; j++)
    {
	for (i = 0; i < N; i++)
	{
	    tmp = A[i][j];
	    B[j][i] = tmp;
	}
    }
}


/*
*	function block_snake_trans
*	basic blocking snake function made to run with asymetric matricies with any matrix size
*	best results found when block = 8
*/
char block_snake_trans_desc[] = "block based transpose function";
void block_snake_trans(int M, int N, int A[N][M], int B[M][N])
{
    const int block_i = 4;
    const int block_j = 4;
    int i, j, ii, jj, tmp;
    int M_tail = M % block_i;
    int N_tail = N % block_j;
    M -= M_tail;
    N-= N_tail;
    
    // transpose blocks that divide evenly
    for (i = 0; i < N; i += block_i)
    {
	for (j = 0; j < M; j += block_j)
	{
	    for (ii = i; ii < i + block_i; ii++)
	    {
		if (ii % 2) // if odd row, go backwards
		{
		    for (jj = j + block_j - 1; jj >= j; jj--)
		    {
			tmp = A[ii][jj];
			B[jj][ii] = tmp;
		    }
		}
		else // if even row, go forwards
		{
		    for (jj = j; jj < j + block_j; jj++)
		    {
			tmp = A[ii][jj];
			B[jj][ii] = tmp;
		    }
		}
	    }
	}
    }

    // transpose tail manually
    for (i = N; i < N+N_tail; i++)
    {
	for (j = 0; j < M+M_tail; j++)
	{
	    tmp = A[i][j];
	    B[j][i] = tmp;
	}
    }
    for (j = M; j < M+M_tail; j++)
    {
	for (i = 0; i < N; i++)
	{
	    tmp = A[i][j];
	    B[j][i] = tmp;
	}
    }
}



/*
*	function block_square_diag_trans
*	basic blocking snaking function that uses special handling for the diagonal blocks. Only works with square block sizes
*	still works for asymetric matricies
*/
char block_square_diag_trans_desc[] = "block based diagonal handling transpose function";
void block_square_diag_trans(int M, int N, int A[N][M], int B[M][N])
{
    // choose block size
    int b;
    if ( M == N && M == 32)
    {
	b = 8;
    }
    else
    {
	b = 4;
    }
    const int block = b;
    int i, j, ii, jj;
    int M_tail = M % block;
    int N_tail = N % block;
    M -= M_tail;
    N -= N_tail;
    if( M > N) // create a square submatrix
    {
	M_tail += (M - N);
	M -= (M - N);
    }
    if( N > M)
    {
	N_tail += (N - M);
	N -= (N - M);
    }
    
    // transpose blocks that divide evenly
    for (i = 0; i < N; i += block)
    {
	for (j = 0; j < M; j += block)
	{
	    if (i != j) // transpose non-diagonal blocks first
	    {
		for (ii = i; ii < i + block; ii++)
            	{
                    if (ii % 2) // if odd row, go backwards
		    {
		    	for (jj = j + block - 1; jj >= j; jj--)
		    	{
			    int tmp = A[ii][jj];
			    B[jj][ii] = tmp;
		    	}
		    }
		    else // if even row, go forwards
		    {
		    	for (jj = j; jj < j + block; jj++)
		    	{
			    int tmp = A[ii][jj];
			    B[jj][ii] = tmp;
		    	}
		    }
                }
	    }
	}
    }
    
    
    int temp[block][block];
    // transpose the diagonal blocks in two steps
    for (i = 0; i < N; i += block) 
    {
        // Step A -> temp
        for (ii = 0; ii < block && (i + ii) < N; ii++) {
            for (jj = 0; jj < block && (i + jj) < M; jj++) {
                temp[jj][ii] = A[i + ii][i + jj];   // transpose into temp
            }
        }

        // Copy temp -> B
        for (ii = 0; ii < block && (i + ii) < N; ii++) {
            for (jj = 0; jj < block && (i + jj) < N; jj++) {
                B[i + jj][i + ii] = temp[jj][ii];    // write from temp
            }
        }
    }



    // transpose tail manually
    for (i = N; i < N+N_tail; i++)
    {
	for (j = 0; j < M+M_tail; j++)
	{
	    B[j][i] = A[i][j];
	}
    }
    for (j = M; j < M+M_tail; j++)
    {
	for (i = 0; i < N; i++)
	{
	    B[j][i] = A[i][j];
	}
    }
}



/*
*	function case_transpose
*	a blocking function that takes each submission case and uses the optimal algorithm to get the transpose 
*	For the 64x64 case the registers are loaded 8 at a time and then stored 8 at a time
*	best results found when block = 8 for 32x32 and 64x64
*/
#define BLOCK_32 8
#define BLOCK_64 8
#define BLOCK_GENERIC 16

char case_transpose_desc[] = "GPT Generated Transpose";

void case_transpose(int M, int N, int A[N][M], int B[M][N])
{
    // -----------------------------
    // Case 1: 32x32
    // -----------------------------
    if (M == 32 && N == 32) {
        for (int ii = 0; ii < N; ii += BLOCK_32) {
            for (int jj = 0; jj < M; jj += BLOCK_32) {
                for (int i = ii; i < ii + BLOCK_32; i++) {
                    int diag = -1;
                    int diag_val = 0;
                    for (int j = jj; j < jj + BLOCK_32; j++) {
                        if (i == j) {
                            diag = j;
                            diag_val = A[i][j];
                        } else {
                            B[j][i] = A[i][j];
                        }
                    }
                    if (diag != -1) {
                        B[diag][diag] = diag_val;
                    }
                }
            }
        }
        return;
    }

    // -----------------------------
    // Case 2: 64x64
    // -----------------------------
    if (M == 64 && N == 64) {
	for (int ii = 0; ii < 64; ii += BLOCK_64) {
            for (int jj = 0; jj < 64; jj += BLOCK_64) {
	        
		int a0, a1, a2, a3, a4, a5, a6, a7; // 8 Local variables
			// load in 8 values at a time
		for (int i=0; i<4; i++) {
		    a0 = A[ii+i][jj+0];
		    a1 = A[ii+i][jj+1];
		    a2 = A[ii+i][jj+2];
		    a3 = A[ii+i][jj+3];
		    a4 = A[ii+i][jj+4];
		    a5 = A[ii+i][jj+5];
		    a6 = A[ii+i][jj+6];
		    a7 = A[ii+i][jj+7];
			// Then write the 8 values at the same time
		    B[jj+0][ii+i] = a0;
		    B[jj+1][ii+i] = a1;
		    B[jj+2][ii+i] = a2;
		    B[jj+3][ii+i] = a3;

		    B[jj+0][ii+i+4] = a4;
		    B[jj+1][ii+i+4] = a5;
		    B[jj+2][ii+i+4] = a6;
		    B[jj+3][ii+i+4] = a7;

		}

		for (int j = 0; j < 4; j++) {
		    a0 = A[ii+4][jj+j];
		    a1 = A[ii+5][jj+j];
		    a2 = A[ii+6][jj+j];
		    a3 = A[ii+7][jj+j];

		    int b0, b1, b2, b3; // 4 Local variables

		    b0 =B[jj+j][ii+4];
		    b1 =B[jj+j][ii+5];
		    b2 =B[jj+j][ii+6];
		    b3 =B[jj+j][ii+7];

		    B[jj+j][ii+4] = a0;
		    B[jj+j][ii+5] = a1;
		    B[jj+j][ii+6] = a2;
		    B[jj+j][ii+7] = a3;

		    B[jj+j+4][ii+0] = b0;
		    B[jj+j+4][ii+1] = b1;
		    B[jj+j+4][ii+2] = b2;
		    B[jj+j+4][ii+3] = b3;
		}

		for (int i = 4; i < 8; i++) {
		    a0 = A[ii+i][jj+4];
		    a1 = A[ii+i][jj+5];
		    a2 = A[ii+i][jj+6];
		    a3 = A[ii+i][jj+7];

		    B[jj+4][ii+i] = a0;
		    B[jj+5][ii+i] = a1;
		    B[jj+6][ii+i] = a2;
		    B[jj+7][ii+i] = a3;

		}
            }
	}

        return;
    }

    // -----------------------------
    // Case 3: Generic sizes (e.g., 61x67)
    // -----------------------------
    for (int ii = 0; ii < N; ii += BLOCK_GENERIC) {
        for (int jj = 0; jj < M; jj += BLOCK_GENERIC) {
            int i_end = (ii + BLOCK_GENERIC < N) ? ii + BLOCK_GENERIC : N;
            int j_end = (jj + BLOCK_GENERIC < M) ? jj + BLOCK_GENERIC : M;
            for (int i = ii; i < i_end; i++) {
                for (int j = jj; j < j_end; j++) {
                    B[j][i] = A[i][j];
                }
            }
        }
    }
}










/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions(void)
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    //registerTransFunction(block_trans, block_trans_desc);
    registerTransFunction(block_asym_trans, block_asym_trans_desc);
    registerTransFunction(block_square_diag_trans, block_square_diag_trans_desc);
    registerTransFunction(case_transpose, case_transpose_desc);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}   
