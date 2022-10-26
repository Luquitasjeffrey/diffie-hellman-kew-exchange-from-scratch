# diffie-hellman-key-exchange-from-scratch
The protocol that runs all the internet for mantaining privacy in the communication (only for demonstrative purpouses)

given two numbers p and q, been very latge prime numbers, arround 4096 bits long for q, and 2048 for p, and p is a primitive root of q, you can create
a crypto-system that allows you to exchange your private keys in a way that they are not showed and both of you end up with a simetric and shared private
key.

pk0 will be your private key, and pk1 will be the other person private key.

first you compute g0
g0=p^pk0 mod q

at the same time the other person computes g1
g1=p^pk1 mod q

then both of you exchange g0, and g1, and anyone can see both numbers.

then you compute sk0
sk0=g1^pk0 mod q

and the other person computes sk1
sk1=g0^pk1

since g0=p^pk0 mod q, and (a^b mod c)^d mod c=a^(b*d) mod c, and b*c=c*b, then sk0 and sk1 are the same.

shared key=p^(pk1*pk2) mod q.

note that in no moment you send your private key-
