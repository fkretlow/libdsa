#ifndef _test_utils_h
#define _test_utils_h

int compint(const void* a, const void* b)
{
    return *(int*)a < *(int*)b ? -1 : *(int*)a > *(int*)b ? 1 : 0;
}

void make_random(int* A, size_t n_members, unsigned max_value)
{
    for (size_t i = 0; i < n_members; ++i) {
        A[i] = rand() % max_value;
    }
}

#endif // _test_utils_h
