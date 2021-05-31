#include <stdio.h>

#define ARRAY_SIZE(arr) ((sizeof(arr)/ sizeof(arr[0])))

typedef enum {FALSE, TRUE} BOOL;

int * ret = 0;

int menu(){

    int ret = 0;

    BOOL pass = TRUE;

    do{
        printf("*************************************************\n* 1. Two-Sum Function\t\t\t\t*"
        "\n* 2. Add Two Number Function\t\t\t*\n"
        "*************************************************\n");

        scanf("%d", &ret);  // User input

        if (ret < 3 && ret >= 1) {
            pass = FALSE;
        }else{
            pass = TRUE;
        }

    }while(pass);    

    printf("------x------x------x------x------x------x-------\n\n");

    return ret;
}

/*
*   This function is let user to give an target value and the function would find two values inside
*   existing array of numbers and sum them up. Then, the function would return the index number of 
*   those two existing number inside the array. User have to enter two parameters which are one array
*   of random numbers and a target number. 
*/

int * twoSum(int * arr, int target, size_t size){

    int temp[2];
    int f_index = 0;      // index number to find the first number to sum
    int s_index = 0;      // second index number to find the second number to sum

    while(f_index != size){

        int first_num = *(arr + f_index);       // find the first number

        s_index = f_index+1;                    // second number index

        // second number to find the sum
        while(s_index != size){

            int sec_num = *(arr + s_index);     // find the second number

            if((first_num + sec_num) == target){

                temp[0] = f_index;
                temp[1] = s_index;

                ret = temp;
                
                return ret;
            }

            s_index++;
        }

        f_index++;
    }


    return ret;
}



/********************************* Helper Functions *************************************/

void print_array(int * arr, size_t size){

    printf("{");

    for (auto i : arr){

        printf(" %d,", i);
    }

    printf("}");
}



int main(){

    int arr[10] = {4, 5, 6, 7, 9, 1, 62, 3, 11, 15};



    int entry = 0;          // users entry


    switch(menu()){

        case 1: 
                printf("The given array values = ");
                printf("\nPlease enter the target value: ");

                scanf("%d", &entry);

                int * indexes = twoSum(arr, entry, ARRAY_SIZE(arr));

                printf("Two Indexes = {%d, %d}.\n", *(indexes+0), *(indexes+1));

            break;

        case 2:

            break;

        default:
    }

    return 0;
}