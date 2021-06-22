#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char **tokens;
long long ntokens;

int
LoadTokens(char *path)
{
  FILE *f;
  long long id, max;
  char token[1024];
  
  //printf("# path: %s\n", path);
  f = fopen(path, "r");
  if (f == NULL) {
    printf("Input file %s not found\n", path);
    return(-1);
  }
  max = -1;
  while (1) {
    fscanf(f, "%s %lld\n", &token, &id);
    if (feof(f)) break;
    //printf("%lld %s\n", id, token);
    if (max < id) max = id;
  }
  fclose(f);
  
  //printf("max: %lld\n", max);
  ntokens = max+1;
  tokens = (char **) malloc(ntokens*sizeof(char *));

  f = fopen(path, "r");
  while (1) {
    fscanf(f, "%s %lld\n", &token, &id);
    if (feof(f)) break;
    tokens[id] = strdup(token);
    //printf("%lld %s\n", id, tokens[id]);
  }
  fclose(f);
  
  return(0);
}

long long
CheckToken(char *token)
{
  long long id;
  for (id = 0; id < ntokens; id++) {
    if (strcmp(tokens[id], token) == 0) return(id);
  }
  id = -1;
  return(id);
}

void
DumpMatrix(long long id, long long size, float *M)
{
  long long i;
  printf("%lld", id);
  for (i = 0; i < size; i++) {
    printf(" %f", M[i]);
  }
  printf("\n");
  fflush(stdout);
}

const long long max_w = 50; // max length of vocabulary entries

int
ScanBinFile(char *path,
	    long long (*cfunc)(char *),
	    void (*dfunc)(long long, long long, float *))
{
  FILE *f;
  long long id;
  float len;
  long long words, size;
  long long a, b, c, d;
  float *M;
  char *vocab;
  f = fopen(path, "rb");
  if (f == NULL) {
    printf("Input file %s not found\n", path);
    return(-1);
  }
  fscanf(f, "%lld", &words);
  fscanf(f, "%lld", &size);
  //printf("words: %lld\n", words);
  //printf("size: %lld\n", size);
  printf("%lld %lld\n", ntokens, size);
  vocab = (char *)malloc((long long)words * max_w * sizeof(char));
  M = (float *)malloc((long long)words * (long long)size * sizeof(float));
  if (M == NULL) {
    printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
    return -1;
  }
  for (b = 0; b < words; b++) {
    a = 0;
    while (1) {
      vocab[b * max_w + a] = fgetc(f);
      if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
      if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
    }
    vocab[b * max_w + a] = 0;
    for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
    len = 0;
    for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
    len = sqrt(len);
    for (a = 0; a < size; a++) M[a + b * size] /= len;

    // Akito
    id = (*cfunc)(&vocab[b * max_w]);
    if (id >= 0) {
      (*dfunc)(id, size, &M[b * size]);
    }
  }
  fclose(f);
  return(0);
}

int
main(int argc, char *argv[])
{
  LoadTokens(argv[1]);
  /*
  long long i;
  for (i = 0; i < ntokens; i++) {
    printf("%s\n", tokens[i]);
    if (strcmp(tokens[i], "AAI") == 0) {
      printf("id: %lld\n", i);
      break;
    }
  }
  */
  ScanBinFile(argv[2], CheckToken, DumpMatrix);
  exit(0);
}
