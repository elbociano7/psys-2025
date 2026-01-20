#ifndef BANK_OPERATIONS
#define BANK_OPERATIONS

#include <unistd.h>

int add_amount(int *balance, int amount)
{
    *balance += amount;
    return *balance;
}

int slow_add_amount(int *balance, int amount, int u_time)
{
    int tmp = *balance;
    tmp += amount;
    usleep(u_time);
    *balance = tmp;
    return tmp;
}

void transfer(int *balance_from, int *balance_to, int amount)
{
    *balance_from -= amount;
    *balance_to += amount;
}

void slow_transfer(int *balance_from, int *balance_to, int amount, int u_time)
{
    int tmp_from = *balance_from;
    int tmp_to = *balance_to;
    tmp_from -= amount;
    tmp_to += amount;
    usleep(u_time);
    *balance_from = tmp_from;
    *balance_to = tmp_to;
}

#endif