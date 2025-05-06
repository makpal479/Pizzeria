#include <iostream>
#include <vector>
#include <string>
#include <memory>


enum class OrderType { DINE_IN, TAKEAWAY, DELIVERY };
enum class OrderStatus { PENDING, PREPARING, READY, DELIVERED, CANCELLED };


class Topping {
private:
    std::string name;
    float price;
public:
    Topping(const std::string& n, float p) : name(n), price(p) {}
    std::string getName() const { return name; }
    float getPrice() const { return price; }
    void display() const {
        std::cout << "  + " << name << " ($" << price << ")\n";
    }
};

class MenuItem {
protected:
    std::string name;
    float price;
    std::vector<Topping> toppings;
public:
    MenuItem(const std::string& n, float p) : name(n), price(p) {}
    virtual void display() const {
        std::cout << name << " - $" << getPrice() << "\n";
        for (const auto& t : toppings) t.display();
    }
    virtual float getPrice() const {
        float total = price;
        for (const auto& t : toppings) total += t.getPrice();
        return total;
    }
    virtual std::string getName() const { return name; }
    void addTopping(const Topping& topping) {
        toppings.push_back(topping);
    }
    virtual std::shared_ptr<MenuItem> clone() const = 0;
    virtual ~MenuItem() {}
};

class Pizza : public MenuItem {
public:
    Pizza(const std::string& name, float price) : MenuItem(name, price) {}
    std::shared_ptr<MenuItem> clone() const override {
        auto copy = std::make_shared<Pizza>(*this);
        return copy;
    }
};

class Drink : public MenuItem {
public:
    Drink(const std::string& name, float price) : MenuItem(name, price) {}
    std::shared_ptr<MenuItem> clone() const override {
        auto copy = std::make_shared<Drink>(*this);
        return copy;
    }
};

class SideDish : public MenuItem {
public:
    SideDish(const std::string& name, float price) : MenuItem(name, price) {}
    std::shared_ptr<MenuItem> clone() const override {
        auto copy = std::make_shared<SideDish>(*this);
        return copy;
    }
};


class Menu {
private:
    std::vector<std::shared_ptr<MenuItem>> items;
public:
    Menu() {
        items.push_back(std::make_shared<Pizza>("Margherita", 5.5f));
        items.push_back(std::make_shared<Pizza>("Pepperoni", 6.5f));
        items.push_back(std::make_shared<Drink>("Coke", 1.5f));
        items.push_back(std::make_shared<SideDish>("Garlic Bread", 2.0f));
    }

    void displayMenu() const {
        std::cout << "\n--- MENU ---\n";
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << i << ". ";
            items[i]->display();
        }
    }

    std::shared_ptr<MenuItem> getItemCopy(int index) const {
        if (index >= 0 && index < static_cast<int>(items.size()))
            return items[index]->clone();
        return nullptr;
    }

    void addNewItem() {
        int type;
        std::string name;
        float price;
        std::cout << "\nAdd New Menu Item\n";
        std::cout << "Choose type (0 - Pizza, 1 - Drink, 2 - SideDish): ";
        std::cin >> type;
        std::cin.ignore();
        std::cout << "Enter name: ";
        std::getline(std::cin, name);
        std::cout << "Enter price: $";
        std::cin >> price;

        switch (type) {
        case 0:
            items.push_back(std::make_shared<Pizza>(name, price));
            break;
        case 1:
            items.push_back(std::make_shared<Drink>(name, price));
            break;
        case 2:
            items.push_back(std::make_shared<SideDish>(name, price));
            break;
        default:
            std::cout << "Invalid type.\n";
        }
    }


};


class Order {
private:
    int id;
    std::string customerName;
    OrderType type;
    std::string deliveryAddress;
    OrderStatus status;
    std::vector<std::shared_ptr<MenuItem>> items;
    float total;

public:
    Order(int id, const std::string& name, OrderType type, const std::string& address = "")
        : id(id), customerName(name), type(type), deliveryAddress(address), status(OrderStatus::PENDING), total(0.0f) {
    }

    void addItem(std::shared_ptr<MenuItem> item) {
        if (item) {
            total += item->getPrice();
            items.push_back(item);
        }
    }

    void removeItem(int index) {
        if (index >= 0 && index < static_cast<int>(items.size())) {
            total -= items[index]->getPrice();
            items.erase(items.begin() + index);
            std::cout << "Item removed from order.\n";
        }
        else {
            std::cout << "Invalid index.\n";
        }
    }

    void calculateTotal() {
        total = 0.0f;
        for (const auto& item : items)
            total += item->getPrice();
    }

    void displayOrder() const {
        std::cout << "\nOrder #" << id << " (" << customerName << ") ";
        std::cout << "[Status: " << static_cast<int>(status) << "]\n";
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << i << ". ";
            items[i]->display();
        }
        std::cout << "Total: $" << total << "\n";
    }

    int getId() const { return id; }

    void updateStatus(OrderStatus newStatus) {
        status = newStatus;
    }
};


class Pizzeria {
private:
    Menu menu;
    std::vector<std::shared_ptr<Order>> orders;
    int nextOrderId;
    std::vector<Topping> predefinedToppings = {
        Topping("Cheese", 0.5f),
        Topping("Olives", 0.3f),
        Topping("Mushrooms", 0.4f),
        Topping("Pepperoni", 0.6f),
        Topping("Pineapple", 0.5f),
        Topping("Tomatoes", 0.25f)
    };

public:
    Pizzeria() : nextOrderId(1) {}

    void takeOrder() {
        std::string name, address;
        int typeInput;

        std::cout << "\nEnter customer name: ";
        std::cin >> name;

        std::cout << "Select order type (0 - DINE_IN, 1 - TAKEAWAY, 2 - DELIVERY): ";
        std::cin >> typeInput;

        OrderType type = static_cast<OrderType>(typeInput);

        if (type == OrderType::DELIVERY) {
            std::cout << "Enter delivery address: ";
            std::cin.ignore();
            std::getline(std::cin, address);
        }

        std::shared_ptr<Order> order = std::make_shared<Order>(nextOrderId++, name, type, address);

        bool adding = true;
        while (adding) {
            menu.displayMenu();
            std::cout << "Enter item index to add to order (-2 to remove item, -1 to finish): ";
            int index;
            std::cin >> index;
            if (index == -1) break;
            if (index == -2) {
                order->displayOrder();
                std::cout << "Enter item index to remove: ";
                int removeIndex;
                std::cin >> removeIndex;
                order->removeItem(removeIndex);
                continue;
            }

            auto item = menu.getItemCopy(index);
            if (item) {
                char addTopping;
                do {
                    std::cout << "Add topping to " << item->getName() << "? (y/n): ";
                    std::cin >> addTopping;
                    if (addTopping == 'y' || addTopping == 'Y') {
                        std::cout << "Available toppings:\n";
                        for (size_t i = 0; i < predefinedToppings.size(); ++i) {
                            std::cout << i << ". " << predefinedToppings[i].getName()
                                << " ($" << predefinedToppings[i].getPrice() << ")\n";
                        }
                        int toppingIndex;
                        std::cout << "Choose topping index: ";
                        std::cin >> toppingIndex;
                        if (toppingIndex >= 0 && toppingIndex < static_cast<int>(predefinedToppings.size())) {
                            item->addTopping(predefinedToppings[toppingIndex]);
                        }
                        else {
                            std::cout << "Invalid topping index.\n";
                        }
                    }
                } while (addTopping == 'y' || addTopping == 'Y');

                order->addItem(item);
            }
            else {
                std::cout << "Invalid item.\n";
            }
        }

        order->calculateTotal();
        orders.push_back(order);

        std::cout << "\nOrder placed successfully!\n";
        order->displayOrder();
    }

    void updateOrderStatus() {
        int id;
        std::cout << "Enter order ID to update: ";
        std::cin >> id;

        for (auto& order : orders) {
            if (order->getId() == id) {
                std::cout << "Select new status (0 - PENDING, 1 - PREPARING, 2 - READY, 3 - DELIVERED, 4 - CANCELLED): ";
                int statusInput;
                std::cin >> statusInput;
                order->updateStatus(static_cast<OrderStatus>(statusInput));
                std::cout << "Order status updated.\n";
                return;
            }
        }
        std::cout << "Order not found.\n";
    }

    void cancelOrder() {
        int id;
        std::cout << "Enter order ID to cancel: ";
        std::cin >> id;

        for (auto& order : orders) {
            if (order->getId() == id) {
                order->updateStatus(OrderStatus::CANCELLED);
                std::cout << "Order #" << id << " has been cancelled.\n";
                return;
            }
        }
        std::cout << "Order not found.\n";
    }

    void showAllOrders() const {
        for (const auto& order : orders)
            order->displayOrder();
    }

    void addMenuItem() {
        menu.addNewItem();
    }
};


int main() {
    Pizzeria pizzeria;
    int choice;

    do {
        std::cout << "\n--- Pizzeria Simulator ---\n";
        std::cout << "1. Take Order\n2. Update Order Status\n3. Show All Orders\n4. Add Menu Item\n5. Cancel Order\n0. Exit\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            pizzeria.takeOrder();
            break;
        case 2:
            pizzeria.updateOrderStatus();
            break;
        case 3:
            pizzeria.showAllOrders();
            break;
        case 4:
            pizzeria.addMenuItem();
            break;
        case 5:
            pizzeria.cancelOrder();
            break;
        case 0:
            std::cout << "Exiting...\n";
            break;
        default:
            std::cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}