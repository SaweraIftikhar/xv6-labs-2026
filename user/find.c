#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

char *cmd[MAXARG];
int cmd_count;
int exec_mode;

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

if (strcmp(p, name) == 0) {
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

