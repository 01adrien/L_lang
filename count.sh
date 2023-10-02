wc -l src/*.c src/includes/*.h
grep -v -E "(^$)" src/*.c src/includes/*.h | wc -l  