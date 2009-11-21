#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/*
 * A simple program to read a binary file and output it
 * as a sequence of decimal representations of the binary words
 * Options:
 *   -n
 *      numbered words in output 
 *   -s <size of word>
 *      1,2,3 or 4 bytes; default is 1
 */

//Words to read at once
const unsigned BUFF_SIZE=255;

//Neatly tucked configurations in a struct
struct
Conf {
  char n_;        //byte numbering (treated like bool)
  char bpw_;      //bytes per word
  unsigned bpb_;  //bytes per buffer
  FILE* inp_;     //input file
};

void
usage()
{
    fprintf(stderr,
	    "bin2dec [-n] [-s wordsize] sourcefile [outputfile]\n");
}

void
read_opts(int argc, char** argv, Conf* conf)
{
  char ch, bpw;
  while( (ch = getopt(argc, argv, "ns:")) != EOF) {
    if(ch == 'n') conf->n_=true;
    if(ch == 's') {
        bpw=atoi(optarg);
        if(bpw<=4 && bpw>=1) {
          conf->bpw_=bpw;
          conf->bpb_=bpw*BUFF_SIZE;
        }
        else {
          fprintf(stderr,
              "bin2dec: invalid number of bytes per word: %d (should be a number from 1 to 4)\n",
              bpw);
          exit(1);
        }
    }
  }
}

void
open_input(int argc, char** argv, Conf* conf)
{
  if( NULL == (conf->inp_ = fopen(argv[optind], "rb")) ) {
    fprintf(stderr,"bin2dec: cannot access %s: No such file or directory\n", argv[optind]);
    exit(1);
  }
}
void
print_bin(unsigned x)
{
  for(int i=31; i; --i) printf("%d", (x&(1<<i))?1:0);
  printf("\n");
}
void
print_bytes(const Conf* conf)
{
  unsigned br;
  unsigned word;
  char *buff = (char*)malloc(conf->bpb_);
  if( NULL == buff ) {
          fprintf(stderr, "Out of memory");
          exit(1);
  }
  while( (br = fread(buff, sizeof(char), conf->bpb_, conf->inp_)) ) {
      for(unsigned w=0; w<br; w+=conf->bpw_) {//for each word 
        word = 0x00000000;
        for(unsigned b=0; b<conf->bpw_; ++b) { //for each byte in the word
          word <<= 8;
          word ^= (unsigned)buff[w+b];
        }
        if(conf->n_)
          printf("%ld: ", ftell(conf->inp_)+w/conf->bpw_);
        printf("%ld\n", word);
      }
  }
}

int
main(int argc, char** argv)
{
  Conf conf = { 0, 1, BUFF_SIZE, NULL };
  read_opts( argc, argv, &conf );
  open_input( argc, argv, &conf );
  print_bytes( &conf );
  return 0;
}
