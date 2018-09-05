#define _XOPEN_SOURCE 500 
#define  MAXDIM 1000
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int is_symbolic(const char *fpath) ;
ssize_t readlink(const char *restrict path, char *restrict buf,
		 size_t bufsize);

/*
*  function fun after calling in main with ntfw , will keep track of all path
*  also keep track of all cycles it finds. Also by finding a cycle it searches in all path
*  to check if it backs to the root or not. if finds print a message.If not it saves that cycle to check
*  later when it receives a new path.
*
*/

static int fun(const char *fpath, const struct stat *sb,int tflag, struct FTW *ftwbuf)
{     

	static char   all_path[MAXDIM][100]; 
	static char   all_sym[MAXDIM][100]; 
	static char   sym_path [MAXDIM][100];
	char buf[512] ;
	char *ptr ;


	static int k = 0;
	static int j = 0;
	static int num = 1;
	int result ;


        //saving a new path in an array of all path we got.
	strncpy (all_path[k] , fpath , strlen(fpath));
	k++ ;



        //check if there is a cycle, find the path what it backs to
	if(!is_symbolic(fpath)){


		int count = readlink(fpath, buf, sizeof(buf));
		if (count >= 0) 
			buf[count] = '\0';

		for (int i = 0 ; i < k  ; i++) {

		//compare the current destination's path that cycle backs to. 	
		result = strcmp(all_path[i], buf);

			if(result ==0) {

                                //if this cycle backs to one of path we found print a message 
				printf ("\nYes! There is a cycle.Because %s to %s\n",fpath, all_path[i]);

				return 0;

			}
		}

		if (result != 0){
			//if couldn't find a matched path in this branch of the root ,save both new path and the path the cycle backs.
			strncpy (all_sym[j] , buf , strlen(buf));
			strncpy (sym_path[j] ,fpath , strlen(fpath));

			j++ ;
		}
       	}

	for (int i = 0 ; i <j  ; i++) {

               //this is when we may find matched path in other branch of the dirname.
		ptr = realpath(all_sym[i], (char *)fpath);

		if(ptr) {

			//we define a counter and we want to print the message just one time
			if( num ==1)
				printf ("Yes! Because directory %s to %s \n", sym_path[i], fpath);
			num++ ;

			return 0 ; }

	}	     


	return 0;          
}


/*
* This function check if there is symbolik link on the path
* that we gave to it or not.
*/

int is_symbolic(const char *fpath) {
	struct stat buf;
	

	lstat (fpath, &buf);
	if (S_ISLNK(buf.st_mode))

		return 0;

	else

		return -1 ;


}

/*
 * this program gets one or two command line arguments. It calls
 * fun in each entry in the tree. 
*/

int main(int argc, char *argv[])
{
       

	if (nftw((argc < 2) ? "." : argv[1], fun, 20, FTW_PHYS)
			== -1) {
		perror("nftw");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}












