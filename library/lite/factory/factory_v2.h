#ifndef _FACTORY_H
#define _FACTORY_H

#include <string>
#include <unordered_map>

template <class Product, class SeriesID = std::string>
class Factory
{
public:
    template <class ...Args>
    static Product* produce(const SeriesID& seriesID, Args&& ...args)
    {
        const auto fi = productionLineCollection<Args...>().find(seriesID);
        return fi != productionLineCollection<Args...>().end() ?
                   fi->second->produce(std::forward<Args>(args)...) : nullptr;
    }

    template <class ...Args>
    class ProductionLine
    {
    public:
        virtual Product* produce(Args&& ...args) = 0;

    protected:
        ProductionLine(const SeriesID& seriesID)
            : m_seriesID(seriesID)
        {
            productionLineCollection<Args...>().emplace(seriesID, this);
        }

        virtual ~ProductionLine()
        {
            productionLineCollection<Args...>().erase(m_seriesID);
        }

    private:
        SeriesID m_seriesID;
    };

private:
    template <class ...Args>
    static std::unordered_map<SeriesID, ProductionLine<Args...>*>& productionLineCollection()
    {
        static std::unordered_map<SeriesID, ProductionLine<Args...>*> collection;
        return collection;
    }
};

template <class Product, class Series, class SeriesID, class ...Args>
class ProductionLine : public Factory<Product, SeriesID>::template ProductionLine<Args...>
{
    typedef typename Factory<Product, SeriesID>::template ProductionLine<Args...> Base;

public:
    ProductionLine(const SeriesID& seriesID) : Base(seriesID) {}

    Product* produce(Args&& ...args) override
    {
        return new Series(std::forward<Args>(args)...);
    }
};

#endif // _FACTORY_H
