// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#ifndef RNG_H_
#define RNG_H_

#include <random>
#include <time.h>

static std::mt19937 random_number_generator(time(0));

bool roll(int);
bool rollPerc(int);
int randInt(int,int);
int randZero(int);

#endif
