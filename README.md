# Making Why3 1.8.2 Recognize Rocq 9.1.0 (Frama-C WP + Rocq)

## Background

Frama-C WP calls Rocq (formerly Coq) through Why3. During `why3 config detect`, Why3 runs each prover's version command and extracts the version number with a regex. A prover is written to `~/.why3.conf`, and becomes usable by WP, only if two things are true: the version is parsed successfully, and it matches the Rocq version that `why3-coq` was compiled against.

## Symptom

```
$ frama-c -wp ... -wp-prover coq ...
[wp] User Error: Unknown prover coq

$ why3 config detect
Found prover Coq version , but the compiled Why3 library supports only version 9.1.0
```

Note that the version string is **empty**. Why3 found `coqtop` but could not read its version.

## Root Cause

The detection rule in Why3 1.8.2 was written for Coq 8.x. It lives in the `[ITP coq]` section of `$(why3 --print-datadir)/provers-detection-data.conf`:

```ini
exec = "coqtop"
version_switch = "-v"
version_regexp = "The Coq Proof Assistant, version \\([^ \n]+\\)"
version_ok = "^8\.20\.[0-9]+$"
...
```

After the rename, `coqtop -v` in Rocq 9 prints:

```
The Rocq Prover, version 9.1.0
```

This causes two problems:

1. `version_regexp` only matches `The Coq Proof Assistant`, so the match fails and the version comes out empty.
2. `version_ok` only lists 8.x versions, so 9.x is not accepted.

## Fix

### Prerequisites (all in the same opam switch)

```sh
# coq-core: provides coqc/coqtop compat binaries for Rocq 9
# coq-stdlib: Rocq standard library
# why3-coq: Why3's Coq realization libraries, compiled for the installed Rocq
opam install coq-core coq-stdlib why3-coq
```

If your OS offers to install a distro package such as `coq-core-compat`, decline. Its Rocq version may not match the one in opam.

### Patch the detection rule

Change the regex so it matches both the old and new output formats, and add a `version_ok` entry for 9.x:

```sh
DATA="$(why3 --print-datadir)/provers-detection-data.conf"   # fish: set DATA (why3 --print-datadir)/provers-detection-data.conf
cp "$DATA" "$DATA.bak"

python3 - "$DATA" <<'EOF'
import sys
path = sys.argv[1]
text = open(path).read()

# Old Coq-only regexp
old = r'version_regexp = "The Coq Proof Assistant, version \\([^ \n]+\\)"'
# New regexp: matches both "The Coq Proof Assistant" and "The Rocq Prover";
# also accept Rocq 9.x versions
new = (r'version_regexp = "The [A-Za-z ]*, version \\([^ \n]+\\)"' + "\n"
       + r'version_ok = "^9\.[0-9]+\.[0-9]+$"')

n = text.count(old)
if n == 0:
    sys.exit("Pattern not found; file unchanged.")
open(path, "w").write(text.replace(old, new))
print(f"Patched {n} occurrence(s).")
EOF
```

The new regex deliberately avoids `\\|` alternation. That keeps a single capture group, since Why3 takes the first group as the version.

### Re-detect and verify

```sh
why3 config detect 2>&1 | grep -i coq
# Found prover Coq version 9.1.0, OK.

frama-c -wp-list-provers
# [wp] Prover Coq        9.1.0  [Coq:9.1.0] (coq)
```

`-wp-prover coq` now works.

## Editing Proof Scripts

WP generates scripts at `<session>/interactive/<goal>.v`. You can edit them directly and check them in batch mode:

```sh
frama-c -wp -wp-prop sum_closed -wp-prover coq -wp-session mysession rocq-test.c
```

VsRocq needs to know where Why3's libraries are. Without that, the editor reports `Unable to locate library BuiltIn`. Fix it by creating a `_CoqProject` in the workspace root:

```sh
echo "-R $(why3 --print-libdir)/coq Why3" > _CoqProject   # fish: echo "-R "(why3 --print-libdir)"/coq Why3" > _CoqProject
```

## Notes

- `opam reinstall why3`, or any Why3 upgrade, overwrites `provers-detection-data.conf`, so you will need to re-apply the patch (a backup is kept as `.bak`). Once upstream Why3 supports the Rocq 9 output format, the patch is no longer needed.
- `-wp-interactive edit` launches `coqide` by default, but Rocq 9 renamed it to `rocqide`. Either run `opam install rocqide` or change the Coq `editor` entry in `~/.why3.conf`. Keep in mind that `why3 config detect` rewrites `~/.why3.conf`.
- Why3 1.8.2 also reports Z3 4.15 and CVC5 1.3.3 as "not recognized", but WP can still use them.