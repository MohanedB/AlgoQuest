#include "Inventory.h"


void CharInventory::add(const Item& item) {
    inv.push_back(item);
}


void CharInventory::pop() {
    if (!inv.empty()) inv.pop_front();
}

void CharInventory::removeItemById(int itemId) {
	for (auto it = inv.begin(); it != inv.end(); ++it) {
		if (it->itemId == itemId) {
			inv.erase(it);
			break;
		}
	}
}

bool CharInventory::isEmpty() const {
    return inv.empty();
}


const std::deque<Item>& CharInventory::getAllItems() const {
    return inv;
}