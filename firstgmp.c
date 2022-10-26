#include <gmp.h>
#include <stdio.h>
#include <assert.h>
/*
compile as:
gcc -o gnufib firstgmp.c -lgmp
*/
void fib(int steps, mpz_t retrn){
    int i=0;
    mpz_t x0; mpz_init(x0); mpz_set_ui(x0, 0);
    mpz_t x1; mpz_init(x1); mpz_set_ui(x1, 1);
    mpz_t x2; mpz_init(x2); mpz_set_ui(x2, 0);

    for(; i<steps; i++){
        mpz_set(x2, x1);
        mpz_set(x1, x0);
        mpz_add(x0, x1, x2);

    }
    mpz_set(retrn, x0);
}

int main(){
    mpz_t prt; mpz_init(prt);
    int steps;
    scanf("%i", &steps);
    fib(steps, prt);
    //mpz_set_ui(prt, steps
    mpz_out_str(stdout, 10, prt);
    printf("\n\n");
    return 0;
}
