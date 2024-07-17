#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <cassert>

using namespace std;
//class product
class Product {
protected:
    string name;
    double price; 
    int stock;
    string description;

public:
    
     
    Product(const string& n, double p, int s, const string& d) 
        : name(n), price(p), stock(s), description(d) {}
    virtual ~Product() = default;

    virtual double getPrice() const { return price; }
    virtual string getName() const { return name; }
    virtual int getStock() const { return stock; }
    virtual void updateStock(int quantity) { 
        if (quantity > stock) {
            throw runtime_error("Not enough stock");
        }
        stock -= quantity; 
    }

    virtual void display() const {
        cout << "Product: " << name << ", Price: KES " << fixed << setprecision(2) << price << ", Stock: " << stock << endl;
    }

    virtual void displayDetails() const {
        display();
        cout << "Description: " << description << endl;
    }
};

//class FreshProduct
 
class FreshProduct : public Product {
private:
    int shelfLife; 

public:
    FreshProduct(const string& n, double p, int s, const string& d, int life)
        : Product(n, p, s, d), shelfLife(life) {}

    int getShelfLife() const { return shelfLife; }

    void display() const override {
        Product::display();
        cout << "Shelf Life: " << shelfLife << " days" << endl;
    }

    void displayDetails() const override {
        Product::displayDetails();
        cout << "Shelf Life: " << shelfLife << " days" << endl;
    }
};


//class ShoppingCart
class ShoppingCart {
private:
    struct CartItem {
        shared_ptr<Product> product;
        int quantity;
    };

    vector<CartItem> items;

public:
    void addToCart(shared_ptr<Product> product, int quantity) {
        if (quantity <= 0) {
            throw invalid_argument("Quantity must be positive");
        }
        if (product->getStock() < quantity) {
            throw runtime_error("Not enough stock");
        }
        items.push_back({product, quantity});
        product->updateStock(quantity);
    }

    double getTotalCost() const {
        double total = 0.0;
        for (const auto& item : items) {
            total += item.product->getPrice() * item.quantity;
        }
        return total;
    }

    void display() const {
        cout << "Shopping Cart:" << endl;
        for (const auto& item : items) {
            cout << item.product->getName() << " x " << item.quantity
                 << " = KES " << fixed << setprecision(2) << (item.product->getPrice() * item.quantity) << endl;
        }
        cout << "Total: KES " << fixed << setprecision(2) << getTotalCost() << endl;
    }
};

//class DiscountStrategy
 
class DiscountStrategy {
public:
    virtual double applyDiscount(double amount) = 0;
    virtual ~DiscountStrategy() = default;
};

//class percentage discount
class PercentageDiscount : public DiscountStrategy {
private:
    double percentage;

public:
    PercentageDiscount(double p) : percentage(p) {}
    double applyDiscount(double amount) override {
        return amount * (1 - percentage / 100);
    }
};

//class FixedAmountDiscount
class FixedAmountDiscount : public DiscountStrategy {
private:
    double amount;

public:
    FixedAmountDiscount(double a) : amount(a) {}
    double applyDiscount(double originalAmount) override {
        return max(0.0, originalAmount - amount);
    }
};

//class Billing
 
class Billing {
public:
    static void generateBill(const ShoppingCart& cart) {
        cout << "Generating bill..." << endl;
        cart.display();
    }

    static double applyDiscount(double amount, DiscountStrategy* strategy) {
        return strategy->applyDiscount(amount);
    }
};

//class RecommendationSystem
 
class RecommendationSystem {
private:
    unordered_map<string, vector<string>> purchaseHistory;

public:
    void recordPurchase(const string& user, const string& product) {
        purchaseHistory[user].push_back(product);
    }

    vector<string> getRecommendations(const string& user) {
        vector<string> recommendations;
        auto it = purchaseHistory.find(user);
        if (it != purchaseHistory.end()) {
            unordered_map<string, int> productFrequency;
            for (const auto& product : it->second) {
                for (const auto& otherUser : purchaseHistory) {
                    if (otherUser.first != user && find(otherUser.second.begin(), otherUser.second.end(), product) != otherUser.second.end()) {
                        for (const auto& otherProduct : otherUser.second) {
                            if (otherProduct != product) {
                                productFrequency[otherProduct]++;
                            }
                        }
                    }
                }
            }
            vector<pair<string, int>> sortedProducts(productFrequency.begin(), productFrequency.end());
            sort(sortedProducts.begin(), sortedProducts.end(), 
                 [](const pair<string, int>& a, const pair<string, int>& b) { return a.second > b.second; });
            for (int i = 0; i < min(5, (int)sortedProducts.size()); i++) {
                recommendations.push_back(sortedProducts[i].first);
            }
        }
        return recommendations;
    }
};

// Function overloading for different payment methods
void processPayment(const string& accountNumber, double amount) {
    cout << "Processing bank payment of KES " << fixed << setprecision(2) << amount 
         << " from account " << accountNumber << endl;
}

void processMpesaPayment(const string& phoneNumber, const string& pin, double amount) {
    cout << "Processing M-PESA payment of KES " << fixed << setprecision(2) << amount 
         << " from phone number " << phoneNumber << endl;
    
    cout << "PIN verified. Payment successful." << endl;
}

//tests cases
void runTests() {
    // Product class tests
    shared_ptr<Product> apple = make_shared<Product>("Apple", 50.00, 100, "Fresh apple");
    assert(apple->getName() == "Apple");
    assert(apple->getPrice() == 50.00);
    assert(apple->getStock() == 100);

    // FreshProduct class tests
    shared_ptr<FreshProduct> milk = make_shared<FreshProduct>("Milk", 120.00, 30, "Fresh milk", 7);
    assert(milk->getShelfLife() == 7);

    // ShoppingCart class tests
    ShoppingCart cart;
    cart.addToCart(apple, 2);
    assert(cart.getTotalCost() == 100.00);

    // Discount strategy tests
    PercentageDiscount tenPercentOff(10);
    assert(Billing::applyDiscount(100.00, &tenPercentOff) == 90.00);

    FixedAmountDiscount twentyShillingsOff(20);
    assert(Billing::applyDiscount(100.00, &twentyShillingsOff) == 80.00);

    cout << "All tests passed!" << endl;
}

int main() {
    runTests();

    vector<shared_ptr<Product>> inventory;
    inventory.push_back(make_shared<Product>("Apple", 50.00, 100, "Fresh"));
    inventory.push_back(make_shared<Product>("Bread",65.00, 50, "Broadways"));
    inventory.push_back(make_shared<FreshProduct>("Milk", 120.00, 30, "Pasteurized whole milk from Brookside dairy", 7));
    inventory.push_back(make_shared<Product>("Maize Flour", 200.00, 100, "tupike "));
    inventory.push_back(make_shared<Product>("Basmati Rice", 180.00, 80, "Premium long-grain basmati rice from mwea millers"));
    inventory.push_back(make_shared<Product>("Cooking Oil", 170.00, 50, "1 liter of pure vegetable cooking oil"));
    inventory.push_back(make_shared<Product>("Ringos", 10.00, 200, "Crunchy potato chips"));
    inventory.push_back(make_shared<FreshProduct>("Yoghurt", 110.00, 40, "Creamy vanilla, probiotic-rich yoghurt", 14));
    inventory.push_back(make_shared<Product>("Ketepa Coffee", 36.00, 100, "Rich Kenyan coffee blend"));
    inventory.push_back(make_shared<FreshProduct>("Nyanya (Tomatoes)", 10.00, 150, "Fresh, ripe tomatoes", 5));
    inventory.push_back(make_shared<FreshProduct>("Machungwa (Oranges)", 20.00, 120, "from uasingishu", 7));

    ShoppingCart cart;
    Billing billing;
    RecommendationSystem recommendationSystem;
    string currentUser = "User1"; // In a real system, this would be set after user login

    while (true) {
        cout << "\n1. View Product List\n2. View Product Details\n3. Add to Cart\n4. View Cart\n5. Checkout\n6. Get Recommendations\n7. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Product List:" << endl;
                for (size_t i = 0; i < inventory.size(); ++i) {
                    cout << "Index " << i << ": ";
                    inventory[i]->display();
                }
                break;
            case 2:
                {
                    int productIndex;
                    cout << "Enter product index to view details: ";
                    cin >> productIndex;
                    if (productIndex >= 0 && productIndex < static_cast<int>(inventory.size())) {
                        inventory[productIndex]->displayDetails();
                    } else {
                        cout << "Invalid product index. Please try again." << endl;
                    }
                }
                break;
            case 3:
                {
                    int productIndex, quantity;
                    cout << "Enter product index (0-" << inventory.size() - 1 << "): ";
                    cin >> productIndex;
                    if (productIndex >= 0 && productIndex < static_cast<int>(inventory.size())) {
                        inventory[productIndex]->displayDetails();
                        cout << "Enter quantity: ";
                        cin >> quantity;
                        try {
                            cart.addToCart(inventory[productIndex], quantity);
                            cout << "Item added to cart." << endl;
                            recommendationSystem.recordPurchase(currentUser, inventory[productIndex]->getName());
                        } catch (const exception& e) {
                            cerr << "Error: " << e.what() << endl;
                        }
                    } else {
                        cout << "Invalid product index. Please try again." << endl;
                    }
                }
                break;
            case 4:
                cart.display();
                break;
            case 5:
                {
                    billing.generateBill(cart);
                    double totalCost = cart.getTotalCost();
                    PercentageDiscount tenPercentOff(10);
                    double discountedCost = Billing::applyDiscount(totalCost, &tenPercentOff);
                    cout << "Discounted Total: KES " << fixed << setprecision(2) << discountedCost << endl;

                    cout << "1. Bank\n2. M-PESA\nChoose payment method: ";
                    int paymentChoice;
                    cin >> paymentChoice;
                    if (paymentChoice == 1) {
                        string accountNumber;
                        cout << "Enter bank account number: ";
                        cin >> accountNumber;
                        processPayment(accountNumber, discountedCost);
                    } else if (paymentChoice == 2) {
                        string phoneNumber, pin;
                        cout << "Enter M-PESA phone number: ";
                        cin >> phoneNumber;
                        cout << "Enter M-PESA PIN: ";
                        cin >> pin;
                        processMpesaPayment(phoneNumber, pin, discountedCost);
                    } else {
                        cout << "Invalid payment method. Transaction cancelled." << endl;
                    }
                }
                return 0;
            case 6:
                {
                    vector<string> recommendations = recommendationSystem.getRecommendations(currentUser);
                    cout << "Recommended products for you:" << endl;
                    for (const auto& product : recommendations) {
                        cout << "- " << product << endl;
                    }
                }
                break;
            case 7:
                cout << "Thank you for using our system!" << endl;
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
