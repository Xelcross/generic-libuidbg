#ifndef PRIDATA_H
#define PRIDATA_H

#include <memory>

#define PriData \
private:    \
    struct Data;    \
    friend struct Data; \
    std::unique_ptr<struct Data> d;

#endif // PRIDATA_H
