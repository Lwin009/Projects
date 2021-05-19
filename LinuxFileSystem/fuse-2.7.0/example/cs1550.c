/*
	FUSE: Filesystem in Userspace
	Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

	This program can be distributed under the terms of the GNU GPL.
	See the file COPYING.
*/

#define	FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

//size of a disk block
#define	BLOCK_SIZE 512

//we'll use 8.3 filenames
#define	MAX_FILENAME 8
#define	MAX_EXTENSION 3

//How many files can there be in one directory?
#define MAX_FILES_IN_DIR (BLOCK_SIZE - sizeof(int)) / ((MAX_FILENAME + 1) + (MAX_EXTENSION + 1) + sizeof(size_t) + sizeof(long))

//The attribute packed means to not align these things
struct cs1550_directory_entry
{
	int nFiles;	//How many files are in this directory.
				//Needs to be less than MAX_FILES_IN_DIR

	struct cs1550_file_directory
	{
		char fname[MAX_FILENAME + 1];	//filename (plus space for nul)
		char fext[MAX_EXTENSION + 1];	//extension (plus space for nul)
		size_t fsize;					//file size
		long nIndexBlock;				//where the index block is on disk
	} __attribute__((packed)) files[MAX_FILES_IN_DIR];	//There is an array of these

	//This is some space to get this to be exactly the size of the disk block.
	//Don't use it for anything.
	char padding[BLOCK_SIZE - MAX_FILES_IN_DIR * sizeof(struct cs1550_file_directory) - sizeof(int)];
} ;

typedef struct cs1550_root_directory cs1550_root_directory;

#define MAX_DIRS_IN_ROOT (BLOCK_SIZE - sizeof(int) - sizeof(long)) / ((MAX_FILENAME + 1) + sizeof(long))

struct cs1550_root_directory
{
	long lastAllocatedBlock; //The number of the last allocated block
	int nDirectories;	//How many subdirectories are in the root
						//Needs to be less than MAX_DIRS_IN_ROOT
	struct cs1550_directory
	{
		char dname[MAX_FILENAME + 1];	//directory name (plus space for nul)
		long nStartBlock;				//where the directory block is on disk
	} __attribute__((packed)) directories[MAX_DIRS_IN_ROOT];	//There is an array of these

	//This is some space to get this to be exactly the size of the disk block.
	//Don't use it for anything.
	char padding[BLOCK_SIZE - MAX_DIRS_IN_ROOT * sizeof(struct cs1550_directory) - sizeof(int) - sizeof(long)];
} ;


typedef struct cs1550_directory_entry cs1550_directory_entry;

//How many entries can one index block hold?
#define	MAX_ENTRIES_IN_INDEX_BLOCK (BLOCK_SIZE/sizeof(long))

struct cs1550_index_block
{
	// Number of data block or datablock index
	int nDiskBlock;
    //All the space in the index block can be used for index entries.
	// Each index entry is a data block number.
    long entries[MAX_ENTRIES_IN_INDEX_BLOCK];
};

typedef struct cs1550_index_block cs1550_index_block;

//How much data can one block hold?
#define	MAX_DATA_IN_BLOCK (BLOCK_SIZE)

struct cs1550_disk_block
{
	//All of the space in the block can be used for actual data
	//storage.
	char data[MAX_DATA_IN_BLOCK];
};

typedef struct cs1550_disk_block cs1550_disk_block;

/************************************ HELPER FUNCTIONS****************************/
static void write_root( struct cs1550_root_directory *root);
static struct cs1550_root_directory read_root(void);
static void write_dir_entry(long offset, struct cs1550_directory_entry *entry);
static struct cs1550_directory_entry read_dir_entry(long offset);
static void write_index_block(long _offset, struct cs1550_index_block *index);
// static struct cs1550_index_block read_index_block(long _offset);
static void write_disk_block(long _offset, struct cs1550_disk_block *data);
// static struct cs1550_disk_block read_disk_block(long _offset);

/**
 * @brief Read the block #0 from the .disk file which is the root directory
 * 		  and return the read information.
 * 
 * @return block #0: structure of cs1550_root_directory table
 */
static struct cs1550_root_directory read_root(){

	struct cs1550_root_directory root;
	FILE *file;
	file = fopen(".disk", "rb+");
	fseek(file,0, SEEK_SET);
	fread(&root, sizeof(struct cs1550_root_directory), 1, file);
	fclose(file);
	return root;
}


/**
 * @brief write the updated root directory information in the root directory table
 * 
 * @param root brand new root information to be set
 */
static void write_root( struct cs1550_root_directory *root){

	FILE *file;
	file = fopen(".disk", "rb+");
	fseek(file, 0, SEEK_SET);
	fwrite(root, sizeof(struct cs1550_root_directory), 1, file);
	fclose(file);
}

/**
 * @brief Write the subdirectory entry in the .disk file
 * 
 * @param offset the given disk space to be written
 * @param entry  the new structure (table) of new directory information
 */
static void write_dir_entry(long _offset, struct cs1550_directory_entry *entry){

	FILE *file;
	file = fopen(".disk", "rb+");
	fseek(file, _offset, SEEK_SET);
	fwrite(entry, sizeof(struct cs1550_directory_entry),1,file);
	fclose(file);		
}

/**
 * @brief Read the subdirectory entry from the .disk file where the given offset
 * 		  numbered place.
 * 
 * @param offset the exact location of earlier written
 * @return struct cs1550_directory_entry 
 */
static struct cs1550_directory_entry read_dir_entry(long _offset){

	struct cs1550_directory_entry entry;
	FILE *file;
	file = fopen(".disk", "rb+");
	fseek(file, _offset, SEEK_SET);
	fread(&entry, sizeof(struct cs1550_directory_entry),1, file);
	fclose(file);
	return entry;
}

/**
 * @brief Write the index block table for the user's file data block number
 * 
 * @param _offset the exact place in the .disk file to be written
 * @param index the array of index information of file block numbers
 */
static void write_index_block(long _offset, struct cs1550_index_block *index){

	FILE *file;
	file = fopen(".disk", "rb+");
	fseek(file, _offset, SEEK_SET);
	fwrite(index, sizeof(struct cs1550_index_block ), 1, file);
	fclose(file);
}

/**
 * @brief Read the written index block information from the .disk file
 * 
 * @param _offset the exact place to be read from the .disk file
 * @return struct cs1550_index_block 
 */
// static struct cs1550_index_block read_index_block(long _offset){

// 	struct cs1550_index_block index;

// 	FILE *file;
// 	file = fopen(".disk", "rb+");
// 	fseek(file, _offset, SEEK_SET);
// 	fread(&index, sizeof(struct cs1550_index_block), 1, file);
// 	fclose(file);
// 	return index;
// }

/**
 * @brief  Write the Data block table or information about how much data is written as file
 * 
 * @param _offset the exact place to be written data information
 * @param data the new information with new data information
 */
static void write_disk_block(long _offset, struct cs1550_disk_block *data){
	
	FILE *file;
	file = fopen(".disk", "rb+");
	fseek(file, _offset, SEEK_SET);
	fwrite(data, sizeof(struct cs1550_disk_block), 1, file);
	fclose(file);
}

/**
 * @brief Read the disk block information from the .disk file 
 * 
 * @param _offset the exact place to read from the .disk file
 * @return struct cs1550_disk_block 
 */
// static struct cs1550_disk_block read_disk_block(long _offset){

// 	struct cs1550_disk_block disk;
// 	FILE *file;
// 	file = fopen(".disk", "rb+");
// 	fseek(file, _offset, SEEK_SET);
// 	fread(&disk, sizeof(struct cs1550_disk_block), 1, file);
// 	fclose(file);
// 	return disk;
// }
/************************************ HELPER FUNCTIONS END ***********************/


/*
 * Called whenever the system wants to know the file attributes, including
 * simply whether the file exists or not.
 *
 * man -s 2 stat will show the fields of a stat structure
 */
static int cs1550_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));

	char empty[] = "";
	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];
	strcpy(directory, empty);
	strcpy(filename, empty);
	strcpy(extension, empty);

	// if path is not NULL then fetch the given path into relative variables
	if(path != NULL) sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);

	// // if directory name length is larger than the MAX_FILENAME then return the error
	// // if filename's length is larger than the MAX_FILENAME then return the error
	// // if the file's extension's name length is larger than the MAX_FILENAME then return the error
	// // error message of File name is too long
	// if(strlen(directory) > MAX_FILENAME || strlen(filename) > MAX_FILENAME ||
	// 	strlen(extension) > MAX_EXTENSION) return -ENAMETOOLONG;

	//is path the root dir?
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else {

		//Check if name is subdirectory

		// This point there is a directory other than just "/", root directory
		// Frist of all read the root written file. Exepected that the root will have 
		// value of all zero and empty directory name.
		struct cs1550_root_directory root = read_root();
		int total_directories = root.nDirectories;

		// Debugging purpose only
		// printf("\nROOT DEBUG::\npath = %s", path);
		// // printf("\ndirectory = %s, file = %s.%s\n", directory, filename, extension);
		// printf("\nLast Allocated Block = %ld\n"
		// 		"Number of directories = %d\nDirectory Name = %s\nDirectory start Block = %ld\n"
		// 		, root.lastAllocatedBlock, total_directories, root.directories[total_directories].dname,
		// 		root.directories[total_directories].nStartBlock);

		// // Searching in the root directory
		// printf("\nSearching...\n[ ");
		int i;
		for (i = 0; i < total_directories; i++)
		{
			// printf("%s", root.directories[i].dname);
			if (strcmp(directory, root.directories[i].dname) == 0)
			{	
				// printf("]\nDirectory Found it...\nROOT DEBUG END:::\n");
				// At this point, we knows that there is a directory but it needs
				// to determine just directory name or there is something else?
				// By doing so, we check in a way that if the path has a file extension
				// marks such as '.'. If yes, then this is not just directory name so continue to extrect file names,
				// otherwise, return there is a directory exist
				if(!strchr(path,'.')){
					///Might want to return a structure with these fields
					stbuf->st_mode = S_IFDIR | 0755;
					stbuf->st_nlink = 2;
					res = 0;
					return res;
				}else{

					// At this point, there is a file exist in this subdirectory so we need to fetch that file information.
					struct cs1550_directory_entry entry = read_dir_entry(root.directories[i].nStartBlock);
					int total_files = entry.nFiles; 		// total files in this subdirectory

					for (i = 0; i < total_files; i++)
					{
						if(strcmp(entry.files[i].fname, filename) == 0 &&
							strcmp(entry.files[i].fext, extension) == 0){
							//regular file, probably want to be read and write
							stbuf->st_mode = S_IFREG | 0666;
							stbuf->st_nlink = 1; //file links
							stbuf->st_size = entry.files[i].fsize; //file size - make sure you replace with real size!
							res = 0; // no error
							return res;
						}
					}

					return -ENOENT; 		// REGULAR FILE NOT FOUND...
				}
			}
			
		}

		// printf("]\n Directory NOT FOUND...\n");
		return -ENOENT;	// DIRECTORY NOT FOUND... 

		/*
			//Might want to return a structure with these fields
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
			res = 0; //no error
		*/

		//Check if name is a regular file
		/*
			//regular file, probably want to be read and write
			stbuf->st_mode = S_IFREG | 0666;
			stbuf->st_nlink = 1; //file links
			stbuf->st_size = 0; //file size - make sure you replace with real size!
			res = 0; // no error
		*/
	}
	return res;
}

/*
 * Called whenever the contents of a directory are desired. Could be from an 'ls'
 * or could even be when a user hits TAB to do autocompletion
 */
static int cs1550_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	//Since we're building with -Wall (all warnings reported) we need
	//to "use" every parameter, so let's just cast them to void to
	//satisfy the compiler
	(void) offset;
	(void) fi;

	//the filler function allows us to add entries to the listing
	//read the fuse.h file for a description (in the ../include dir)
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

	char empty[] = "";
	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];
	strcpy(directory, empty);
	strcpy(filename, empty);
	strcpy(extension, empty);

	// if path is not NULL then copy paths components into each categories 
	if(path != NULL) sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);


	// if directory name length is larger than the MAX_FILENAME then return the error
	// error message of File name is too long
	if(strlen(directory) > MAX_FILENAME) return -ENAMETOOLONG;

	// // if the filename is empty or extension is empty, which is invalid file
	// // so return error
	// if(strcmp(filename, "") == 0 || strcmp(extension, "") == 0) return -ENOENT;

	// printf("\nREADDIR DEBUG::\nPath = %s\n", path);	


	if (strcmp(path, "/") == 0){
		//the filler function allows us to add entries to the listing
		//read the fuse.h file for a description (in the ../include dir)
		struct cs1550_root_directory root = read_root();
		int total_directory = root.nDirectories; 	// total directory
		int i;
		// printf("DISPLAYING the root...\n");
		for ( i = 0; i < total_directory; i++)
		{
			filler(buf, root.directories[i].dname, NULL, 0);
		}
		
	}else{
		// This is not just root, there are subdirectory and (may be) regular files
		struct cs1550_root_directory root = read_root();
		int total_directory = root.nDirectories; 	// total directory

		// Debugging purpose only
		// printf("\ndirectory = %s, file = %s.%s\n", directory, filename, extension);
		// printf("CURRENT ROOT::\nLast Allocated Block = %ld\n"
		// 		"Number of directories = %d\nDirectory Name = %s\nDirectory start Block = %ld\n"
		// 		, root.lastAllocatedBlock, total_directory, root.directories[total_directory].dname,
		// 		root.directories[total_directory].nStartBlock);
		int i;
		// printf("\nSearching in the Directory Table...\n");
		for (i = 0; i < total_directory; i++)
		{
			if (strcmp(root.directories[i].dname, directory) == 0)
			{	
				// printf("Found the directory name = %s\n", root.directories[i].dname);
				struct cs1550_directory_entry entry = read_dir_entry(root.directories[i].nStartBlock);
				int total_files = entry.nFiles;

				// printf("CURRENT ENTRY::\nTotal Files = %d\n", entry.nFiles);

				if(total_files < 1) return -ENOENT;	// File do not exist

				// printf("Now LOOKING INTO directory...\n");

				int j;
				for (j = 0; j < total_files; j++)
				{
					// printf("Keep Searching... %s.%s\n", entry.files[j].fname, entry.files[j].fext);
					if (strcmp(entry.files[j].fname, "") != 0 &&
						strcmp(entry.files[j].fext, "") != 0)
					{
						// printf("Found file!\nPrinting it out\n");
						// printf("Filename = %s.%s\n",entry.files[j].fname, entry.files[j].fext);
						char temp[MAX_FILENAME + MAX_EXTENSION + 1];
						strcpy(temp, entry.files[j].fname);
						strcat(temp,".");
						strcat(temp, entry.files[j].fext);
						filler(buf, temp, NULL, 0);
						// printf("Done writing...\n");
					}
					
				}	

				goto DONE;		
			}
			
		}

		return -ENOENT;		// DIRECTORY DOES NOT EXIST
	}

	/*
	//add the user stuff (subdirs or files)
	//the +1 skips the leading '/' on the filenames
	filler(buf, newpath + 1, NULL, 0);
	*/
	DONE:
	return 0;
}

/*
 * Creates a directory. We can ignore mode since we're not dealing with
 * permissions, as long as getattr returns appropriate ones for us.
 */
static int cs1550_mkdir(const char *path, mode_t mode)
{
	int res = 0;

	(void) path;
	(void) mode;

	char empty[] = "";
	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];
	strcpy(directory, empty);
	strcpy(filename, empty);
	strcpy(extension, empty);

	// if path is not NULL then copy paths components into each categories 
	if(path != NULL) sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);


	// if directory name is larger than the maximum file name then reaturn error
	if(strlen(directory) > MAX_FILENAME) return -ENAMETOOLONG;

	// Debugging message...
	//printf("\nMKDIR DEBUG::\nDirectory = %s, Fname = %s.%s\n", directory,filename, extension);
	struct cs1550_root_directory root = read_root();
	int total_directory = root.nDirectories;		// total number of directories 

	//printf("Current directory total = %d\n", root.nDirectories);
	//printf("Last allocated Bl0ck = %ld\n", root.lastAllocatedBlock);

	// Check if the directory already exist or not
	int i;
	for( i = 0; i < total_directory; i++){
		if(strcmp(directory, root.directories[i].dname) == 0){
			// directory exist...
			return -EEXIST;
		}
	}

	// Directory does not exist... Lets create a new one
	struct cs1550_directory_entry entry;
	memset(&entry, 0, sizeof(struct cs1550_directory_entry));
	write_dir_entry(root.lastAllocatedBlock, &entry);

	root.nDirectories += 1;	
	strcpy(root.directories[total_directory].dname, directory);
	root.directories[total_directory].nStartBlock = root.lastAllocatedBlock;
	root.lastAllocatedBlock += sizeof(struct cs1550_directory_entry);
	 
	write_root(&root);

	return res;
}

/*
 * Removes a directory.
 */
static int cs1550_rmdir(const char *path)
{
	(void) path;
    return 0;
}

/*
 * Does the actual creation of a file. Mode and dev can be ignored.
 *
 */
static int cs1550_mknod(const char *path, mode_t mode, dev_t dev)
{
	(void) mode;
	(void) dev;
	(void) path;

	char empty[] = "";
	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];
	strcpy(directory, empty);
	strcpy(filename, empty);
	strcpy(extension, empty);

	// if path is not NULL then copy paths components into each categories 
	if(path != NULL) sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);

	// if directory name length is larger than the MAX_FILENAME then return the error
	// if filename's length is larger than the MAX_FILENAME then return the error
	// if the file's extension's name length is larger than the MAX_FILENAME then return the error
	// error message of File name is too long
	if(strlen(directory) > MAX_FILENAME || strlen(filename) > MAX_FILENAME ||
		strlen(extension) > MAX_EXTENSION) return -ENAMETOOLONG;

	
	// At this poing we need to find the given directory location firstly so we can add the file
	// firstly.
	struct cs1550_root_directory root = read_root();
	int total_directory = root.nDirectories;

	int i;
	for (i = 0; i < total_directory; i++)
	{
		if (strcmp(root.directories[i].dname, directory) == 0)
		{
			//Create a new file
			//First check if the file already existed
			struct cs1550_directory_entry entry = read_dir_entry(root.directories[i].nStartBlock);
			int total_files = entry.nFiles;			// total file in this dierctory

			int j;
			for ( j = 0; j < total_files; j++)
			{
				if (strcmp(entry.files[j].fname, filename) == 0 &&
					strcmp(entry.files[j].fext, extension) == 0)
				{
					return -EEXIST;			// File exist
				}
				
			}

			entry.nFiles += 1;
			strcpy(entry.files[total_files].fname, filename);
			strcpy(entry.files[total_files].fext, extension);
			entry.files[total_files].fsize = 0;
			entry.files[total_files].nIndexBlock = root.lastAllocatedBlock +sizeof(struct cs1550_index_block);

			write_dir_entry(root.directories[i].nStartBlock, &entry);

			root.lastAllocatedBlock = root.lastAllocatedBlock +sizeof(struct cs1550_index_block);

			
			// At this point there is no file exist...
			//Lets create a new file
			//First we have to create a index file then data block
			

			// Now, we need to add the actural file information such as index block table
			struct cs1550_index_block new_index;
			memset(&new_index, 0, sizeof(struct cs1550_index_block));
			int total_index = new_index.nDiskBlock;		//total disk blocks
			new_index.entries[total_index] = root.lastAllocatedBlock + sizeof(struct cs1550_disk_block);

			write_index_block(root.lastAllocatedBlock, &new_index);

			struct cs1550_disk_block new_file;
			memset(&new_file, 0, sizeof(struct cs1550_disk_block ));

			write_disk_block(root.lastAllocatedBlock + sizeof(struct cs1550_disk_block), &new_file);

			root.lastAllocatedBlock = root.lastAllocatedBlock + sizeof(struct cs1550_disk_block);

			write_root(&root);
			return 0;
			
		}
		
	}
	


	return -EPERM;
}

/*
 * Deletes a file
 */
static int cs1550_unlink(const char *path)
{
    (void) path;

    return 0;
}

/*
 * Read size bytes from file into buf starting from offset
 *
 */
static int cs1550_read(const char *path, char *buf, size_t size, off_t offset,
			  struct fuse_file_info *fi)
{
	(void) buf;
	(void) offset;
	(void) fi;
	(void) path;

	size = 0;

	return size;
}

/*
 * Write size bytes from buf into file starting from offset
 *
 */
static int cs1550_write(const char *path, const char *buf, size_t size,
			  off_t offset, struct fuse_file_info *fi)
{
	(void) buf;
	(void) offset;
	(void) fi;
	(void) path;

	return size;
}

/*
 * truncate is called when a new file is created (with a 0 size) or when an
 * existing file is made shorter. We're not handling deleting files or
 * truncating existing ones, so all we need to do here is to initialize
 * the appropriate directory entry.
 *
 */
static int cs1550_truncate(const char *path, off_t size)
{
	(void) path;
	(void) size;

    return 0;
}


/*
 * Called when we open a file
 *
 */
static int cs1550_open(const char *path, struct fuse_file_info *fi)
{
	(void) path;
	(void) fi;
    /*
        //if we can't find the desired file, return an error
        return -ENOENT;
    */

    //It's not really necessary for this project to anything in open

    /* We're not going to worry about permissions for this project, but
	   if we were and we don't have them to the file we should return an error

        return -EACCES;
    */

    return 0; //success!
}

/*
 * Called when close is called on a file descriptor, but because it might
 * have been dup'ed, this isn't a guarantee we won't ever need the file
 * again. For us, return success simply to avoid the unimplemented error
 * in the debug log.
 */
static int cs1550_flush (const char *path , struct fuse_file_info *fi)
{
	(void) path;
	(void) fi;

	return 0; //success!
}

/* Thanks to Mohammad Hasanzadeh Mofrad (@moh18) for these
   two functions */
static void * cs1550_init(struct fuse_conn_info* fi)
{
	  (void) fi;
    printf("We're all gonna live from here ....\n");
	
	//Initialize the root directory here
	struct cs1550_root_directory root;
	root.nDirectories = 0;
	root.lastAllocatedBlock = sizeof(struct cs1550_root_directory);
	strcpy(root.directories[0].dname, "");
	root.directories[0].nStartBlock = 0;
	write_root(&root);
	return NULL;
}

static void cs1550_destroy(void* args)
{
		(void) args;
    printf("... and die like a boss here\n");

}


//register our new functions as the implementations of the syscalls
static struct fuse_operations hello_oper = {
    .getattr	= cs1550_getattr,
    .readdir	= cs1550_readdir,
    .mkdir	= cs1550_mkdir,
		.rmdir = cs1550_rmdir,
    .read	= cs1550_read,
    .write	= cs1550_write,
		.mknod	= cs1550_mknod,
		.unlink = cs1550_unlink,
		.truncate = cs1550_truncate,
		.flush = cs1550_flush,
		.open	= cs1550_open,
		.init = cs1550_init,
    .destroy = cs1550_destroy,
};

//Don't change this.
int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &hello_oper, NULL);
}
