cat category-data | python3 .\data-to-json.py | python3 .\category-json-to-cpp.py | out-file -encoding utf8 ../src/constants/categories.cpp
cat author-data | python3 .\data-to-json.py | python3 .\author-json-to-cpp.py | out-file -encoding utf8 ../src/constants/authors.cpp
