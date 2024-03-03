
#define _CRT_SECURE_NO_WARNINGS
/*
    disables the warnings
*/
#include <fcntl.h> 
/*
    -open (read, write, both) 
    -system call
*/
#include<stdio.h>
/*
    - printf
    - scanf
*/
#include<stdlib.h>
/*
    - malloc
    - free
*/
#include "mystring.h"
/*
    - mystrcpy
    - mystrncpy
    - mymemset

*/
/*
    #include<io.h>

    -header file is used in windows
    1 -> open
    2-> close
    4-> write
   
*/
#include<unistd.h>
/*
    -header file is used in linux/mac
*/

#define MAX_INODE 50
#define MAX_FILE_SIZE 1024
#define REGULAR 1

#define READ 1
#define WRITE 2

#define CURRENT 1
#define START 0
#define END 2

/*
    Superblock

    
 */
typedef struct super_block{

    int total_inode;

    int free_inode;
}SUPER_BLOCK,*P_SUPER_BLOCK;

/*
    Index node structure
*/

typedef struct i_node{
    char file_name[50]; 
    
    int inode_number;
    
    int file_size;

    int file_actual_size;

    int file_type;
    
    char *buffer;
   
    int link_count;
    
    int reference_count;
    
    int permission;
    struct i_node *next;

}I_NODE, *P_INODE, **PP_INODE;

/*
    File Table
*/

typedef struct file_table{
    int read_offset;

    int write_offset;

    int count;
    
    int mode;

    P_INODE ptr_inode;
}FILE_TABLE , *P_FILE_TABLE;


/*
    User file descriptor table 
*/
typedef struct ufdt{
    P_FILE_TABLE ptr_file_table;
}UFDT;

/*
	Declaration of function
*/
void Usage_operation();
void display_operation_name();
void man(char *name);
void initialise_superblock();
int get_fd_from_name(char *name);
void create_dynamic_inode_list_block();
int create_file(char *name, int permission);
int open_file(char *name, int mode);
int stat_file(char *name);
int fd_stat_file(int fd);
int rm_file(char *name);
int truncate_file(char *name);
int back_up();
void ls_file();
int close_file_by_name(char *name);
void close_all_file();
int read_file(int fd, char *arr, int i_size);
int write_file(int fd, char *arr, int i_size);
int lseek_file(int fd, int i_size, int from);


UFDT UFDT_arr[MAX_INODE];
SUPER_BLOCK super_block_obj;
P_INODE head = NULL;
/*
    main function
*/
int main(){
    char *str = NULL;

    int i_ret = 0, fd = 0, i_count = 0;
    char c_command[4][70],c_str[70],c_arr[MAX_FILE_SIZE];
	Usage_operation();
    printf("___________________________________________________________________________________________________________\n\n");
    initialise_superblock();
    create_dynamic_inode_list_block();
	display_operation_name();

    while(1){
        
        fflush(stdin);  // to clear the input buffer
        mystrcpy(c_str,"");

        printf("\nENTER OPERATION : \t");

        fgets(c_str,70,stdin);  //to read a line of text from a file stream or from the standard input

        i_count = sscanf(c_str, "%s %s %s %s",c_command[0],c_command[1],c_command[2],c_command[3]);  //used to parse formatted input from a string

        if(i_count == 1){

            if(mystrcmp(c_command[0],"ls") == 0 ){
                ls_file();
            }
            else if(mystrcmp(c_command[0],"clear") == 0){
               /*
                system("cls");
                  -used in Windows environments to clear the console screen
                */
                system("clear");
                /*
                 -used in Unix environments to clear the console screen
                */
                continue;
            }
            else if(mystrcmp(c_command[0],"closeall") == 0){
                close_all_file();
                continue;
            }
            else if(mystrcmp(c_command[0],"backup") == 0){
                i_ret = back_up();
                if(i_ret == -1){
                    printf("error creating file...\n");
                }

                if(i_ret == 0){
                    printf("file created successfully....\n");
                }
                continue;
            }
            else if(mystrcmp(c_command[0], "help") == 0){
                display_operation_name();
                continue;
            }
            else if(mystrcmp(c_command[0], "exit") == 0){
                printf("Terminating the program.....\n");
                break;
            }
            else{
                printf("Command not found....\n");
                continue;
            }
        }
        else if(i_count == 2){
            
            if(mystrcmp(c_command[0],"stat") == 0){
                i_ret = stat_file(c_command[1]);
                if(i_ret == -1){
                    printf("Incorrect parameters...\n");
                }

                if(i_ret == -2){
                    printf("There is no such file...\n");
                }
                continue;
            }
            else if (mystrcmp(c_command[0], "fstat") == 0){
                i_ret = fd_stat_file(atoi(c_command[1]));

                if(i_ret == -1){
                    printf("Incorrect parameters...\n");
                }

                if(i_ret == -2){
                    printf("There is no such file.../n");
                }

                continue;
            }
            else if(mystrcmp(c_command[0], "close") == 0){	
				i_ret = close_file_by_name(c_command[1]);

				if(i_ret == -1){
                    printf("There is no such file.....\n");
                }

                if(i_ret == -2){
                    printf("The File Is Already Closed\n");
                }

                if(i_ret == 0){
                    printf("File is closed successfully..\n");
                }
                continue;
			}

			else if(mystrcmp(c_command[0], "rm") == 0) {
				i_ret = rm_file(c_command[1]);

				if(i_ret == -1){
                    printf("There is no such file.....\n");
				    continue;
                }

                if(i_ret ==  0){
                    printf("File deleted successfully.../n");
                }
			}

			else if(mystrcmp(c_command[0], "man") == 0){
				man(c_command[1]);
			}

			else if(mystrcmp(c_command[0], "write") == 0)
			{
				fd = get_fd_from_name(c_command[1]);
				
                if(fd == -1){
					printf("Incorrect parameter....\n");
					continue;
				}

                if(UFDT_arr[fd].ptr_file_table -> ptr_inode -> reference_count == 0){
                    printf("File is Not Opened.....\n");
                }
                
                else{
                    printf("Enter the data : \n");
				   fgets(c_arr, sizeof(c_arr), stdin);
                }
				fflush(stdin); // empty input buffer
	
				i_ret = mystrlen(c_arr);

				if(i_ret == 0)
				{
					printf("Incorrect parameter....\n");
					continue;
				}

				i_ret = write_file(fd,c_arr,i_ret); // 0, Adress, 4

				if(i_ret == -1)
                		{
                    			printf("Permission denied....\n");
                		}
					
				if(i_ret == -2)
                		{
                    			printf("There is no sufficient memory to write...\n");
                		}
					
				if(i_ret == -3)
                		{
                    			printf("It is not regular file....\n");
                		}

				if(i_ret == -4)
				{
					printf("There is no sufficient memory Available....\n");
				}

                		if(i_ret > 0)
				{
					printf("Sucessfully : %d bytes written\n", i_ret);
				}
			}

			else if(mystrcmp(c_command[0], "truncate") == 0){
				i_ret = truncate_file(c_command[1]);

				if(i_ret == -1){
                    printf("Incorrect parameter...\n");
                }

                if(i_ret == 0){
                    printf("file data delete successfully.../n");
                }
			}	

			else	
			{	
				printf("\nCommand not found !!!\n");
			}	
            		continue;
		}

		else if(i_count == 3){

			if(mystrcmp(c_command[0], "create") == 0){
				i_ret = create_file(c_command[1], atoi(c_command[2])); // atoi -> ASCII to Integer
		
				if(i_ret >= 0){
                    printf("File is successfully created with file descriptor : %d\n", i_ret);
                }
					
				if(i_ret == -1){
        			printf("Incorrect parameters...\n");
                }
					
				if(i_ret == -2){
                    printf("There is no inodes...\n");
            	}
					
				if(i_ret == -3){
                    printf("File already exists...\n");
            	}
					
				if(i_ret == -4){
                    printf("Memory allocation failure...\n");
                }
                continue;
			}

			else if(mystrcmp(c_command[0], "open") == 0)
			{
				i_ret = open_file(c_command[1], atoi(c_command[2]));
		
				if(i_ret >= 0)
                		{
                    			printf("File is successfully opened with file descriptor : %d\n", i_ret);
                		}
					
				if(i_ret == -1)
                		{
                    			printf("Incorrect parameters...\n");
                		}
					
				if(i_ret == -2)
                		{
                   	 		printf("File not present...\n");
                		}
					
				if(i_ret == -3)
                		{
                    			printf("Permission denied...\n");
                		}
                		continue;
			}

			else if(mystrcmp(c_command[0], "read") == 0){

				fd = get_fd_from_name(c_command[1]);

				if(fd == -1){
					printf("Incorrect parameter...\n");
					continue;
				}

				str = (char *)malloc(sizeof(atoi(c_command[2])) + 1);

				if(str == NULL){
					printf("Memory allocation failure...\n");
					continue;
				}

				i_ret = read_file(fd, str, atoi(c_command[2]));
				
                if(i_ret == -1){
                    printf("File not existing...\n");
                }
					
				if(i_ret == -2){
                    printf("Permission denied...\n");
            	}
					
				if(i_ret == -3){
                    printf("Reached at end of file...\n");
                }
					
				if(i_ret == -4){
                    printf("It is not regular file\n");
                }	

                if (i_ret == -5){
                    printf("File is not opened\n");
                }
					
				if(i_ret == 0){
                	printf("File empty\n");
                }
					
				if(i_ret > 0){
					write(2, str, i_ret);  // 0 for stdin, 1 for stdout 
				}
				continue;
			}

			else	
			{	
				printf("\nCommand not found !!!\n");
				continue;
			}	
		}

		else if(i_count == 4)
		{
			if(mystrcmp(c_command[0], "lseek") == 0){
				fd = get_fd_from_name(c_command[1]);

				if(fd == -1){
					printf("Incorrect parameter...\n");
					continue;
				}
				
				i_ret = lseek_file(fd, atoi(c_command[2]), atoi(c_command[3]));
				
                if(i_ret == -1){
					printf("Unable to perform lseek...\n");
				}

                if(i_ret == -2){
                    printf("File is not opened...\n");
                }

                if(i_ret == 0){
                    printf("sucessfully changed ...\n");
                }
			}
            else{
				printf("\nCommand not found !!!\n");
				continue;
			}
		}
        else{
			printf("\nCommand not found !!!\n");
			continue;
		}
	}	
	return	0;
}





/*
    Display function
*/

void display_operation_name(){
    printf("\nman      :  Info about operation ->man command_name\n");
    printf("ls        :  List the file of a directory\n");
    printf("clear     :  clear console\n");
    printf("open      :  open the file\n");
    printf("close     :  close the file\n");
    printf("closeall  :  close all open file\n");
	printf("create    :  used to create new regular file\n");
    printf("read      :  Read the contents from file\n"); 
    printf("write     :  Write contents into file\n"); 
    printf("exit      :  Terminate file system\n"); 
    printf("stat      :  Display information of file using name\n"); 
    printf("fstat     :  Display information of file using file descriptor\n"); 
    printf("truncate  :  Remove all data from file\n"); 
    printf("rm        :  Delet the file\n");
    printf("backup    :  Take backup Of all created files\n");
}


/*
 MAN
*/


void man(char *name)
{
    if(name == NULL) 
    {
        return;
    }

    if(mystrcmp(name, "create") == 0) 
    {
        printf("Description : Used To Create New Regular File\n");
        printf("Usage : create file_name  permission( READ 1 WRITE 2  READ + WRITE 3)\n\n");
    }

    else if(mystrcmp(name, "read") == 0) 
    {
        printf("Description : Used to read data from regular file\n");
        printf("Usage : read file_name  no_of_bytes_to_read\n\n");
    }

    else if(mystrcmp(name, "write") == 0) 
    {
        printf("Description : Used to write data into regular file\n"); 
        printf("Usage : write file_name\n After this enter the data that we want to write\n\n");
    }

    else if(mystrcmp(name, "ls") == 0) 
    {
        printf("Description : Used to list all information of files\n"); 
        printf("Usage : ls\n\n");
    }

    else if(mystrcmp(name, "stat") == 0) 
    {
        printf("Description : Used to display information of file\n"); 
        printf("Usage : stat file_name\n\n");
    }

    else if(mystrcmp(name, "fstat") == 0) 
    {
        printf("Description : Used to display information of file\n"); 
        printf("Usage : stat file_descriptor\n\n");
    }

    else if(mystrcmp(name, "truncate") == 0) 
    {
        printf("Description : Used to remove data from file\n"); 
        printf("Usage : truncate File_name\n\n");
    }

    else if(mystrcmp(name, "open") == 0) 
    {
        printf("Description : Used to open existing file\n"); 
        printf("Usage : open File_name mode( READ 1 WRITE 2  READ + WRITE 3)\n\n");
    }

    else if(mystrcmp(name, "close") == 0)
    {
        printf("Description : Used to close opened file\n"); 
        printf("Usage : close File_name\n\n"); 
    }

    else if(mystrcmp(name, "closeall") == 0) 
    {
        printf("Description : Used to close all opened file\n"); 
        printf("Usage : closeall\n\n");
    }

    else if(mystrcmp(name, "lseek") == 0) 
    {
        printf("Description : Used to change file offset\n"); 
        printf("Usage : lseek File_Name ChangeInOffset StartPoint\n\n");
    }

    else if(mystrcmp(name, "rm") == 0) 
    {
        printf("Description : Used to delete the file\n"); 
        printf("Usage : rm File_Name\n\n");
    }

    else if(mystrcmp(name,"backup") == 0)
    {
        printf("Description : Used To Take Backup Of All Files Created\n");
        printf("Usage : backup\n\n");
    }

    else
    {
        printf("ERROR : No manual entry available.\n\n"); 
    }
}
/*
    Initialise superblock
*/

void initialise_superblock(){
    int i = 0;
    while(i < MAX_INODE){
        UFDT_arr[i].ptr_file_table = NULL;
        i++;
    }
    super_block_obj.total_inode = MAX_INODE;
    super_block_obj.free_inode = MAX_INODE;
}


/*
    get file descriptor from name
*/

int get_fd_from_name(char *name){
    int i = 0;
    while(i < MAX_INODE){
        
        if(UFDT_arr[i].ptr_file_table != NULL){
            
            if(mystrcmp(UFDT_arr[i].ptr_file_table -> ptr_inode -> file_name,name) == 0){
                break;
            }
        }
    }

    if(i == MAX_INODE){
        return -1;
    }
    else{
        return i;
    }
}



/*
    Create Dynamic Inode List Block
*/


void create_dynamic_inode_list_block(){
    P_INODE newn = NULL;
    P_INODE temp = head;
    int i = 1;
    while(i <= MAX_INODE){
        newn = (P_INODE)malloc(sizeof(I_NODE));

        if(newn == NULL){
            printf("Dynamic inode list block failed to create....\n");
            return;
        }

        newn -> link_count = 0;
        newn -> reference_count = 0;
        newn -> file_size = 0;
        newn -> file_type = 0;
        newn -> buffer = NULL;
        newn -> next = NULL;
        newn -> inode_number = i;
    

        if(temp == NULL){
            head = newn;
            temp = head;
        }
        else{
            temp -> next = newn;
            temp = temp -> next;
        }
        i++;
    }
    printf("Dynamic inode list block created successfully.....\n");

}
/*
    get inode
*/

P_INODE get_inode(char *name){

    P_INODE temp = head;
    int i = 0;

    if(name == NULL){
        return NULL;
    }

    while(temp != NULL){
        if(mystrcmp(name, temp -> file_name) == 0){
            break;
        }
        temp = temp -> next;
    }

    return temp;
}
/*
    Create File
*/

int create_file(char *name, int permission){
    int i  = 0;
    P_INODE  temp = head;

    if((name == NULL) || (permission == 0) || (permission > 3)/*permission > 3 as file create for write purpose only*/){
        return -1;
    }

    if(super_block_obj.free_inode == 0){
        return -2;
    }

    (super_block_obj.free_inode)--;

    if(get_inode(name) != NULL){
        return -3;
    }

    while(temp != NULL){
        if(temp -> file_type == 0){
            break;
        }

        temp = temp -> next;
    }

    while(i < MAX_INODE){
        if(UFDT_arr[i].ptr_file_table == NULL){
            break;
        }
        i ++;
    }

    UFDT_arr[i].ptr_file_table = (P_FILE_TABLE)malloc(sizeof(FILE_TABLE));

    if(UFDT_arr[i].ptr_file_table == NULL){
        return -4;
    }

    UFDT_arr[i].ptr_file_table -> count = 1;
    UFDT_arr[i].ptr_file_table -> mode = permission;
    UFDT_arr[i].ptr_file_table -> read_offset = 0;
    UFDT_arr[i].ptr_file_table -> write_offset = 0;

    UFDT_arr[i].ptr_file_table ->ptr_inode = temp;

    mystrcpy(UFDT_arr[i].ptr_file_table -> ptr_inode -> file_name , name);
    UFDT_arr[i].ptr_file_table -> ptr_inode -> file_type = REGULAR;
    UFDT_arr[i].ptr_file_table -> ptr_inode -> reference_count = 1;
    UFDT_arr[i].ptr_file_table -> ptr_inode -> link_count = 1;
    UFDT_arr[i].ptr_file_table -> ptr_inode -> file_size = MAX_FILE_SIZE;
    UFDT_arr[i].ptr_file_table -> ptr_inode -> file_actual_size = 0;
    UFDT_arr[i].ptr_file_table -> ptr_inode -> permission = permission;
    UFDT_arr[i].ptr_file_table -> ptr_inode -> buffer = (char *)malloc(MAX_FILE_SIZE);

    mymemset(UFDT_arr[i].ptr_file_table -> ptr_inode -> buffer, 0 , 1024);

    return i;
} 

/*
    Open File
*/

int open_file(char *name, int mode){
    int i = 0;
    P_INODE temp = NULL;

    if(name == NULL || mode <= 0){
        return -1;
    }

    temp = get_inode(name);

    if(temp == NULL){
        return -2;
    }

    if(temp -> permission < mode){
        return -3;
    }

    while(i < MAX_INODE){
        
        if(UFDT_arr[i]. ptr_file_table == NULL){
            break;
        }
        i++;
    }

    UFDT_arr[i].ptr_file_table = (P_FILE_TABLE)malloc(sizeof(FILE_TABLE));

    if(UFDT_arr[i].ptr_file_table == NULL){
        return -1;
    }

    UFDT_arr[i].ptr_file_table -> count = 1;
    UFDT_arr[i].ptr_file_table -> mode = mode;

    if(mode == READ + WRITE){
        UFDT_arr[i].ptr_file_table -> read_offset = 0;
        UFDT_arr[i].ptr_file_table -> write_offset = 0;
    }
    else if(mode == READ){
        UFDT_arr[i].ptr_file_table -> read_offset = 0;
    }
    else if(mode == WRITE){
        UFDT_arr[i].ptr_file_table -> write_offset = 0;
    }

    UFDT_arr[i].ptr_file_table -> ptr_inode = temp;
    (UFDT_arr[i].ptr_file_table -> ptr_inode -> reference_count) ++;

    printf("File opened successfully....\n");
    return i;
}

/*
    Read file
*/

int read_file(int fd, char *arr, int i_size){
    
    int read_size = 0;
    if(UFDT_arr[fd].ptr_file_table == NULL){
        return -1;
    }

    if(UFDT_arr[fd].ptr_file_table -> mode != READ && UFDT_arr[fd].ptr_file_table -> mode != READ + WRITE){
        return -2;
    }

    if(UFDT_arr[fd].ptr_file_table -> ptr_inode -> permission != READ && UFDT_arr[fd].ptr_file_table -> ptr_inode -> permission != READ + WRITE){
        return -2;
    }

    if(UFDT_arr[fd].ptr_file_table -> read_offset == UFDT_arr[fd].ptr_file_table ->ptr_inode ->file_actual_size){
        return -3;
    }

    if(UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_type != REGULAR){
        return -4;
    }

    read_size = (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) - (UFDT_arr[fd].ptr_file_table -> read_offset);

    if(read_size < i_size){
        mystrncpy(arr,(UFDT_arr[fd].ptr_file_table ->ptr_inode -> buffer) + (UFDT_arr[fd].ptr_file_table -> read_offset),i_size);
        UFDT_arr[fd].ptr_file_table -> read_offset = UFDT_arr[fd].ptr_file_table ->read_offset + read_size;
    }
    else{
        mystrncpy(arr,(UFDT_arr[fd].ptr_file_table -> ptr_inode ->buffer) + (UFDT_arr[fd].ptr_file_table -> read_offset),i_size);
        (UFDT_arr[fd].ptr_file_table -> read_offset) = (UFDT_arr[fd].ptr_file_table -> read_offset + i_size);
    }

    return i_size;
}

/*
    Write function
*/


int write_file(int fd, char *arr, int i_size){

    if((UFDT_arr[fd].ptr_file_table -> ptr_inode -> permission != WRITE && UFDT_arr[fd].ptr_file_table -> ptr_inode -> permission != READ + WRITE)  || (UFDT_arr[fd].ptr_file_table -> mode != WRITE && UFDT_arr[fd].ptr_file_table -> mode != READ + WRITE)){
        return -1;
    }

    if(UFDT_arr[fd].ptr_file_table -> write_offset == MAX_FILE_SIZE){
        return -2;
    }

    if(UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_type != REGULAR){
        return -3;
    }

    if((UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_size)-(UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) < i_size){
        return -4;
    }

    mystrncpy(((UFDT_arr[fd].ptr_file_table -> ptr_inode -> buffer) + (UFDT_arr[fd].ptr_file_table -> write_offset)),arr,i_size);
    (UFDT_arr[fd].ptr_file_table -> write_offset) = (UFDT_arr[fd].ptr_file_table -> write_offset) + i_size;
    (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) = (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) + i_size;

    return 5;
}


/*
	How to use opertion
*/
void Usage_operation(){
	printf("\n man offer essential guidance on how to use and understand the operation.\n Example :  man create \n\t    man ls\n");
	return;
}


/*
    Long seek
*/

int lseek_file(int fd, int i_size, int from){
    if((fd < 0) || (from > 2) || (UFDT_arr[fd].ptr_file_table == NULL)){
        return -1;
    }

    if(UFDT_arr[fd].ptr_file_table -> ptr_inode -> reference_count == 0){
        return -2;
    }

    if((UFDT_arr[fd].ptr_file_table -> mode == READ) || (UFDT_arr[fd].ptr_file_table -> mode = READ + WRITE)){
        if(from == CURRENT){
            if(((UFDT_arr[fd].ptr_file_table -> read_offset ) + i_size) > (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) ) {
                return -1;
            }

            if((UFDT_arr[fd].ptr_file_table -> read_offset + i_size) < 0){
                return -1;
            }

            (UFDT_arr[fd].ptr_file_table -> read_offset) = (UFDT_arr[fd].ptr_file_table -> read_offset) + i_size;
        }
        else if(from == START){

            if(i_size > UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size){
                return -1;
            }

            if(i_size < 0){
                return -1;
            }

            (UFDT_arr[fd].ptr_file_table -> read_offset ) = i_size;
        }
        else if(from == END){
            
            if(((UFDT_arr[fd].ptr_file_table ->ptr_inode ->file_actual_size)+ i_size ) > MAX_FILE_SIZE){
                return -1;
            }

            if(((UFDT_arr[fd].ptr_file_table -> read_offset) + i_size) < 0){
                return -1;
            }

            (UFDT_arr[fd].ptr_file_table -> read_offset) = (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) + i_size;
        }
    }
    else if(UFDT_arr[fd].ptr_file_table -> mode == WRITE){

        if(from == CURRENT){

            if(((UFDT_arr[fd].ptr_file_table -> write_offset) + i_size ) > MAX_FILE_SIZE){
                return -1;
            }

            if(((UFDT_arr[fd].ptr_file_table -> write_offset) + i_size) < 0){
                return -1;
            }

            if(((UFDT_arr[fd].ptr_file_table -> write_offset) + i_size) > (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size)){
                (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) = (UFDT_arr[fd].ptr_file_table -> write_offset) + i_size;
                (UFDT_arr[fd].ptr_file_table -> write_offset) = (UFDT_arr[fd].ptr_file_table -> write_offset) + i_size;
            }
        }
        else if(from == START){

            if(i_size > MAX_FILE_SIZE  || i_size < 0){
                return -1;
            }
            
            if(((UFDT_arr[fd].ptr_file_table -> write_offset) + i_size) > (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size)){
                (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) = (UFDT_arr[fd].ptr_file_table -> write_offset) + i_size;
                (UFDT_arr[fd].ptr_file_table -> write_offset) = (UFDT_arr[fd].ptr_file_table -> write_offset) + i_size;
            }
        }
        else if(from == END){

            if(((UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) + i_size) > MAX_FILE_SIZE  || (((UFDT_arr[fd].ptr_file_table -> write_offset) + i_size) < 0)){
                return -1;
            }

            (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) = (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) + i_size;
            (UFDT_arr[fd].ptr_file_table -> write_offset) = (UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size) + i_size;
        }
    }
   

    return 0;

}


/*
    list all file
*/

void ls_file(){
    int i = 0;

    P_INODE temp = head;

    if(super_block_obj.free_inode == MAX_INODE){
        printf("There are no files...\n");
        return;
    }

    printf("--------------------------------------------------------------------------\n");
    while(temp != NULL){
        if(temp -> file_type != 0){
            printf("File Name :\t%s\nInode number :\t%d\nFile size :\t%d\nLink count :\t%d\n",temp ->file_name, temp -> inode_number,temp ->file_actual_size,temp ->link_count);
        }
    }
    printf("--------------------------------------------------------------------------\n");
}

/*
    close file by name
*/

int close_file_by_name(char *name){
    if(name == NULL){
        return -1;
    }

    int fd = get_fd_from_name(name);

    if(fd < 0  || UFDT_arr[fd].ptr_file_table == NULL ){
        return -1;
    }

    if(UFDT_arr[fd].ptr_file_table -> ptr_inode -> reference_count == 0){
        return -2;
    }

    UFDT_arr[fd].ptr_file_table -> ptr_inode -> reference_count = 0;
    UFDT_arr[fd].ptr_file_table -> read_offset = 0;
    UFDT_arr[fd].ptr_file_table -> write_offset = 0;

    return 0;
}

/*
    close all file
*/

void close_all_file(){
    int i = 0;
    while(i < MAX_INODE){
        if(UFDT_arr[i].ptr_file_table != NULL){
            UFDT_arr[i].ptr_file_table -> read_offset = 0;
            UFDT_arr[i].ptr_file_table -> write_offset = 0;
            UFDT_arr[i].ptr_file_table -> ptr_inode -> reference_count = 0;
        }
        i++;
    }
    printf("All files are closed ......\n");
}

/*
    stat
    Display Statistical Information Of The File By Using File Name
*/

int stat_file(char *name){
    P_INODE temp = head;

    if(name == NULL){
        return -1;
    }

    while(temp != NULL){
        if(mystrcpy(temp -> file_name ,name) == 0){
            break;
        }
        temp = temp -> next;
    }

    if(temp == NULL){
        return -2;
    }

    printf("-------------------------------------------------------------\n");
    
    
    printf("File name : %s\n", temp -> file_name);
	printf("Inode Number %d\n", temp -> inode_number);
    printf("Link count : %d\n", temp -> link_count);
    printf("Reference count : %d\n", temp -> reference_count);
	printf("File size : %d\n", temp -> file_size);
	printf("Actual File size : %d\n", temp -> file_actual_size);
	

	if(temp -> permission == 1){
        	printf("File Permission : Read only\n");
    }
	else if(temp -> permission == 2){
        printf("File Permission : Write\n");
    }	
    else if(temp -> permission == 3){
        printf("File Permission : Read & Write\n");
    }
		
	printf(" ------------------------------------------------------------\n"); 
	
	return 0; 
}

/*
    fd_stat_file
     Display Statistical Information Of The File By Using file descriptor.
*/

int fd_stat_file(int fd){

    if(fd < 0){
        return -1;
    }

    if(UFDT_arr[fd].ptr_file_table == NULL){
        return -2;
    }

    P_INODE temp = UFDT_arr[fd].ptr_file_table -> ptr_inode;

    printf("-------------------------------------------------------------\n");
    
    
    printf("File name : %s\n", temp -> file_name);
	printf("Inode Number %d\n", temp -> inode_number);
    printf("Link count : %d\n", temp -> link_count);
    printf("Reference count : %d\n", temp -> reference_count);
	printf("File size : %d\n", temp -> file_size);
	printf("Actual File size : %d\n", temp -> file_actual_size);
	

	if(temp -> permission == 1){
        	printf("File Permission : Read only\n");
    }
	else if(temp -> permission == 2){
        printf("File Permission : Write\n");
    }	
    else if(temp -> permission == 3){
        printf("File Permission : Read & Write\n");
    }
		
	printf(" ------------------------------------------------------------\n"); 
	
	return 0; 


}

/*
    Remove file
*/

int rm_file(char *name){

    if(name == NULL){
        return -1;
    }

    int fd = get_fd_from_name(name);

    if(fd < 0){
        return -1;
    }

    (UFDT_arr[fd].ptr_file_table -> ptr_inode ->link_count)--;

    if(UFDT_arr[fd].ptr_file_table -> ptr_inode -> link_count == 0){

        mystrcpy(UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_name, "");
        UFDT_arr[fd].ptr_file_table -> ptr_inode -> reference_count = 0;
        free(UFDT_arr[fd].ptr_file_table -> ptr_inode -> buffer);
        UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size = 0;
        UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_type = 0;
        UFDT_arr[fd].ptr_file_table -> ptr_inode -> permission = 0;
        free(UFDT_arr[fd].ptr_file_table );
    }

    UFDT_arr[fd].ptr_file_table = NULL;
    (super_block_obj.free_inode)++;

    return 0;

}


/*
 truncate file

*/
int truncate_file(char *name){

    if(name == NULL){
        return -1;
    }

    int fd = get_fd_from_name(name);

    if(fd < 0){
        return -1;
    }

    mymemset(UFDT_arr[fd].ptr_file_table -> ptr_inode -> buffer, 0 , MAX_FILE_SIZE);
    UFDT_arr[fd].ptr_file_table -> read_offset = 0;
    UFDT_arr[fd].ptr_file_table -> write_offset = 0;
    UFDT_arr[fd].ptr_file_table -> ptr_inode -> file_actual_size = 0;

    return 0;

}

/*
    Backup
*/

int back_up() {
    FILE *f_ptr;
    P_INODE temp = head;

    while (temp != NULL) {
        if (temp->file_type != 0) {
            f_ptr = fopen(temp->file_name, "w");
            if (f_ptr == NULL) {
                return -1; 
            }

            write(fileno(f_ptr), temp->buffer, temp->file_actual_size);

            fclose(f_ptr);
        }
        temp = temp->next; 
    }

    return 0;
}

