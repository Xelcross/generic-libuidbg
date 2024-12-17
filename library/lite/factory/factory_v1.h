#ifndef _FACTORY_H
#define _FACTORY_H

#include <string>
#include <unordered_map>

template <class Product, class ...Args>
struct Producer
{
    virtual Product* produce(Args&& ...args) = 0;
};

template <class Product, class SeriesID = std::string>
class Factory
{
public:
    template <class ...Args>
    static Product* produce(const SeriesID& seriesID, Args&& ...args)
    {
        Product* product(nullptr);
        do {
            const auto& collection = productionLineCollection();
            const auto fi = collection.find(seriesID);
            if (fi == collection.end())
                break;
            auto producer = dynamic_cast<Producer<Product, Args...>*>(fi->second);
            if (producer == nullptr)
                break;
            product = producer->produce(std::forward<Args>(args)...);
        } while (false);
        return product;
    }

    class ProductionLine
    {
    protected:
        ProductionLine(const SeriesID &seriesID) : m_seriesID(seriesID)
        {
            productionLineCollection().emplace(seriesID, this);
        }

        virtual ~ProductionLine()
        {
            productionLineCollection().erase(m_seriesID);
        }

        SeriesID m_seriesID;
    };

private:
    static std::unordered_map<SeriesID, ProductionLine*>& productionLineCollection()
    {
        static std::unordered_map<SeriesID, ProductionLine*> collection;
        return collection;
    }
};

template <class Product, class Series, class SeriesID, class ...Args>
class ProductionLine : public Factory<Product, SeriesID>::ProductionLine
    , public Producer<Product, Args...>
{
    typedef class Factory<Product, SeriesID>::ProductionLine Base;

public:
    ProductionLine(const SeriesID& seriesID) : Base(seriesID) {}

    Product* produce(Args&& ...args) override
    {
        return new Series(std::forward<Args>(args)...);
    }
};

#endif // _FACTORY_H
