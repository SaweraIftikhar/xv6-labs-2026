#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

char *cmd[MAXARG];
int cmd_count;
int exec_mode;

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
if(re[0] == '^')
return matchhere(re+1, text);
do{
if(matchhere(re, text))
return 1;
}while(*text++ != '\0');
return 0;
}

int
matchhere(char *re, char *text)
{
if(re[0] == '\0')
return 1;
if(re[1] == '*')
return matchstar(re[0], re+2, text);
if(re[0] == '$' && re[1] == '\0')
return *text == '\0';
if(*text!='\0' && (re[0]=='.' || re[0]==*text))
return matchhere(re+1, text+1);
return 0;
}

int
matchstar(int c, char *re, char *text)
{
do{
if(matchhere(re, text))
return 1;
}while(*text!='\0' && (*text++==c || c=='.'));
return 0;
}

void
run_command(char *path)
{
int pid;

pid = fork();

if (pid == 0) {
cmd[cmd_count] = path;
cmd[cmd_count + 1] = 0;
exec(cmd[0], cmd);
printf("find: exec failed\n");
exit(1);
}

wait(0);
}

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

if (st.type == T_FILE || st.type == T_DIR) {
p = path;

while (*p)
p++;

while (p > path && *(p - 1) != '/')
p--;

if (match(name, p)) {
if (exec_mode)
run_command(path);
else
printf("%s\n", path);
}
}

if (st.type != T_DIR) {
close(fd);
return;
}

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
int i;

if (argc < 3) {
printf("Usage: find path name [-exec command]\n");
exit(1);
}

exec_mode = 0;
cmd_count = 0;

if (argc >= 5 && strcmp(argv[3], "-exec") == 0) {
exec_mode = 1;
cmd_count = argc - 4;

for (i = 0; i < cmd_count; i++)
cmd[i] = argv[i + 4];
}

find(argv[1], argv[2]);

exit(0);
}
