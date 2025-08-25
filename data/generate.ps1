cat category-data |
  python3 .\data-to-json.py |
  python3 .\check-element-separation.py |
  python3 .\check-valid-buckets-for-categories.py |
  python3 .\check-item-completeness.py |
  python3 .\category-json-to-cpp.py |
  out-file -encoding utf8 ../src/constants/categories.cpp

cat author-data |
  python3 .\data-to-json.py |
  python3 .\check-element-separation.py |
  python3 .\check-valid-buckets-for-authors.py |
  python3 .\check-item-completeness.py |
  python3 .\author-json-to-cpp.py |
  out-file -encoding utf8 ../src/constants/authors.cpp
