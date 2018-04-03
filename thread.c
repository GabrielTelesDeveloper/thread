#define _GNU_SOURCE
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <stdio.h>

// 64kB stack
#define FIBER_STACK 1024*64

int value = 5;

// O encadeamento filho executará essa função!
int threadFunction( void* argument )
{
    value += 10;
    printf("Valor da variável dentro da thread: %d\n", value);
    printf("saída de linha filho\n");
    return 0;
}

int main()
{
    void* stack;
    pid_t pid;
    
    // Alocar a pilha
    stack = malloc( FIBER_STACK );
    if ( stack == 0 )
    {
        perror("malloc: não foi possível alocar pilha\n");
        exit(1);
    }
    
    printf( "Criando encadeamento filho\n" );
    printf( "Valor da variável antes da Thread: %d\n", value);
    
    // Call the clone system call to create the child thread
    pid = clone( &threadFunction, (char*) stack + FIBER_STACK,
        SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0 );
    if ( pid == -1 )
    {
        perror( "clone" );
        exit(2);
    }
    
    // Wait for the child thread to exit
    pid = waitpid( pid, 0, 0 );
    if ( pid == -1 )
    {
        perror( "waitpid" );
        exit(3);
    }
    
    // Free the stack
    free( stack );
    printf("Valor final da variável: %d\n", value);
    printf("Encadeamento filho retornado e empilhamento liberado.\n");
    
    return 0;
}
