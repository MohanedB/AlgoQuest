#pragma once
#include <string>
#include <deque>

enum class ItemType { Health, Damage, Buff };

struct Item {
    ItemType type;
    int itemId;
    std::string itemDesc;
};

class CharInventory {
public:
    void add(const Item& item);
    void pop();
    void removeItemById(int itemId);
    bool isEmpty() const;

    const std::deque<Item>& getAllItems() const;

private:
    std::deque<Item> inv;
};