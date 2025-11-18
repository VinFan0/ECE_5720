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

#define BLOCK 4

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void trans(int M, int N, int A[N][M], int B[M][N]);	// Basic transpose function
void zigzag_transpose(int N_start, int M_start,int M, int N, int A[N][M], int B[M][N], int block);


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
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
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
    const int block_i = 4;
    const int block_j = 4;
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
    registerTransFunction(block_zigzag_trans_2, block_zigzag_trans_2_desc);
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
