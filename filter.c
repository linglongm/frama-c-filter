#include <stdio.h>

/*@
  logic integer CountNZ{L}(integer n, int * a) =
  n<=0 ? 0 : CountNZ{L}(n-1, a) + (a[n-1]!=0 ? 1 : 0);

  // lemma CountInc1{L}:
  // \forall int * a, integer n;
  // 0<=n ==> a[n]!=0 ==> CountNZ{L}(n+1, a) == 1 + CountNZ{L}(n, a);

  // lemma CountInc2{L}:
  // \forall int * a, integer i;
  // 0<=i ==> a[i]==0 ==> CountNZ{L}(i+1, a) == CountNZ{L}(i, a);
 */

// CountNZ is always evaluated in the Pre state. Memory at Pre never changes,
// so writes to out[] / *nout cannot affect it and no (inductive) frame lemma
// is needed.

/*@
  requires n>=0;
  requires \valid_read(in+(0..n-1));
  requires \valid(nout);
  requires \valid(out+(0..n-1));
  requires \separated(in+(0..n-1), out+(0..n-1), nout);
  ensures *nout == CountNZ{Pre}(n, in);
  ensures \forall integer i; 0<=i<n ==>
    \old(in[i]) != 0 ==> \old(in[i]) == out[CountNZ{Pre}(i, in)];
  assigns *nout, out[0 .. n-1];
 */
void filter(int n, int * in, int *nout, int * out) {
  int count = 0;
  /*@
    loop invariant 0<=i<=n;
    loop invariant 0<=count<=i;
    loop invariant \forall integer k; 0<=k<n ==> in[k] == \at(in[k], Pre);
    loop invariant count == CountNZ{Pre}(i, in);
    loop invariant \forall integer k; 0<=k<i ==>
      in[k] != 0 ==> 0 <= CountNZ{Pre}(k, in) < count;
    loop invariant \forall integer k; 0<=k<i ==>
      in[k] != 0 ==> out[CountNZ{Pre}(k, in)] == in[k];
    loop assigns i, count, out[0 .. n-1];
    loop variant n-i;
   */
  //  Before ;
  for (int i=0; i<n; i++) {
    if (in[i] != 0) {
      // assert A1: CountNZ{Pre}(i+1, in) == 1 + CountNZ{Pre}(i, in);
      out[count] = in[i];
      count++;
      // assert A1a: count == CountNZ{Pre}(i+1, in);
    } else {
      // assert A2: CountNZ{Pre}(i+1, in) == CountNZ{Pre}(i, in);
      // assert A2a: count == CountNZ{Pre}(i+1, in);
    }
    // in{beofre} ==
  }
  // assert END1: CountNZ{Pre}(n, in) == count;
  *nout = count;
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