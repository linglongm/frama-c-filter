/*@ 
  axiomatic Sum {
    logic integer sum(integer n);
    axiom sum_zero: sum(0) == 0;
    axiom sum_succ: \forall integer n; n > 0 ==> sum(n) == sum(n-1) + n;
  }

  lemma sum_closed: \forall integer n; n >= 0 ==> 2 * sum(n) == n * (n + 1);
*/