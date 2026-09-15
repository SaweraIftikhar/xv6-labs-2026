#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void
find(char *path, char *name)
{
char buf[512], *p;
int fd;
struct dirent de;
struct stat st;

if ((fd = open(path, O_RDONLY)) < 0) {
printf("find: cannot open %s\n", path);
return;
}

if (fstat(fd, &st) < 0) {
printf("find: cannot stat %s\n", path);
close(fd);
return;
}

if (st.type == T_FILE) {
p = path;
while (*p)
p++;

while (p > path && *(p - 1) != '/')
p--;

if (strcmp(p, name) == 0)
printf("%s\n", path);

close(fd);
return;
}

if (st.type != T_DIR) {
close(fd);
return;
}

p=path;
while(*p)
p++;

while(p > path && *(p - 1) != '/')
p--;

if(strcmp(p,name) == 0)
printf("%s\n", path);

strcpy(buf, path);
p = buf + strlen(buf);

if (p < buf + sizeof(buf) - 1) {
*p++ = '/';
*p = 0;
}

while (read(fd, &de, sizeof(de)) == sizeof(de)) {
if (de.inum == 0)
continue;

if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
continue;

if (p + strlen(de.name) >= buf + sizeof(buf))
continue;

strcpy(p, de.name);
find(buf, name);
}

close(fd);
}

int
main(int argc, char *argv[])
{
if (argc != 3) {
printf("Usage: find path name\n");
exit(1);
}

find(argv[1], argv[2]);

exit(0);
}

