#!/bin/sh
set -u
printf '%-20s %s\n' "TEST" "RESULT"
for f in tests/*.txt; do
  case "$f" in
    *tc14_bad.txt) ./vco "$f" >/tmp/vco_test.out 2>&1; rc=$?; if grep -q "Lexical error" /tmp/vco_test.out; then r="EXPECTED LEXICAL ERROR"; else r="FAIL"; fi ;;
    *tc09.txt) ./vco "$f" >/tmp/vco_test.out 2>&1; if grep -q "NOT CHECKED" /tmp/vco_test.out && grep -q "division by zero not folded" /tmp/vco_test.out; then r="EXPECTED RUNTIME GUARD"; else r="FAIL"; fi ;;
    *) ./vco "$f" >/tmp/vco_test.out 2>&1; if grep -q "Semantic equivalence: PASS" /tmp/vco_test.out; then r="PASS"; else r="FAIL"; fi ;;
  esac
  printf '%-20s %s\n' "$f" "$r"
done
