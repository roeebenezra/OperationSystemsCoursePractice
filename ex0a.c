/*=============================ex0===============================
 * Name: Roee Ben Ezra
 * Id: 206123994
 * 
 * summery:
 this program read from file data of dynamic 2d array and save it 
 to Data struct
 The program finds maximum Row that each value in this row is maximum
 in his column, if there's such a line, her index prints to output file
 if there isn't, (-1) prints to output file. 

==============================================================*/

//========================include section=======================
#include <stdio.h>
#include <stdlib.h>

//============================ structs ==================================

struct Data{
    int _num_of_lines;
    int **_the_data;
    int *_lines_len;
};
//========================prototype declaration===========================

void read_data(struct Data* data, FILE* fp);
void read_new_line(FILE* ptr, struct Data* data, int line_len, int lines);
void double_lines_size(struct Data *data, int alloc_lines, int lines);
int* alloc_arr(int size);
int** alloc_2d_arr();
int find_maximum_line(const struct Data* data);
void find_max_cols_values(const struct Data* data, int* arr, const int size);
void progress_row_index(const struct Data* data, int row,const int col);
int biggest_line_len(const int arr[], const int size);
void free_arr(struct Data* data);

//=============================================================================

int main(int argc , char* argv[])
{
    if(argc != 3){
        printf("wrong arguments number, usage: ./<file name> file1 file2\n");
        exit(1);
    }

    struct Data data ;
    FILE* fp;

    fp = fopen(argv[1],"r");
    read_data(&data, fp);
   
    fp = fopen(argv[2],"w");
    fprintf(fp,"%d",find_maximum_line(&data));
    
    free_arr(&data);
    fclose(fp);

    return EXIT_SUCCESS;
}

//=============================================================================
//main_function - finds max line
int find_maximum_line(const struct Data* data) // 2*n^2+n = O(n^2)
{
    int row,col ,counter = 0; 
    //biggest row variable to define size for the arr, biggest_line_len()-O(n)
    int BiggestRow = biggest_line_len(data->_lines_len, data->_num_of_lines);
    
    int max_values[BiggestRow] ; // highest values in each col

	//func to find maximum value in each col,
	// returns array with this values(O(n^2))
    find_max_cols_values(data, max_values, BiggestRow);	
    
    //double for loops to search in _the_data array max values (O(n^2))
    for(row = 0; row < data->_num_of_lines ; row++){
        for(col = 0, counter = 0 ; col < data->_lines_len[row] ; col++){
            if(data->_the_data[row][col] == max_values[col])
                counter++;
            if(counter == data->_lines_len[row]) //using counter in each row
                return row;
        }
    }
    return -1;
}

//=============================================================================
 //func to find maximum values in each row and save them in arr.
void find_max_cols_values(const struct Data* data, int arr[] ,const int size)
{
    int row ,col, max;

    for(col = 0 ; col < size ; col++)
    { 
        row = 0 ;
        progress_row_index(data,row,col); //
        max = data->_the_data[row][col];
        for( ; row < data->_num_of_lines  ; row++)
        {
            progress_row_index(data,row,col);
            if(row == data->_num_of_lines)
                break;
            if(data->_the_data[row][col] > max)
                max = data->_the_data[row][col];
        }
        arr[col] = max;
    }
}
//=============================================================================
//function to progress row index when this [row][col] 
// cell doesn't exists in _the_data array
void progress_row_index(const struct Data* data, int row,const int col)
{
    while(col > data->_lines_len[row] && row < data->_num_of_lines)
        row++;        //continue while cells not exists in arr
}

//=============================================================================
//func to find biggest line length
int biggest_line_len(const int arr[], const int size)
{
    int i ,max_len = 0;

    for(i = 0 ; i < size ; i++){
        if(arr[i] > max_len)
            max_len = arr[i];
    }
    return max_len;
}

//=============================================================================
//read data to data struct from input file
void read_data(struct Data* data, FILE* fp)
{
    int line_len, alloc_lines = 1;
    data->_the_data = NULL;
    data->_num_of_lines = 0;

    while (!feof(fp)){
        fscanf(fp, "%d", &line_len);  //read line len from inp file
        if(data->_the_data == NULL)   //first case
        {
            data->_the_data = alloc_2d_arr();
            data->_the_data[data->_num_of_lines] = alloc_arr(line_len);
            data->_lines_len = alloc_arr(alloc_lines);//alloc_lines = 1 here.
        }
        if(alloc_lines == data->_num_of_lines)//if lines capasity is full
            double_lines_size(data,alloc_lines, data->_num_of_lines);
        
        read_new_line(fp, data, line_len, data->_num_of_lines);
        data->_num_of_lines++ ;	//lines count
    }
    if(alloc_lines > data->_num_of_lines)//re-allocate to correct lines sizes
    {
        data->_the_data = (int **)realloc(data->_the_data, sizeof(int*)
							* alloc_lines-(alloc_lines - data->_num_of_lines));
										
        data->_lines_len = (int *)realloc(data->_lines_len, sizeof(int)
							* alloc_lines-(alloc_lines - data->_num_of_lines)); 
    }
}

//=============================================================================
//read new line to _the_data arr and saving in _line_len the line size
void read_new_line(FILE* ptr, struct Data *data, int line_len, int lines)
{
    int  i;
    data->_the_data[lines] = alloc_arr(line_len);//allocate new line
    for (i = 0 ; i < line_len ; i++)
    {
        fscanf(ptr, "%d", &data->_the_data[lines][i]);//read from inp file 
    }
    data->_lines_len[lines] = line_len ;
}
//=============================================================================
//allocate arr function
int* alloc_arr(int size)
{
    int *arr;
    arr = (int *)malloc(sizeof(int) * size);
    if(arr == NULL)
    {
        perror("cannot allocate 1d arr\n");
        exit(EXIT_FAILURE);
    }
    return arr; 
}
//=============================================================================
//allocate 2d arr function - to allocate data._the_data
int** alloc_2d_arr()
{
    int **arr;
    arr = (int **)malloc(sizeof(int*));
    if(arr == NULL){
        perror("cannot allocate 2d arr\n");
        exit(EXIT_FAILURE);
    }
    return arr;
}
//=============================================================================
//function to double _the_data line and reallocate
void double_lines_size(struct Data *data, int alloc_lines, int lines)
{
    alloc_lines = 2 * alloc_lines ;
    data->_the_data = (int**)realloc(data->_the_data, alloc_lines*sizeof(int*));
    data->_lines_len = (int*)realloc(data->_lines_len,alloc_lines*sizeof(int));
}
//=============================================================================
//free data dynamic arrays
void free_arr(struct Data* data)
{
    int  i;
    for(i = 0 ; i < data->_num_of_lines ; i++)
        free(data->_the_data[i]);
    free(data->_the_data);   
    free(data->_lines_len);
}
