#include <stdio.h>

/*@
  logic integer CountNZ(integer n, int * a) =
  n<=0 ? 0 : CountNZ(n-1, a) + (a[n-1]!=0 ? 1 : 0);

  lemma CountInc1:
  \forall int * a, integer n;
  0<=n ==> a[n]!=0 ==> CountNZ(n+1, a) == 1 + CountNZ(n, a);

  lemma CountInc2:
  \forall int * a, integer i;
  0<=i ==> a[i]==0 ==> CountNZ(i+1, a) == CountNZ(i, a);

  lemma CountUn{L1,L2}:
  \forall int * a, integer n;
  (\at(n,L1)==\at(n,L2) && \forall integer i; 0<=i<\at(n,L2) ==>
   \at(a[i],L1) == \at(a[i],L2))
  ==>
  \at(CountNZ(n, a), L1) == \at(CountNZ(n, a), L2);

  lemma CountBound:
  \forall integer n, int * a; CountNZ(n, a) <= n;
 */

/*@
  requires n>=0;
  requires \valid_read(in+(0..n-1));
  requires \valid(nout);
  requires \valid(out+(0..n-1));
  requires \separated(in+(0..n-1), out+(0..n-1), nout);
  ensures *nout == CountNZ(n, in);
  ensures \forall integer i; 0<=i<n ==>
    in[i] != 0 ==> in[i] == out[CountNZ(i+1, in)];
  assigns *nout, out[0 .. n-1];
 */
void filter(int n, int * in, int *nout, int * out) {
  int count = 0;
  /*@
    loop invariant 0<=i<=n;
    loop invariant 0<=count<=i;
    loop invariant count == CountNZ(i, in);
    loop assigns i, count, out[0 .. n-1];
    loop variant n-i;
   */
  for (int i=0; i<n; i++) {
    if (in[i] != 0) {
      // count == CountNZ(i, in)
      //@ assert A1: CountNZ(i+1, in) == 1 + CountNZ(i, in);
      out[count] = in[i];
      count++;
      //@ assert A1a: count == CountNZ(i+1, in);
    } else {
      //@ assert A2: CountNZ(i+1, in) == CountNZ(i, in);
      //@ assert A2a: count == CountNZ(i+1, in);
    }

  }
  //@ assert END1: CountNZ(n, in) == count;
  //@ assert SEP: \separated(nout, &n, in+(0..n-1), &count, &in);
 L1:
  *nout = count;

  //@ assert UN1: count == \at(count, L1);
  //@ assert UN2: n == \at(n, L1);
  //@ assert UN3: \forall integer i; 0<=i<n ==> in[i] == \at(in[i], L1);
  //@ assert UN4: CountNZ(n, in) == \at(CountNZ(n, in), L1);

  //@ assert END2: CountNZ(n, in) == count;
  //@ assert END3: *nout == count;
  //@ assert END4: *nout == CountNZ(n, in);
}

int main(void) {
  int n = 10;
  int a[n];
  for (int i=0; i<n; i++)
    a[i] = i%2==0 ? i : 0;
  int b[n];
  int nout;
  filter(n, a, &nout, b);
  for (int i=0; i<nout; i++)
    printf("%d ", b[i]);
  printf("\n");
}