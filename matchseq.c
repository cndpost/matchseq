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


//#define DEBUG

#ifdef DEBUG
#define debug_print1(v0)     		printf(v0)
#define debug_print2(v0, v1)     	printf(v0, v1)
#define debug_print3(v0, v1, v2) 	printf(v0, v1, v2)
#define debug_print4(v0, v1, v2, v3) printf(v0, v1, v2, v3)
#define debug_print5(v0, v1, v2, v3, v4) printf(v0, v1, v2, v3, v4)
#else
#define debug_print1(v0)     	
#define debug_print2(v0, v1)     	
#define debug_print3(v0, v1, v2) 	
#define debug_print4(v0, v1, v2, v3) 
#define debug_print5(v0, v1, v2, v3, v4) 
#endif

#if !defined (min)
int min(int one, int two)
{
	if (one < two)
		return one;
	else
		return two;
};
#endif

#define BUFFERSIZE 1024*1024   // 1MB 
#define PRETEXTSIZE 1024
#define POSTTEXTSIZE 1024
#define PATTERNSIZE 1024

char buffer1[BUFFERSIZE];
char buffer2[2*BUFFERSIZE];
char pretext[PRETEXTSIZE];
char posttext[POSTTEXTSIZE];
char pattern[PATTERNSIZE];

static char usage[] = "usage: %s -T:pattern -x:prelen -y:postlen \n";
char epsilon[2] = {0xb5, 0x0ce}; //UTF8 encoding

//char *pattern, *pretext, *posttext;
int pflag = 0, xflag = 0, yflag = 0;
int patternlen =0, prelen = 0, postlen = 0;



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
	
    debug_print3(" firstpat, prelen = %s  %d\n", firstpat, prelen);
    debug_print3(" x y = %s | %s\n", x, y);
    debug_print5("lbound llen rlen rbound = %x %d %d %x\n", lbound, llen, rlen, rbound);
    pretext[0] = '\0';
    posttext[0] = '\0';
    strncat(pretext, x, llen);
	strncat(posttext, (firstpat+patternlen), rlen);
	debug_print4("posttext = %s len = %d rlen= %d\n", posttext, strlen(posttext), rlen);
	if ( rlen > 2) {  // remove epsilon from the end if it exist
        debug_print4("*-- %x %x %x \n", posttext[rlen-3], posttext[rlen-2], posttext[rlen-1]);

        //my hacking way to detect the epsilon.
        unsigned char c = posttext[rlen-2];
		unsigned int dc = c;
        if (  dc  == 181 ) {  
        	debug_print1("*****we got the epsilon \n");
    		posttext[rlen-3] = '\0';
    		posttext[rlen-2] = '\0';
        }
     //   char epsilon = 'ε';
	}
    debug_print4("pretext pattern posttest = %s %s %s\n", pretext, pattern, posttext);
    fprintf(stdout, "%s %s %s\n", pretext, pattern, posttext);

	return 0;
};

//int debug_print(char* fmt,  ...)
//{
//	fprintf(stderr, "%s", fmt);
//}


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
	        strcpy(pattern,optarg+1);
	        patternlen = strlen(pattern);
	        debug_print4("%s: got -T option pattern=%s len=%d\n", argv[0], pattern, patternlen);
             
	        break;
        case 'x':
	        xflag = 1;
            strcpy(pretext,optarg+1);
            prelen = atoi(pretext);
            debug_print4("%s: got -x option pretext=%s len=%d\n", argv[0], pretext, prelen);
	        break;
        case 'y':
	        yflag = 1;
            strcpy(posttext,optarg+1);
	        postlen = atoi(posttext);
	        debug_print4("%s: got -y option posttext=%s len=%d\n", argv[0], posttext, postlen);
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
  
    if ( fgets(ptr1, BUFFERSIZE, stdin) == NULL) {
    	debug_print1("end of data \n");
    	exit(0);
	} else {
		debug_print2("got input : %s\n", ptr1);		
	}

    do {
    	char* lbound  = ptr1;
    	int actual_len = strlen(ptr1);
    	int leftover_len = actual_len;
    	debug_print1("do loop 1 \n");

		do {
			debug_print2("do loop 2 ptr1 = %x\n", ptr1);
			loc1 = strstr(ptr1, pattern);//search pattern
			debug_print4("%s %s %x\n", ptr1, pattern, (unsigned int)loc1);
			if (loc1 != NULL) { //found pattern at loc1
		    	debug_print3("found %s at %x \n", pattern, (unsigned int)loc1);
				
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
		    	fprintf(stderr, "do loop 3 \n");
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

    debug_print1("end of data \n");

};
