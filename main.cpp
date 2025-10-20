#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
//之前的匿名管道pipe的局限性就是只能在有血缘关系的进程中使用
//而这里的fifo命名管道，是一个内核对象--->一个特殊文件(无法cat tail)，只要你这个进程有对这个文件有权限，就可以使用对应的权限
void fifo(){

    printf("%d\n",O_RDONLY);
    if (access("./a.fifo",F_OK) == -1){
        //access函数可以用来检查文件权限
        //第一次参数是文件名，表示要检查那个文件
        //第二个参数是权限名
        //F_OK，是否存在
        //R_OK，是否可读
        //W_OK，是否可写
        //X_OK，是否可执行
        //是就返回0，不是就返回-1
        if (mkfifo("./a.fifo",0666) == -1){
            //第二个参数是设置权限的，0表示是八进制
            //分别是三组：User（文件所有者），Group（文件所属组），Others（其他用户）
            //6表示可读可写，不可执行
            // | 权限组合  | 二进制   | 十进制 | 含义           |
            // | ----- | ----- | --- | ------------ |
            // | `---` | `000` | 0   | 无任何权限        |
            // | `--x` | `001` | 1   | 仅执行          |
            // | `-w-` | `010` | 2   | 仅写           |
            // | `-wx` | `011` | 3   | 写 + 执行       |
            // | `r--` | `100` | 4   | 仅读           |
            // | `r-x` | `101` | 5   | 读 + 执行       |
            // | `rw-` | `110` | 6   | ✅ 读 + 写，不可执行 |
            // | `rwx` | `111` | 7   | 读 + 写 + 执行   |

            fputs("mkfifo failed!\n",stderr);
            exit(-1);
        }
    }
    pid_t pid = fork();
    if (pid > 0){
        int fd = open("./a.fifo",O_RDONLY);//父进程设置只读权限
        if (fd == -1){
            fputs("open failed!\n",stderr);
            exit(-1);
        }
        char buffer[512];
        int len = read(fd,buffer,sizeof(buffer));
        fprintf(stdout,"len:%d\n",len);
        fprintf(stdout,"buffer is:%s\n",buffer);
        close(fd);
    }else if (pid == 0){
        int fd = open("./a.fifo",O_WRONLY);
        if (fd == -1){
            fputs("open failed!\n",stderr);
            exit(-1);
        }
        char str[512];
        // scanf("%s",str);
        fgets(str,sizeof(str),stdin);
        int len = write(fd, str,strlen(str)+1);
        fprintf(stdout,"len:%d\n",len);
        close(fd);
    }else{
        fputs("fork failed!\n",stderr);
        exit(-1);
    }
}
int main(){
    fifo();
    return 0;
}