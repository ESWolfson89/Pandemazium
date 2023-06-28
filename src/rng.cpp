// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#include "rng.h"

bool roll(int sides)
{
    return randInt(0,sides-1) == 0;
}

bool rollPerc(int perc)
{
    return randInt(0,99) < perc;
}

int randInt(int low, int high)
{
    if (high <= low)
        return high;

    std::uniform_int_distribution<int> dist(low,high);
    return dist(random_number_generator);
}

int randZero(int num)
{
    if (num <= 0)
        return 0;

    return randInt(0,num);
}
