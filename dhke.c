#include <gmp.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "socket.c"

/*
"/home/lucas/Desktop/C/gmp/cryptography/DH Key exchange"

gcc -o largeprime dhke.c -lgmp -lm
*/
/*
void generatekeypairs(mpz_t k1, mpz_t k2){
    mpz_init(k1);
    mpz_init(k2);
    mpz_urandomb(k1, time(0), 256);
}
*/

void generatep(int bits, mpz_t rand){
    // *********************** VARIABLE DECLARATION *********************** //
    // initilize the variables as gmp class instances
    mpz_t l;
    unsigned long seed=time(0);
    // perform inits to create variable pointers with 0 value
    mpz_init(l); mpz_init(rand);
    //mpz_inits(l, rand);

    // calculate the random number floor
    mpz_ui_pow_ui(l, 2, bits-1);

    // initilze the state object for the random generator functions
    gmp_randstate_t rstate;
    // initialize state for a Mersenne Twister algorithm. This algorithm is fast and has good randomness properties.
    gmp_randinit_mt(rstate);

    // create the generator seed for the random engine to reference
    gmp_randseed_ui(rstate, seed);

    /*
    Function:
    int mpz_probab_prime_p (const mpz_t n, int reps)

    Determine whether n is prime. Return 2 if n is definitely prime, return 1 if n is probably prime (without being certain),
    or return 0 if n is definitely composite.
    */
    do {
        // return a uniformly distributed random number in the range 0 to n-1, inclusive.
        mpz_urandomb(rand, rstate, bits);

        // add the random number to the low number, which will make sure the random number is between the low and high ranges
        mpz_add(rand, rand, l);

        //gmp_printf("randomly generated number: %Zd\n", rand);

    } while ( !(mpz_probab_prime_p(rand, 256)) );
    // *********************** GARBAGE COLLECTION *********************** //
    // empty the memory location for the random generator state
    gmp_randclear(rstate);
    // clear the memory locations for the variables used to avoid leaks
    mpz_clear(l);
    //gmp_printf("randomly generated prime: %Zd\n", rand);
}

void modexp(mpz_t ptr, mpz_t p, mpz_t q, mpz_t n, int nbits){
    //computes p^n mod q
    mpz_t ncopy; mpz_init(ncopy); mpz_set(ncopy, n); //qcopy=q;
    mpz_t pcopy; mpz_init(pcopy); mpz_set(pcopy, p);
    mpz_init(ptr);

    int i=0;
    while(((bool)mpz_divisible_ui_p(n, 2))){
        mpz_mul(p, p, p);
        mpz_mod(p, p, q);
        mpz_div_ui(n, n, 2);
        i++;
    }
    mpz_set(ptr, p);
    mpz_div_ui(n, n, 2);
    mpz_mul(p, p, p);
    mpz_mod(p, p, q);
    i++;
    for(; i<nbits; i++){
        if(!((bool)mpz_divisible_ui_p(n, 2))){
            mpz_mul(ptr, ptr, p);
            mpz_mod(ptr, ptr, q);//rvalue=rvalue*p mod(q)
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, q); //p*p mod q
        mpz_div_ui(n, n, 2);//dividing n by 2
    }
    mpz_set(n, ncopy); mpz_clear(ncopy);//deleting garbage
    mpz_set(p, pcopy); mpz_clear(pcopy);//deleting garbage
}


void generatekeypairs(int bits, mpz_t p_ptr, mpz_t q_ptr){

    while(1){
        mpz_t q;
        generatep(bits, q);
        mpz_t n;
        mpz_set(n, q);
        mpz_sub_ui(n, n, 1);
        int tcount=0;
        int max=(int)(log(bits)/log(2));
        for(; tcount<max; tcount++){
            mpz_t p, mod;
            generatep(bits/2, p);
            modexp(mod, p, q, n, bits);
            if(mpz_cmp_ui(mod, 1)==0){ //mpz_cmp: returns 1 if op1>op2; returns 0 if op1==op2; returns -1 if op1<op2;
                mpz_clear(mod);
                mpz_init(p_ptr); mpz_set(p_ptr, p);
                mpz_init(q_ptr); mpz_set(q_ptr, q);
                mpz_clear(q);
                mpz_clear(p);
                return;//probably we hit an unique ciclyc group
            }
            mpz_clear(mod);
            mpz_clear(p);
        }
        mpz_clear(q);
    }
}

void setDefaultKeyPairs(mpz_t p, mpz_t q){
/*

this step is to avoid the large computations needed in function generatekeypairs(), and make key exchange faster by standarizing p and q

The selected keypairs are: p=95a402fb3e9bfa2338f7d3d0ab07128b122ab0e06a93e50730094e1b7daa58b23796eeaa69c335c5cb1aa102732846e789089b39b063d1ac19ae723
1dfaf981fede27f13cc08bb87e765e261af6a09cd86265d7e930f91512c46037793871eea4cb03a7910d2eb6f59324306aaf49a9d41125dbd5728403154df122a4ab18045b5ce0f241f5
68566e13664b95066e5d228018c41109d6f13b300952873c184b666cfc70362cd5cfb384aed1dad056e5968b87b4bba5f8f4b59e7e5135900455057ae485709fdafdffb7d6ecab166504
1cc7219acb4cfe7372584cde3e34757677ba751e67792b4df2efecec26c51acee690a01ec4ec144478714bee59fcfc421

q=b3913089e9816cf25699cd76d4c050675bb3fd787d8d84e2d91e33a384dbe3c44e3b929bc542da34b6f641aa98e22bb98ddd0b34464690034e50844d8965a1c031324270568a41f418
1dec32bf61f10f5bc504d8f8d6b251d8c6f5c6da579f18a14f63cc9b0486b3531a1b367bb7a16dcd3777e2d99e61ab4f3e54d68bb92597d091d1b773ba4790e62b3e4ed19859658f8745
d12cc6484be3e6df85b22b1c597db56887d2f3cd1acb932bb724c3b624228e4be85f6e1c083b1509a256c5d8aba8b6f1ff96b4afcdad08855ffca549f52dfd6b02035d3070448cf99f72
e161fff6059f9e412d7bb23ce6638101ae7362cbf15b947a9ef40ce09b8aad44526ebee034b25e5ff54d279510118e6650e2e3bb6d88e16c5403d064affba6090400b4c873a7b80d014c
d0ae853c764efd859b29621e0de6acfc5f5952b7d2466c1365c9bdfaea7e03b69f8856596fb99806fe89804c757c3f5c5ded977e1f2e3d1906823451c494faaec2f37a8a38d763e4c847
b3d3a7aab8a55a7ab81cc40f3e1687602207d4fb706d849533369ff363566504b63e152df64bf869d01280e7c6581a2c181c0867751b35d5115842d0b5c0a189cc29622e8394db18ee40
0e02a7f709015db32ebf0506faaba46edf079979ed0f5d343479b1b1981a8a05ea0608e82ac49e5a334f47c97dd29a11d86c780e73b271ee511f70e3ac990c40e552bfb3c7
*/
mpz_init(p); mpz_init(q);
mpz_set_str(p, "95a402fb3e9bfa2338f7d3d0ab07128b122ab0e06a93e50730094e1b7daa58b23796eeaa69c335c5cb1aa102732846e789089b39b063d1ac19ae7231dfaf981fede2\
7f13cc08bb87e765e261af6a09cd86265d7e930f91512c46037793871eea4cb03a7910d2eb6f59324306aaf49a9d41125dbd5728403154df122a4ab18045b5ce0f241f568566e13664b9\
5066e5d228018c41109d6f13b300952873c184b666cfc70362cd5cfb384aed1dad056e5968b87b4bba5f8f4b59e7e5135900455057ae485709fdafdffb7d6ecab1665041cc7219acb4cf\
e7372584cde3e34757677ba751e67792b4df2efecec26c51acee690a01ec4ec144478714bee59fcfc421", 16);
mpz_set_str(q, "b3913089e9816cf25699cd76d4c050675bb3fd787d8d84e2d91e33a384dbe3c44e3b929bc542da34b6f641aa98e22bb98ddd0b34464690034e50844d8965a1c03132\
4270568a41f4181dec32bf61f10f5bc504d8f8d6b251d8c6f5c6da579f18a14f63cc9b0486b3531a1b367bb7a16dcd3777e2d99e61ab4f3e54d68bb92597d091d1b773ba4790e62b3e4e\
d19859658f8745d12cc6484be3e6df85b22b1c597db56887d2f3cd1acb932bb724c3b624228e4be85f6e1c083b1509a256c5d8aba8b6f1ff96b4afcdad08855ffca549f52dfd6b02035d\
3070448cf99f72e161fff6059f9e412d7bb23ce6638101ae7362cbf15b947a9ef40ce09b8aad44526ebee034b25e5ff54d279510118e6650e2e3bb6d88e16c5403d064affba6090400b4\
c873a7b80d014cd0ae853c764efd859b29621e0de6acfc5f5952b7d2466c1365c9bdfaea7e03b69f8856596fb99806fe89804c757c3f5c5ded977e1f2e3d1906823451c494faaec2f37a\
8a38d763e4c847b3d3a7aab8a55a7ab81cc40f3e1687602207d4fb706d849533369ff363566504b63e152df64bf869d01280e7c6581a2c181c0867751b35d5115842d0b5c0a189cc2962\
2e8394db18ee400e02a7f709015db32ebf0506faaba46edf079979ed0f5d343479b1b1981a8a05ea0608e82ac49e5a334f47c97dd29a11d86c780e73b271ee511f70e3ac990c40e552bf\
b3c7", 16);
}

void generatepk0(mpz_t ptr, mpz_t numberq){
    printf("enter some random characters, arround 2000\n");
    char * password=malloc(65536);
    memset(password, 0, 65536);
    unsigned long int strsize=65536;
    getline(&password, &strsize, stdin);
    int i=0;
    char hex[]="0123456789abcdef";
    //printf("%s\n%lu\n", password, strsize);
    for(; i<strsize; i++){
        if(password[i]==0){
            break;
        }
        password[i]=hex[password[i]%16];
    }
    password=realloc(password, strsize);
    //printf("%s\n", password);
    mpz_init(ptr);
    mpz_set_str(ptr, password, 16);
    mpz_mod(ptr, ptr, numberq);
    gmp_printf("\n\nYour generated password is:%Zu\n", ptr);
}

void exchangekeydemo(mpz_t ptr, mpz_t pk0, mpz_t p, mpz_t q){

    mpz_t g0;
    modexp(g0, p, q, pk0, 4096);
    /*
        exchange keys via internet, send g and recv the other g
    */
    mpz_t pk1;
    generatepk0(pk1, q);
    mpz_t g1;
    modexp(g1, p, q, pk1, 4096);
    mpz_t ptr1;
    modexp(ptr1, g0, q, pk1, 4096);
    gmp_printf("the resultant key'1 is: %Zu\n", ptr1);
    /*
        end of exch keys over the internet
    */

    modexp(ptr, g1, q, pk0, 4096);
    gmp_printf("the resultant key'0 is: %Zu\n", ptr);
    if(mpz_cmp(ptr, ptr1)==0){
        printf("son iguales\n");
        return;
    }
    printf("no so iguales\n");
}

void DFKXCHR(mpz_t ptr, mpz_t pk0, mpz_t p, mpz_t q, SOCKET request){//diffie-hellman key exchange request

    mpz_t g0;
    modexp(g0, p, q, pk0, 4096);
    /*
        exchange keys via internet, send g and recv the other g
    */

    char * g=mpz_get_str(NULL, 16, g0);
    char * initg1=xch_g(g, strlen(g), request);
    mpz_t g1; mpz_init(g1); mpz_set_str(g1, initg1, 16); //g1=recv()
    free(g);
    free(initg1);
    /*
        end of exch keys over the internet
    */

    modexp(ptr, g1, q, pk0, 4096);
    gmp_printf("the resultant key'0 is: %Zu\n", ptr);
    mpz_clear(g1);
    mpz_clear(g0);
}

void mainloop(mpz_t ptr, mpz_t pk0, mpz_t p, mpz_t q, struct connections * conn){
    const struct timeval wait={1, 0};
    for(;;){
        fd_set copy=conn->pwm;
        int stop=select(conn->pwnsize, &copy, NULL, NULL, &wait);
        int i=0;
        for(; i<FD_SETSIZE; i++){
            if(!(bool)FD_ISSET(i, &copy)){
                continue;
            }
            //handle new connection

        }

    }

}

int main(){

    mpz_t p, q;
    //generatekeypairs(4096, p, q);
    setDefaultKeyPairs(p, q);
    //gmp_printf("\n\nThe selected keypairs are: q=%Zx\n\np=%Zx\n", p, q);
    mpz_t prk;
    generatepk0(prk, q);
    mpz_t symmetric_key;
    exchangekeydemo(symmetric_key, prk, p, q);
    //mpz_init(p); mpz_init(q); mpz_init(n);
    //mpz_set_ui(p, 3); mpz_set_ui(q, 7); mpz_set_ui(n, 6);
    //modexp(mod, p, q, n, 3);
    //gmp_printf("%Zu\n", mod);
    return 0;
}
