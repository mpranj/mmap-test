# mmap-test

Compile and run with `gcc -Wall mmap.c -o mmap && valgrind ./mmap`

Output on my machine:
```
========== start
========== pagesize: 4096
========== writing
mmap addr: 0x4027000
========== initial values
TestData {
a: 33, b: 69
}
mmap addr: 0x4028000
========== local change
TestData {
a: 1, b: 2
}
========== read second mapping
TestData {
a: 1, b: 2
}
mmap addr: 0x4029000
========== local change
TestData {
a: 3, b: 4
}
========== read third mapping
TestData {
a: 3, b: 4
}
========== re-read original mapping
TestData {
a: 0, b: 0
}
========== re-read second mapping
TestData {
a: 0, b: 0
}
========== re-read third mapping
TestData {
a: 3, b: 4
}
========== end
```

The data from original and second mapping is destroyed if the underlying file was truncated.
