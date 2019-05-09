1. This code has been tested on ubuntu linux 18.04

2. To compile it, just run following command

   gcc matchseq.c

   then the executable will be produced as a.out

3. To run the test, run the following command:

     cat test1.txt | ./a.out -T:AGTA -x:5 -y:7

                    
4. future improvements needed:

   4.1 the way I detect the epsilon is a kind of hacky. A better way would be calling some encoding library
       to process the test files based on its encoding used. Here my test file is encoded in UTF8

   4.2 I am using the naive search function of strstr() as the basic matching algorithm. A better way
       would be using a custom made variation of the KMP, Rabin-Karp, or Boyer Moore algorithm for this 
       purpose. 

