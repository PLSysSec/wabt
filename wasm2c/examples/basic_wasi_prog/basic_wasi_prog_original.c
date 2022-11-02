#include <stdio.h>
#include <stdlib.h>

void indirect_call_target1(int* ptr) {
    printf("Called indirect_call_target1 with val: %d\n", *ptr);
}

void indirect_call_target2(int* ptr) {
    printf("Called indirect_call_target2 with val: %d\n", *ptr);
}

int main(int argc, char const *argv[])
{
    printf("Hello!\n");

    printf("Calling malloc\n");
    int* ptr = malloc(sizeof(int));

    *ptr = 5;

    typedef void (indirect_call_t)(int* ptr);

    // Add some conditions to prevent inlining
    indirect_call_t* call_ptr = &indirect_call_target1;
    if (argc % 2 == 1) {
        call_ptr = &indirect_call_target2;
    }

    printf("Calling an indirect function\n");
    call_ptr(ptr);

    printf("Calling free\n");
    free(ptr);

    return 0;
}
