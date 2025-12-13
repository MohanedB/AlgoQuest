#include "Inventory.h"


void CharInventory::add(const Item& item) {
    inv.push_back(item);
}


void CharInventory::pop() {
    if (!inv.empty()) inv.pop_front();
}

Item CharInventory::getItemById(int itemId) const {
    for (const auto& item : inv) {
        if (item.itemId == itemId) {
            return item;
        }
    }

    Item voidItem;
	voidItem.itemId = -1;
    voidItem.itemDesc = "Invalid";
	voidItem.itemPower = 0;
    return voidItem;
}


bool CharInventory::removeItemById(int itemId) {
	for (auto it = inv.begin(); it != inv.end(); ++it) {
		if (it->itemId == itemId) {
			inv.erase(it);
			return true;
		}
	}
	return false;
}

bool CharInventory::isEmpty() const {
    return inv.empty();
}


const std::deque<Item>& CharInventory::getAllItems() const {
    return inv;
}