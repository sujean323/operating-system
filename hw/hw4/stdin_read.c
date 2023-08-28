#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    char* buf = calloc(2, sizeof(char));
    long len = 1;
    long ret;
    
    printf("使用 'syscall' 呼叫system call\n");
    __asm__ volatile ( 
        "mov $0, %%rax\n"   //system call number
        "mov $2, %%rdi\n"   //stderr
        "mov %1, %%rsi\n"   //
        "mov %2, %%rdx\n"
        "syscall\n"
        "mov %%rax, %0"
        :  "=m"(ret)
        : "g" (buf), "g" (len)
        : "rax", "rbx", "rcx", "rdx");
    printf("讀入的字元為：%c\n", buf[0]);
}
