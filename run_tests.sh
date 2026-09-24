#!/bin/sh
set -u

printf '%-25s %s\n' "TEST" "RESULT"
printf '%-25s %s\n' "-------------------------" "-------------------------"

for f in tests/*.txt; do
  case "$f" in

    *tc14_bad.txt|*tc23_bad.txt)
      ./vco "$f" >/tmp/vco_test.out 2>&1
      if grep -q "Lexical error" /tmp/vco_test.out; then
        r="EXPECTED LEXICAL ERROR"
      else
        r="FAIL"
      fi
      ;;

    *tc24_bad.txt)
      ./vco "$f" >/tmp/vco_test.out 2>&1
      if grep -q "Parsing failed" /tmp/vco_test.out && \
         grep -q "Parse error" /tmp/vco_test.out; then
        r="EXPECTED SYNTAX ERROR"
      else
        r="FAIL"
      fi
      ;;

    *tc09.txt|*tc26.txt)
      ./vco "$f" >/tmp/vco_test.out 2>&1
      if grep -q "NOT CHECKED" /tmp/vco_test.out && \
         grep -q "division by zero not folded" /tmp/vco_test.out; then
        r="EXPECTED RUNTIME GUARD"
      else
        r="FAIL"
      fi
      ;;

    *)
      ./vco "$f" >/tmp/vco_test.out 2>&1
      if grep -q "Semantic equivalence: PASS" /tmp/vco_test.out; then
        r="PASS"
      else
        r="FAIL"
      fi
      ;;
  esac

  printf '%-25s %s\n' "$f" "$r"
done
