// matheus felipe rodrigues - 93943

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define N 2048
#define AMNT_GEN 2000
#define THREADS 8
int res; // global variable

// headers
int **CreateBoard(int m, int n);                    // create MxN board
void FreeBoard(int **grid, int n);                  // free board
void GliderRPentomino(int **grid);                  // glider/rpentomino on board
int LiveCells(int **grid, int n);                   // amount of alive cells
void ShowBoard(int **grid, int n, int amnt);        // show current board status
int GetNeighbors( int **grid, int i, int j, int n); // amount alive neighbors
void NewGen(int **grid, int **grid_next, int n);    // draw next board

// funcs
// createboard
int **CreateBoard(int m, int n){
    int **M;
    int i;

    // mem
    M = (int **)malloc(sizeof(int *) * m);
    if (M == NULL){ // validation
        printf("OUT OF MEMORY");
        exit(1);
    }

    for (i = 0; i < m; i++){
        // mem
        M[i] = (int *)malloc(sizeof(int) * n);
        if(M[i] == NULL){ // validation
            printf("OUT OF MEMORY");
            exit(1);
        }
    }

    return M;
}

// freeboard
void FreeBoard(int **grid, int n){
    int i;
    for(i = 0; i < n; i++) free(grid[i]);
    free(grid);
}

// gliderrpentomino
void GliderRPentomino(int **grid){
    // glider
    int lin = 1, col = 1;
    grid[lin  ][col+1] = 1;
    grid[lin+1][col+2] = 1;
    grid[lin+2][col  ] = 1;
    grid[lin+2][col+1] = 1;
    grid[lin+2][col+2] = 1;

    // r-pentomino
    lin = 10; col = 30;
    grid[lin  ][col+1] = 1;
    grid[lin  ][col+2] = 1;
    grid[lin+1][col  ] = 1;
    grid[lin+1][col+1] = 1;
    grid[lin+2][col+1] = 1;
}

// livecells
int LiveCells(int **grid, int n){
    int i, j;
    res = 0;
    #pragma omp parallel private(j)
    {
        #pragma omp for
        for(i = 0; i < n; i++){
            for (j = 0; j < n; j++){
                #pragma omp critical
                res += grid[i][j];
            }
        }
    }
    
    return res;
}

// showboard
void ShowBoard(int **grid, int n, int amnt){
    int i, j;
    if(amnt < 6){
        printf("\n");
        printf("------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        for(i = 0; i < n; i++){
            for(j = 0; j < n; j++){
                if(grid[i][j] == 0){
                    printf("[ ]", grid[i][j]);
                }else if(grid[i][j] == 1){
                    printf("[%d]",grid[i][j]);
                }else{
                    printf("[ ]",grid[i][j]);
                }
            }
            printf("\n");
            printf("------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        }
    }
}

// getneighbors
int GetNeighbors(int **grid, int i, int j, int n){
    int amnt_neighbors = 0;
    int sup, inf, left, right;
    
    sup = i - 1;
    inf = i + 1;
    left = j - 1;
    right = j + 1;

    if(i == 0)      sup = n - 1;
    if(i == n - 1)  inf = 0;
    if(j == 0)      left = n - 1;
    if(j == n - 1)  right = 0;

    if(grid[sup][left]  == 1)   amnt_neighbors++;
    if(grid[sup][j]     == 1)   amnt_neighbors++;
    if(grid[sup][right] == 1)   amnt_neighbors++;
    if(grid[i][left]    == 1)   amnt_neighbors++;
    if(grid[i][right]   == 1)   amnt_neighbors++;
    if(grid[inf][left]  == 1)   amnt_neighbors++;
    if(grid[inf][j]     == 1)   amnt_neighbors++;
    if(grid[inf][right] == 1)   amnt_neighbors++;

    return amnt_neighbors;
}

// newgen
void NewGen(int **grid, int **grid_next, int n){
    int i, j;
    int amnt_alive_neighboors;

    #pragma omp parallel private(j, amnt_alive_neighboors)
    {
        #pragma omp for
        for(i = 0; i < n; i++){
            for(j = 0; j < n; j++){
                amnt_alive_neighboors = GetNeighbors(grid, i, j, n);

                if(grid[i][j] == 1){
                    if(amnt_alive_neighboors <  2) grid_next[i][j] = 0;
                    if(amnt_alive_neighboors == 2) grid_next[i][j] = 1;
                    if(amnt_alive_neighboors == 3) grid_next[i][j] = 1;
                    if(amnt_alive_neighboors >= 4) grid_next[i][j] = 0;
                }else{
                    if(amnt_alive_neighboors == 3) grid_next[i][j] = 1;
                    else grid_next[i][j] = 0;
                }
            }
        }
    }
}

int main(){
    long int i, j, amnt_cells;
    int **grid;                         // current board
    int **grid_next;                    // nextgen board
    int **grid_swap;                    // swap board
    struct timeval init_time;           // init time
    struct timeval fin_time;            // fin time
    struct timeval init_time_parallel;  // init time parallel
    struct timeval fin_time_parallel;   // fin time parallel


    gettimeofday(&init_time, NULL); // start time
    omp_set_num_threads(THREADS);   // set threads

    grid = CreateBoard(N, N);       //create current board
    grid_next = CreateBoard(N, N);  // create nextgen board
    GliderRPentomino(grid);

    gettimeofday(&init_time_parallel, NULL); // start time parallel
    printf("\n Cond 0: %d", LiveCells(grid, N));
    //ShowBoard(grid, 50); 
    for(i = 0; i < AMNT_GEN; i++){
        ShowBoard(grid, 50, i); 
        NewGen(grid, grid_next, N);
        grid_swap = grid;
        grid = grid_next;
        grid_next = grid_swap;

        amnt_cells = LiveCells(grid, N);
        printf("\n Gen %d: %d", i + 1, amnt_cells);
    }
    //ShowBoard(grid, 50);
    gettimeofday(&fin_time_parallel, NULL);

    FreeBoard(grid, N);
    FreeBoard(grid_next, N);

    gettimeofday(&fin_time, NULL);
    printf("\nSeconds: %ld", fin_time.tv_sec - init_time.tv_sec);
    //printf("\nmicroseconds: %ld", fin_time.tv_usec - init_time.tv_usec);

    printf("\nSeconds (parallel): %ld", fin_time_parallel.tv_sec - init_time_parallel.tv_sec);
}