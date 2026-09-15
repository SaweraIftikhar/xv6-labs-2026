#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc,char *argv[])
{
int fd=0;
char c;
int num=0;
int number=0;

if(argc>1){
for(int i=1;i<argc;i++){
fd=open(argv[i],O_RDONLY);

if(fd<0){
printf("sixfive:cannot open\n");
printf("%s\n",argv[1]);
exit(1);
}
while(read(fd,&c,1)>0){
if(c >= '0' && c <= '9'){
num=num * 10 + c - '0';
number=1;
}
else if(strchr(" -\r\t\n./,",c)){
if(number){
if(num % 5 == 0 || num % 6 == 0)
printf("%d\n",num);

num=0;
number=0;
}
}
}

if(number && (num %5 == 0 || num %6 == 0))
printf("%d\n",num);

if(fd!=0)
close(fd);
num=0;
number=0;
}
}else{
while(read(fd,&c,1)>0){
if(c >= '0' && c <= '9'){
num=num * 10 + c - '0';
number=1;
}
else if(strchr(" -\r\t\n./,",c)){
if(number){
if(num % 5 == 0 || num % 6 == 0)
printf("%d\n",num);

num=0;
number=0;
}}}
if(number && (num %5 == 0 || num %6 == 0))
printf("%d\n",num);
}

exit(0);
}

