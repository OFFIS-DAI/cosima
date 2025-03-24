#include <cmath>
#include "omnetpp/simtime.h"

inline omnetpp::SimTime from_mosaik_time(int mosaik_time) {
    return mosaik_time / 1000.0;
}

inline int to_mosaik_time(omnetpp::SimTime time) {
    return ceil(time.dbl() * 1000.0);
}
