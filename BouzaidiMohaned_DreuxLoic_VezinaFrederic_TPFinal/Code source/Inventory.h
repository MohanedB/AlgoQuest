#pragma once
#include <string>
#include <deque>

enum class ItemType { Health, Buff };

struct Item {
    ItemType type;
    int itemId;
    float itemPower;
    std::string itemDesc;
};

class CharInventory {
public:
    void add(const Item& item);
    void pop();
	Item getItemById(int itemId) const;
    bool removeItemById(int itemId);
    bool isEmpty() const;

    const std::deque<Item>& getAllItems() const;

private:
    std::deque<Item> inv;
};