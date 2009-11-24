#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/* Friendly File Dumper
 * Usage:
 *      ffd [-nbw] [-s <size of word>] <file>
 * A simple program to read a binary file and dump it
 * as a sequence of decimal/binary representations of the binary words.
 * Options:
 *   -n
 *      Numbered words in output 
 *   -s <size of word>
 *      1,2,3 or 4 bytes; default is 1
 *   -b
 *      Binary mode. Display bits, grouped in bytes of 8.
 *   -w
 *      Turn *off* the warning message for an incomplete word in the end of the file.
 *      An incomplete word occurs when the number of bytes in the input file is not a multiple of the word size)
 */

/*Words to read at once*/
const unsigned BUFF_SIZE=256;
/*Bits per byte*/
const char BYTE_SIZE=8;

/*Neatly tucked configurations in a struct*/
typedef
struct
{
  char n_;        /*byte numbering (treated like bool)*/
  char b_;        /*binary dump (bool)*/
  char bpw_;      /*bytes per word*/
  char w_;        /*incomplete byte warning (bool)*/
  unsigned bpb_;  /*bytes per buffer*/
  FILE* inp_;     /*input file*/
  char* put_;     /*put pointer*/
} Conf;

void
usage()
{
    fprintf(stderr,
	    "Usage:\nffd [-n] [-s wordsize] sourcefile [outputfile]\n");
}

void
read_opts(int argc, char** argv, Conf* conf)
{
  extern char* optarg;
  char ch, bpw;
  while( (ch = getopt(argc, argv, "nbws:")) != EOF) {
    if(ch == 'n')
      conf->n_=1;
    if(ch == 'w')
      conf->w_=0;
    if(ch == 's') {
        bpw=atoi(optarg);
        if(bpw<=4 && bpw>=1) {
          conf->bpw_=bpw;
          conf->bpb_=bpw*BUFF_SIZE;
        }
        else {
          fprintf(stderr,
              "ffd: invalid number of bytes per word: %d (should be a number from 1 to 4)\n",
              bpw);
          exit(1);
        }
    }
    if(ch == 'b') {
      conf->b_=1;
    }
  }
}

void
open_input(const char* fname, Conf* conf)
{
  if( NULL == fname ) {
    usage();
    exit(1);
  }
  if( NULL == (conf->inp_ = fopen(fname, "rb")) ) {
    fprintf(stderr,"ffd: cannot access %s: No such file or directory\n", fname);
    exit(1);
  }
}

void
close_input(Conf* conf)
{
  if(conf->inp_!=NULL)
    fclose(conf->inp_);
} 

/* Prints (in the appropriate number of positions) the word as a binary number */
void
get_bin_str(unsigned x, const Conf* conf, char* p)
{
  int i;
  for(i=BYTE_SIZE*conf->bpw_; i; --i) {
    if(0 == i%BYTE_SIZE) {
      *p++=' ';
    }
    *p++=(1 << (i-1))&x?'1':'0';
  }
  *p++='\0';
}

void
write_output(const Conf* conf)
{
  unsigned br, word, c=0;
  char *buff = (char*)malloc(conf->bpb_);
  char *bin_str = (char*)malloc(conf->bpb_*BYTE_SIZE);

  if( NULL == buff || NULL == bin_str ) {
          fprintf(stderr, "Out of memory");
          exit(1);
  }
  while( (br = fread(buff, sizeof(char), conf->bpb_, conf->inp_)) ) {
      unsigned w;
      char b;

      for(w=0; w<br; w+=conf->bpw_) {/*for each word */
        word = 0x00000000;
        for(b=0; b<conf->bpw_; ++b) { /*for each byte in the word*/
          word <<= BYTE_SIZE; /* next byte */
          word ^= (unsigned)buff[w+b]; /* put this as b-th byte of the word */
          if(w+b==br-1 && b<conf->bpw_-1) {
            if(conf->w_) printf("[Incomplete word] ");
            break;
          }
        }
        if(conf->n_)
          fprintf(stdout, "%u: ", ++c);
        if(conf->b_) {
          get_bin_str(word, conf, bin_str);
          printf("%s\n", bin_str);
        }
        else printf("%u\n", word);
      }
  }
  free(bin_str);
  free(buff);
}

int
main(int argc, char** argv)
{
  extern int optind;
  Conf conf = { 0, 0, 1, 1, BUFF_SIZE, NULL, NULL };
  read_opts( argc, argv, &conf );
  open_input( argv[optind], &conf );
  write_output( &conf );
  close_input( &conf );
  return 0;
}
