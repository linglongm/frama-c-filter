#include <stdio.h>

/*@
  // Number of non-zero elements in a[0..n-1], read in memory state L.
  logic integer CountNZ{L}(integer n, int * a) =
    n<=0 ? 0 : CountNZ{L}(n-1, a) + (a[n-1]!=0 ? 1 : 0);

  // Frame lemma: if a[0..n-1] is the same in L1 and L2, so is CountNZ.
  // Needs induction on n: prove it interactively (Rocq).
  lemma CountUn{L1,L2}:
    \forall int * a, integer n;
      (\forall integer i; 0<=i<n ==> \at(a[i],L1) == \at(a[i],L2)) ==>
      CountNZ{L1}(n, a) == CountNZ{L2}(n, a);
 */

/*@
  requires n>=0;
  requires \valid_read(in+(0..n-1));
  requires \valid(nout);
  requires \valid(out+(0..n-1));
  requires \separated(in+(0..n-1), out+(0..n-1), nout);
  ensures *nout == CountNZ(n, in);
  ensures \forall integer i; 0<=i<n ==>
    in[i] != 0 ==> in[i] == out[CountNZ(i, in)];
  assigns *nout, out[0 .. n-1];
 */
void filter(int n, int * in, int *nout, int * out) {
  int count = 0;
  /*@
    loop invariant 0<=i<=n;
    loop invariant 0<=count<=i;
    loop invariant count == CountNZ(i, in);
    loop invariant \forall integer k; 0<=k<i ==>
      in[k] != 0 ==> 0 <= CountNZ(k, in) < count;
    loop invariant \forall integer k; 0<=k<i ==>
      in[k] != 0 ==> out[CountNZ(k, in)] == in[k];
    loop assigns i, count, out[0 .. n-1];
    loop variant n-i;
   */
  for (int i=0; i<n; i++) {
    if (in[i] != 0) {
      out[count] = in[i];
      count++;
    }
  }
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
