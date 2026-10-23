# Frama-C WP + Rocq 9

## 1. Make Why3 detect Rocq 9

Why3 1.8.2 only recognizes the Coq 8.x version string. Patch its detection rule:

```sh
DATA="$(why3 --print-datadir)/provers-detection-data.conf"   # fish: set DATA (why3 --print-datadir)/provers-detection-data.conf
cp "$DATA" "$DATA.bak"

python3 - "$DATA" <<'EOF'
import sys
path = sys.argv[1]
text = open(path).read()
old = r'version_regexp = "The Coq Proof Assistant, version \\([^ \n]+\\)"'
new = (r'version_regexp = "The [A-Za-z ]*, version \\([^ \n]+\\)"' + "\n"
       + r'version_ok = "^9\.[0-9]+\.[0-9]+$"')
if old not in text:
    sys.exit("Pattern not found; file unchanged.")
open(path, "w").write(text.replace(old, new))
print("Patched.")
EOF

why3 config detect 2>&1 | grep -i coq   # Found prover Coq version 9.1.0, OK.
```

Reapply this after reinstalling or upgrading Why3.

## 2. Write a proof

```sh
frama-c -wp -wp-prop CountUn -wp-prover coq -wp-interactive=edit -wp-session lemma-rocq filter.c
```

This generates `lemma-rocq/interactive/lemma_CountUn.v` and opens it in the IDE. Write the proof between `Proof.` and `Qed.`, then save. Do not edit anything else in the file.

## 3. Check the proof

```sh
frama-c -wp -wp-prop CountUn -wp-prover coq -wp-session lemma-rocq filter.c
```

Or use `make lemmas`.
