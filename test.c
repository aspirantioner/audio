#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define SWAP(a, b)           \
    {                        \
        typeof(*a) tmp = *b; \
        *b = *a;             \
        *a = tmp;            \
    }

int f(int x, int n)
{
    if (x % 2 == 0)
    {
        return x / 2 + n / 2;
    }
    return (x + 1) / 2;
}

int sum = 302;
void fenzhi(int *array, int left, int leftnum, int right, int rightnum)
{
    if (leftnum > 2)
    {
        int len = left % 2 == 0 ? leftnum - leftnum / 2 : leftnum / 2;
        fenzhi(array, left, len, left + len, leftnum - len);
        // for (int i = 0; i < leftnum; i++)
        // {
        //     printf("%d\t", array[left + i]);
        // }
        // printf("\n");
    }
    if (rightnum > 2)
    {
        int len = right % 2 == 0 ? rightnum - rightnum / 2 : rightnum / 2;
        fenzhi(array, right, len, right + len, rightnum - len);
        // for (int i = 0; i < rightnum; i++)
        // {
        //     printf("%d\t", array[right + i]);
        // }
        // printf("\n");
    }
    if (leftnum == 2)
    {
        if (left % 2 == 1)
        {
            int tmp = array[left + 1];
            array[left + 1] = array[left];
            array[left] = tmp;
        }
    }
    if (rightnum == 2)
    {
        if (right % 2 == 1)
        {
            int tmp = array[right + 1];
            array[right + 1] = array[right];
            array[right] = tmp;
        }
    }
    // printf("fenzhi qian!\n");
    // for (int i = 0; i < leftnum; i++)
    // {
    //     printf("%d\t", array[left + i]);
    // }
    // printf("\n");
    // for (int i = 0; i < rightnum; i++)
    // {
    //     printf("%d\t", array[right + i]);
    // }
    // printf("\n");
    int sort_num = leftnum > rightnum ? leftnum / 2 : rightnum / 2;
    if (leftnum == rightnum && left % 2 == 1 && leftnum % 2 == 1)
    {
        sort_num++;
    }
    for (int i = 0; i < sort_num; i++)
    {
        int tmp = array[right + i];
        array[right + i] = array[leftnum - sort_num + i + left];
        array[leftnum - sort_num + i + left] = tmp;
    }
    // for (int i = 0; i < leftnum + rightnum; i++)
    // {
    //     printf("%d\t", array[left + i]);
    // }
    // printf("\n");
}

int main()
{
    float a = 0.3;
    float b = 1.5;
    SWAP(&a, &b);
    printf("%f\t%f\n", a, b);
    float array[4] = {1.0, 2.0, 3.0, 4.0};
    SWAP((float *)array + 0, (float *)array + 1);
    for (int i = 0; i < 4; i++)
    {
        printf("%f\n", array[i]);
    }
    // int array[sum];
    // for (int i = 0; i < sum; i++)
    // {
    //     array[i] = i + 1;
    // }
    // fenzhi(array, 0, sum - sum / 2, sum - sum / 2, sum / 2);
    // bool arr[sum / 2 + 1];
    // for (int i = 0; i < sum / 2 + 1; i++)
    // {
    //     arr[i] = false;
    // }
    // for (int i = 2; i <= sum / 2; i += 2)
    // {
    //     if (arr[i / 2])
    //     {
    //         continue;
    //     }
    //     else
    //     {
    //         arr[i / 2] = true;
    //     }
    //     int res = i;
    //     int head = i;
    //     int end = i;
    //     printf("%d\t", i);
    //     do
    //     {
    //         res = f(res, sum);
    //         if (res <= sum / 2 && res % 2 == 0)
    //         {
    //             arr[res / 2] = true;
    //         }
    //         printf("%d\t", res);
    //         end = head;
    //         head = array[res];
    //         array[res] = end;
    //     } while (res != i);
    //     printf("\n");
    // }
    // for (int i = 0; i < sum; i++)
    // {
    //     printf("%d\t", array[i]);
    // }
    // printf("\n");
    return 0;
}
