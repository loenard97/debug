<div align="center">

# debug
Library for debugging memory allocations

![last commit](https://img.shields.io/github/last-commit/loenard97/debug?&style=for-the-badge&logo=github&color=3776AB)
![repo size](https://img.shields.io/github/repo-size/loenard97/debug?&style=for-the-badge&logo=github&color=3776AB)

</div>


## Example usage
Including this library overwrites `malloc` and `free` with implementations that print errors and warnings for double free and memory leak issues. `check_alloc` prints out all pointers that have not been freed yet.
```c
#define DEBUG_INFO  // optional flag to enable more output
#include "debug.h"

int main() {
    int *arr1 = (int *)malloc(5 * sizeof(int));
    int *arr2 = (int *)malloc(5 * sizeof(int));

    free(arr1);
    free(arr1);  // prints error, because 'arr1' has already been freed.

    check_alloc();  // prints warning, because 'arr2' has not been freed.

    return 0;
}
```

The above example outputs
```
[INFO] .\main.c:7 malloc 0x00110014 size 20
[INFO] .\main.c:8 malloc 0x00120014 size 20
[INFO] .\main.c:10 free 0x00110014 size 20
[ERRO] .\main.c:11 double free 0x00110014 size 20
[WARN] .\main.c:8 no free 0x00120014 size 20
```