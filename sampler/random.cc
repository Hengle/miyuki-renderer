//
// Created by Shiina Miyuki on 2019/1/17.
//

#include "random.h"
using namespace Miyuki;
Float Miyuki::RandomSampler::nextFloat() {
    return (Float)erand48(seed.getPtr());
}

int Miyuki::RandomSampler::nextInt() {
    return nrand48(seed.getPtr());
}

Float Miyuki::RandomSampler::nextFloat(Seed &Xi) {
    return (Float)erand48(Xi.getPtr());
}

int Miyuki::RandomSampler::nextInt(Seed &Xi) {
    return nrand48(Xi.getPtr());
}
