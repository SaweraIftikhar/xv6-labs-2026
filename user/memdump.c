#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{
int i = 0;
int pos = 0;

while (fmt[i] != '\0') {
char c = fmt[i];

if (c == 'i') {
if (pos + 4 > len){ 
printf("memdump: not enough data for 'i'\n");
return;
}

int n = *(int *)(data + pos);
printf("%d\n", n);
pos += 4;
}

else if (c == 'p') {
if (pos + 8 > len) {
printf("memdump: not enough data for 'p'\n");
return;
}

unsigned long long n = *(unsigned long long *)(data + pos);
printf("%llx\n", n);
pos += 8;
}

else if (c == 'h') {
if (pos + 2 > len) {
printf("memdump: not enough data for 'h'\n");
return;
}

short n = *(short *)(data + pos);
printf("%d\n", n);
pos += 2;
}
else if (c == 'c') {
if (pos + 1 > len) {
printf("memdump: not enough data for 'c'\n");
return;
}

printf("%c\n", data[pos]);
pos += 1;
}

else if (c == 's') {
if (pos + 8 > len) {
printf("memdump: not enough data for 's'\n");
return;
}
char *ptr = *(char **)(data + pos);
printf("%s\n", ptr);
pos += 8;
}

else if (c == 'S') {
int j = pos;

while (j < len && data[j] != '\0') {
printf("%c", data[j]);
j++;
}

printf("\n");
pos = len;
}

i++;
}
}

