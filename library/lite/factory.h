#ifndef FACTORY_H
#define FACTORY_H

#define FACTORY_SELECTION 2

#if  FACTORY_SELECTION <= 1
#include "factory/factory_v1.h"
#elif FACTORY_SELECTION == 2
#include "factory/factory_v2.h"
#endif //FACTORY_SELECTION

#define _SPLICE(X, Y) X##Y
#define SPLICE(X, Y) _SPLICE(X, Y)
#define SPLICE_LINE(X) SPLICE(X, __LINE__)

/**
 * @brief define productionline in cpp files.
 */
#define DEFINE_PRODUCTIONLINE(Product, Series, SeriesID, seriesID, ...) \
ProductionLine<Product, Series, SeriesID, ##__VA_ARGS__> \
    SPLICE(SPLICE_LINE(g_##Product##_ProductionLine_##Series##_), _(seriesID));

#define DEFINE_PRODUCTIONLINE_DEFAULT(Product, Series, ...) \
DEFINE_PRODUCTIONLINE(Product, Series, std::string, #Series, ##__VA_ARGS__)

#endif // FACTORY_H
