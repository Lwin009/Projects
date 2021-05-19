#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PROC_0 0
#define PROC_1 1
#define ARG_COUNT 8
#define FILE_EXTEN_OFFSET 6
#define BYTE 1024
#define GETBIT(bit) (round(log(bit) / log(2)))
#define GET_PAGE_NUM(hex,pgsize, mask) ( (hex - (hex & (pgsize-1) )) >> mask)
#define STR_TO_HEX(hexs) (strtoul(hexs, NULL, 16))
#define GET_FRAME_1(nf, x, y) ((nf * y) / (x + y))
#define GET_FRAME_0(nf, siz) (nf - siz)

using namespace std;

////////////////// Struct START /////////////////////////////

struct process
{
    unsigned long int address;
    bool reference;
    bool dirty;
};

struct monitor
{
    unsigned long int pagefault;
    unsigned long int disk_write;
    unsigned int refe_pointer;
    unsigned int arr_SIZE;
    unsigned int element_access;
};

unsigned long int num_frame = 0;
unsigned long int page_size = 0;
unsigned int p0_frames = 0;
unsigned int p1_frames = 0;
const char *file_name;

struct monitor monitor_0;
struct monitor monitor_1;

///////////////// Struct END /////////////////////////////////

///////////////// Pre-Function ///////////////////////////////

int check_arguments(int, char const **);
void initialize();
void display();
int search (struct process arr[], struct process, int);
void process_0_handler(struct process arr[], struct process);
void process_1_handler(struct process arr[], struct process);

///////////////// Pre-Function END ///////////////////////////

/*********************************************************
 * Main function  
 * ******************************************************/
int main(int argc, char const *argv[])
{
    if (check_arguments(argc, argv) < 0){    
        cout << "Error: Invalid arguments...: "<< endl;    
        return -1;
    }
    initialize();   // initialize the global data field
    struct process proc_0[p0_frames];
    struct process  proc_1[p1_frames];
    unsigned int mask = GETBIT(page_size);    
    
    FILE *file;

    file = fopen(file_name, "r");

    while (!feof(file))
    {
      
        unsigned int addr = 0;
        char mode = '-';
        int p_ID = -1;

        fscanf(file,"%c %x %i ", &mode, &addr, &p_ID );

        struct process temp;
        temp.address = GET_PAGE_NUM(addr, page_size, mask);
        temp.reference = false;

        if (mode == 's') temp.dirty = true;
        if (mode == 'l') temp.dirty = false;

        if (p_ID == 0)  process_0_handler(proc_0, temp);
        if (p_ID == 1)  process_1_handler(proc_1, temp);
    }
   
    fclose(file);
    display();
    return 0;
}

/*********************************************************************
 * Display the result of Virtual Memory Management
 * 
 * ******************************************************************/
void 
display(){
    cout << "Algorithm: Second Chance\n"
        << "Number of frames: " << num_frame << "\n"
        << "Page size: " << (page_size / BYTE) << " KB\n"
        << "Total memory accesses: " << monitor_0.element_access + monitor_1.element_access<< "\n"
        << "Total page faults: " << monitor_0.pagefault + monitor_1.pagefault << "\n"
        << "Total writes to disk: " << monitor_0.disk_write + monitor_1.disk_write << endl;
}

/**********************************************************************************
 * This method handle the process number zero. It adds the given process address
 * into the table (Array). This method handle the address with the Second Chance
 * algorithm. 
 * 
 * @param arr the table (array)
 * @param p   the new process: address, dirty, and reference 
 * *******************************************************************************/
void
process_0_handler( struct process arr[], struct process p)
{    
    monitor_0.element_access++;
    
    // Searh if the new process is alreay in the array
    int at = search(arr, p, monitor_0.arr_SIZE);

    if(monitor_0.arr_SIZE < p0_frames){    
        // Array not full: there are still empty spots
        // Go fill them 

        if (at < 0)
        {   // Item not found
            arr[monitor_0.arr_SIZE] = p;
            monitor_0.pagefault++;
            monitor_0.arr_SIZE++;            
        }else{            
            // Item found it
            if(p.dirty == true) arr[at].dirty = true;
            arr[at].reference = true;            
        }

    }else{
        //Array full
        if (at < 0)
        {
            // Item Not found
            if(monitor_0.refe_pointer == (p0_frames)) monitor_0.refe_pointer = 0;

            unsigned int i;
            for( i = monitor_0.refe_pointer; i < monitor_0.arr_SIZE; i++){

                if (arr[i].reference == true){
                    arr[i].reference = false;
                }else{
                    
                    if(arr[i].dirty == true) monitor_0.disk_write++;
                    monitor_0.pagefault++;
                    arr[i] = p;
                    monitor_0.refe_pointer = i+1;
                    goto done;
                }
            }

            for (i = 0; i < monitor_0.arr_SIZE; i++)
            {
                if (arr[i].reference == true){
                    arr[i].reference = false;
                }else{
                    
                    if(arr[i].dirty == true) monitor_0.disk_write++;
                    monitor_0.pagefault++;
                    arr[i] = p;
                    monitor_0.refe_pointer = i+1;
                    break;
                }
            }
            done: return;

        }else{
            // Item found it
            if(p.dirty == true) arr[at].dirty = true;
            arr[at].reference = true;
        }
    }
}

/**********************************************************************************
 * This method handle the process number one. It adds the given process address
 * into the table (Array). This method handle the address with the Second Chance
 * algorithm. 
 * 
 * @param arr the table (array)
 * @param p   the new process: address, dirty, and reference 
 * *******************************************************************************/
void
process_1_handler( struct process arr[], struct process p)
{   
      
    monitor_1.element_access++;
    
    // Searh if the new process is alreay in the array
    int at = search(arr, p, monitor_1.arr_SIZE);
    
    if(monitor_1.arr_SIZE < p1_frames){    //Array not full

        if (at < 0)
        {   // not found
            arr[monitor_1.arr_SIZE] = p;
            monitor_1.pagefault++;
            monitor_1.arr_SIZE++;            
        }else{
            if(p.dirty == true) arr[at].dirty = true;
            arr[at].reference = true;
            //found
        }

    }else{

        //Array full
        if (at < 0)
        {
            // Item Not found
            if(monitor_1.refe_pointer == (p1_frames)) monitor_1.refe_pointer = 0;

            unsigned int i;
            for( i = monitor_1.refe_pointer; i < monitor_1.arr_SIZE; i++){

                if (arr[i].reference == true){
                    arr[i].reference = false;
                }else{                    
                    if(arr[i].dirty == true) monitor_1.disk_write++;
                    monitor_1.pagefault++;
                    arr[i] = p;
                    monitor_1.refe_pointer = i+1;
                    goto done;
                }
            }

            for (i = 0; i < monitor_1.arr_SIZE; i++)
            {
                if (arr[i].reference == true){
                    arr[i].reference = false;
                }else{                    
                    if(arr[i].dirty == true) monitor_1.disk_write++;
                    monitor_1.pagefault++;
                    arr[i] = p;
                    monitor_1.refe_pointer = i+1;
                    break;
                }
            }
            done: return;

        }else{
            // Item found it
            if(p.dirty == true) arr[at].dirty = true;
            arr[at].reference = true;
        }
    }
}

/**************************************************************
 * Searching the given process in the arry of processes
 * 
 * @param arr the array of processes
 * @param p   the process to be found in the array
 * @param size the total length of that array
 * @return index number of finding process, -1 otherwise.
 * ************************************************************/
int
search (struct process arr[], struct process p, int size){

    for (int i = 0; i < size; i++)
    {
        if (arr[i].address == p.address) return i;        
    } 
    return -1;
}

/**********************************************
 * Check the number of argument and its values
 * Then assign them to the approciate data 
 * field
 * 
 * @param arg_num the number of arguments
 * @param arg_val the array of argument inputs
 * @return 0 on success, -1 otherwise
 *********************************************/
int 
check_arguments(int arg_num, char const *arg_val[])
{       
    int found = 0;

    string extension;
    string p0_ratio = "", p1_ratio = "", pg_size = "";
    string colon = ":", size_MB = "MB", size_KB = "KB";

    // Check the user's argument number
    if (arg_num < ARG_COUNT){
        cout << "Number of arguments error: \"User inputs are less than eight arguments\"\n";
        return -1;
    }

    // Check the value of users' input
    int i;
    for ( i = 1; i < ARG_COUNT; i++)
    {
        string str = arg_val[i];
        string val;
        string temp_size = "";
        char *ch;
        switch (i)
        {
        case 1: // Checking on user input -n (Number of frames)

            if (str.compare("-n") != 0)
            {
                cout << "Argument " <<str << " is missing.\n";
                return -1;
            }
            val = arg_val[i+1];
            num_frame = stol(val);    
            i++;
            break;

        case 3: // Checking on user input -p (Page sizes in KB, MB, GB)

            if (str.compare("-p") != 0)
            {
                //printf("Argument identifier(%s), missing.\n", str);
                cout << "Argument " << str << " is missing.\n";
                return -1;
            }
            val = arg_val[i+1];           
            page_size = stol(val) * BYTE;
            i++;
            break;

        case 5: // Checking on user input -s (Memory Split)
            
            if (str.compare("-s") != 0)
            {
                //printf("Argument identifier(%s), missiong.\n", str);
                cout << "Argument "<< str << " is missing.\n";
                return -1;
            }            
            val = arg_val[i+1];

            // Parsing the user ratio inputs from "1:2" to value 1 and 2 
            unsigned int j;
            for (j = 0; j < val.length(); j++)
            {
                ch = &val.at(j);             // collect each char 

                if (found != 1 && *ch != ':')
                {
                    p0_ratio.append(ch);
                }else{
                    if (found != 1)
                    {
                        found = 1;
                        j++;
                        ch = &val.at(j);
                    }
                    p1_ratio.append(ch);
                }

                if ((found != 1) && (val.length() == j+1))
                {
                   cout << "Invalid argument input: Expected = 1:2 or 2:1, Actual = " << val << endl;
                }               
            }            
            p1_frames = GET_FRAME_1(num_frame, stol(p0_ratio), stol(p1_ratio));
            p0_frames = GET_FRAME_0(num_frame, p1_frames);
            i++;
            break;

        case 7: // Checking on user input filename
            
            file_name = arg_val[i];          
            break;        
        default:
            cout << "Invalid argument inputs: Problem found at argument number " << i << " and the value is " << arg_val[i];
            return -1;
        }        
    }    
    return 0;    
}

/**********************************************
 * Initialize the global struct
 * Someone has to initialize them
 * *******************************************/
void initialize(){    
    monitor_0.pagefault = 0;
    monitor_0.disk_write = 0;
    monitor_0.refe_pointer = 0;
    monitor_0.arr_SIZE = 0;
    monitor_0.element_access = 0;

    monitor_1.pagefault = 0;
    monitor_1.disk_write = 0;
    monitor_1.refe_pointer = 0;
    monitor_1.arr_SIZE = 0;
    monitor_1.element_access = 0;
}