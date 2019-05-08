#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(getopt)
extern int optind, opterr;
extern char *optarg;
extern int getopt(int argc, char *argv[], char *optstring);
#endif

#if !defined(main)
#undef main
#endif

#define BUFFERSIZE 1024*1024   // 1MB 
char buffer1[BUFFERSIZE];
char buffer2[2*BUFFERSIZE];
static char usage[] = "usage: %s -T:pattern -x:prelen -y:postlen \n";
char epsilon[2] = {0x03, 0xb5};

char *pattern, *pretext, *posttext;
int pflag = 0, xflag = 0, yflag = 0;
int patternlen =0, prelen = 0, postlen = 0;

#if !defined (min)
int min(int one, int two)
{
	if (one < two)
		return one;
	else
		return two;
};
#endif

int printpattern(char* lbound, char* firstpat, char* rbound)
{
	int llen = prelen;
	int rlen = postlen;
	char *x = firstpat - prelen;
	char *y;
   
	if (x < lbound) {
		x = lbound;
		llen = (int)(firstpat - lbound);
	};

	y = firstpat + patternlen + postlen;
    if ( y > rbound ) {
		rlen = (int) (rbound - firstpat - patternlen);
	}
	    
	strncat(pretext, x, llen);
	strncat(posttext, (firstpat+patternlen), rlen);
	if ( rlen > 2) {  // remove epsilon from the end if it exist
        if ( posttext[rlen-1] == '\0x03') {
    	    if ( posttext[rlen-2] == '\0xb5') {
    		    posttext[rlen-2] = '\0';
    		    posttext[rlen-1] = '\0';
    	    }
        }
	}
    fprintf(stdout, "%s %s %s\n", pretext, pattern, posttext);

	return 0;
};

int main(int argc, char* argv[])
{
    extern char *optarg;
    extern int optind;
    char c;
    char *loc1, *loc2, *lbound, *rbound, *ptr1, *ptr2;
    while ((c = getopt(argc, argv, "T:x:y:")) != -1)
        switch (c) {
        case 'T':
	        pflag = 1;
	        pattern = optarg;
	        patternlen = strlen(pattern);
            break;
        case 'x':
	        xflag = 1;
            pretext = optarg;
            prelen = atoi(pretext);
	        break;
        case 'y':
	        yflag = 1;
            posttext = optarg;
	        postlen = atoi(posttext);
            break;
        }

    if ( pflag == 0)  {
        fprintf(stderr, "%s: missing -T option\n", argv[0]);
        fprintf(stderr, usage, argv[0]);
        exit(1);
    }else if (xflag == 0) {
       fprintf(stderr, "%s: missing -x option\n", argv[0]);
       fprintf(stderr, usage, argv[0]);
       exit(1);
    }else if (yflag == 0) {
      fprintf(stderr, "%s: missing -y option\n", argv[0]);
      fprintf(stderr, usage, argv[0]);
      exit(1);
    }

    ptr1 = buffer1;
    ptr2 = buffer2;
    lbound = buffer1;
    rbound = buffer1 + BUFFERSIZE;
  
    if ( fgets(ptr1, BUFFERSIZE, stdin) == NULL)
	exit(0);

    do {
    	char* lbound  = ptr1;
    	int actual_len = strlen(ptr1);
    	int leftover_len = actual_len;
		do {
			loc1 = strstr(ptr1, pattern);//search pattern
			if (loc1 != NULL) { //found pattern at loc1
			    printpattern(lbound, loc1, rbound);
			    leftover_len -= ( loc1 - ptr1);
			    ptr1 = loc1+1;
			}
		}while (loc1 != NULL); //loop to look for more overlapped patterns
		

		//deal with left over if it exist
		if ( leftover_len > 0) {
			if ( leftover_len > (prelen + patternlen) )
				leftover_len = (prelen + patternlen); //only need examine at most this many left over bytes for next buffer
			
			ptr1 = buffer1 + (actual_len - leftover_len);
			buffer2[0] = '\0';
			strcat(ptr2, ptr1);
			lbound = buffer2;		
			rbound = buffer2 + 2*BUFFERSIZE;
		}
		
		//read next round of data to buffer
		ptr1 = buffer1;
		if ( fgets(ptr1, BUFFERSIZE, stdin) == NULL ) 
			exit(0);

		if ( leftover_len > 0 ) {
			//deal with data between two reads
			strncat(ptr2, ptr1, min(BUFFERSIZE, patternlen+prelen));
			do {
				loc2 = strstr(ptr2, pattern);
				if ( loc2 == NULL)
					break;
				
				if ( loc2 >= (ptr2 + leftover_len + prelen))
					break;//leftover bytes will not be in pattern or in pretext
				
				if (loc2 != NULL) { // found pattern between two buffers
					printpattern(lbound, loc2, rbound);
					ptr2 = loc2;
				}
			}while (loc2 != NULL); //loop to look for overlapped patterns

    	}

		//go processing the rest of the newly read in buffer
    } while(1);

};
