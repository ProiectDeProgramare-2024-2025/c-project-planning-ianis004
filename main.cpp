#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

class Date {
public:
    int day, month, year;

    Date() : day(1), month(1), year(2024) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    string toString() const {
        return to_string(day) + "/" + to_string(month) + "/" + to_string(year);
    }

    static Date getCurrentDate() {
        return Date(24, 5, 2025);
    }
};

class Product {
public:
    string barcode;
    string name;
    int quantity;
    double price;

    Product() : barcode(""), name(""), quantity(0), price(0.0) {}
    Product(string bc, string n, int q, double p)
        : barcode(bc), name(n), quantity(q), price(p) {}

    string toString() const {
        return barcode + " " + name + " " + to_string(quantity) + " " + to_string(price);
    }
};

class Order {
public:
    vector<Product> products;
    Date date;

    Order() : date(Date::getCurrentDate()) {}
    Order(vector<Product> prods, Date d) : products(prods), date(d) {}
};

class User {
public:
    string username;
    string password;
    bool isAdmin;

    User(string u, string p, bool admin) : username(u), password(p), isAdmin(admin) {}
};

class StoreSystem {
private:
    vector<Product> stock;
    vector<Order> orders;
    vector<Product> cart;
    vector<User> users;
    User* currentUser;

    const string STOCK_FILE = "stoc.txt";
    const string ORDERS_FILE = "comenzi.txt";
    const string CART_FILE = "cos_cumparaturi.txt";
    const string USERS_FILE = "users.txt";

    const string RED = "\033[31m";
    const string RESET = "\033[0m";

public:
    StoreSystem() : currentUser(nullptr) {
        initializeUsers();
        loadStock();
        loadOrders();
        loadCart();
    }

    void initializeUsers() {
        users.push_back(User("admin", "admin123", true));
        users.push_back(User("user", "user123", false));
        users.push_back(User("ion", "ion123", true));
        users.push_back(User("maria", "maria123", false));
    }

    bool isValidInput(const string& input) {
        for (char c : input) {
            if (!isalpha(c) && !(c >= '1' && c <= '9')) {
                return false;
            }
        }
        return !input.empty();
    }

    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    bool login() {
        string username, password;
        cout << "=== Store Management System ===" << endl;
        cout << "Login Required" << endl;
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        if (!isValidInput(username) || !isValidInput(password)) {
            cout << RED << "Incorrect! Only alphabet characters and numbers from 1 to 9 are allowed." << RESET << endl;
            return false;
        }

        for (auto& user : users) {
            if (user.username == username && user.password == password) {
                currentUser = &user;
                cout << "Login successful! Welcome, " << username << endl;
                if (user.isAdmin) {
                    cout << "Admin privileges granted." << endl;
                } else {
                    cout << "Customer privileges granted." << endl;
                }
                return true;
            }
        }

        cout << RED << "Incorrect! Only alphabet characters and numbers from 1 to 9 are allowed." << RESET << endl;
        return false;
    }

    void logout() {
        if (currentUser) {
            cout << "Goodbye, " << currentUser->username << "!" << endl;
            currentUser = nullptr;
        }
    }

    void loadStock() {
        ifstream file(STOCK_FILE);
        if (!file.is_open()) return;

        int numProducts;
        file >> numProducts;
        file.ignore();

        stock.clear();
        for (int i = 0; i < numProducts; i++) {
            string line;
            getline(file, line);
            if (!line.empty()) {
                istringstream iss(line);
                string barcode, name;
                int quantity;
                double price;

                iss >> barcode >> name >> quantity >> price;
                stock.push_back(Product(barcode, name, quantity, price));
            }
        }
        file.close();
    }

    void saveStock() {
        ofstream file(STOCK_FILE);
        if (!file.is_open()) return;

        file << stock.size() << endl;
        for (const auto& product : stock) {
            file << product.toString() << endl;
        }
        file.close();
    }

    void loadOrders() {
        ifstream file(ORDERS_FILE);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                Order order;
                orders.push_back(order);
            }
        }
        file.close();
    }

    void saveOrders() {
        ofstream file(ORDERS_FILE);
        if (!file.is_open()) return;

        for (const auto& order : orders) {
            file << order.date.toString() << endl;
            for (const auto& product : order.products) {
                file << product.barcode << " ";
            }
            file << endl;
        }
        file.close();
    }

    void loadCart() {
        if (currentUser && currentUser->isAdmin) return;

        ifstream file(CART_FILE);
        if (!file.is_open()) return;

        cart.clear();
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                istringstream iss(line);
                string barcode;
                int quantity;
                iss >> barcode >> quantity;

                for (const auto& stockProduct : stock) {
                    if (stockProduct.barcode == barcode) {
                        Product cartProduct = stockProduct;
                        cartProduct.quantity = quantity;
                        cart.push_back(cartProduct);
                        break;
                    }
                }
            }
        }
        file.close();
    }

    void saveCart() {
        if (currentUser && currentUser->isAdmin) return;

        ofstream file(CART_FILE);
        if (!file.is_open()) return;

        for (const auto& product : cart) {
            file << product.barcode << " " << product.quantity << endl;
        }
        file.close();
    }

    void viewStockProducts() {
        if (!currentUser || !currentUser->isAdmin) {
            cout << "Access denied! Admin privileges required." << endl;
            return;
        }

        cout << "\n=== STOCK PRODUCTS ===" << endl;
        cout << left << setw(12) << "Barcode" << setw(20) << "Name"
             << setw(10) << "Quantity" << setw(10) << "Price" << endl;
        cout << string(52, '-') << endl;

        for (const auto& product : stock) {
            cout << left << setw(12) << product.barcode << setw(20) << product.name
                 << setw(10) << product.quantity << setw(10) << fixed << setprecision(2) << product.price << endl;
        }
        cout << endl;
    }

    void addProduct(string barcode, string name, int quantity, double price) {
        if (!currentUser || !currentUser->isAdmin) {
            cout << "Access denied! Admin privileges required." << endl;
            return;
        }

        for (auto& product : stock) {
            if (product.barcode == barcode) {
                cout << RED << "Error: A product with barcode '" << barcode << "' already exists!" << RESET << endl;
                cout << "Existing product: " << product.name << " (Quantity: " << product.quantity << ")" << endl;
                cout << "Cannot add duplicate barcode. Please use a different barcode." << endl;
                return;
            }
        }

        stock.push_back(Product(barcode, name, quantity, price));
        cout << "Product added successfully!" << endl;
        saveStock();
    }

    void deleteProduct(string barcode) {
        if (!currentUser || !currentUser->isAdmin) {
            cout << "Access denied! Admin privileges required." << endl;
            return;
        }

        auto it = remove_if(stock.begin(), stock.end(),
                           [barcode](const Product& p) { return p.barcode == barcode; });

        if (it != stock.end()) {
            stock.erase(it, stock.end());
            cout << "Product deleted successfully!" << endl;
            saveStock();
        } else {
            cout << "Product not found!" << endl;
        }
    }

    void modifyProduct(string type, string barcode, double newValue) {
        if (!currentUser || !currentUser->isAdmin) {
            cout << "Access denied! Admin privileges required." << endl;
            return;
        }

        for (auto& product : stock) {
            if (product.barcode == barcode) {
                if (type == "price") {
                    product.price = newValue;
                } else if (type == "quantity") {
                    product.quantity = (int)newValue;
                }
                cout << "Product modified successfully!" << endl;
                saveStock();
                return;
            }
        }
        cout << "Product not found!" << endl;
    }

    void viewOrders() {
        if (!currentUser || !currentUser->isAdmin) {
            cout << "Access denied! Admin privileges required." << endl;
            return;
        }

        cout << "\n=== ORDERS ===" << endl;
        for (size_t i = 0; i < orders.size(); i++) {
            cout << "Order " << (i + 1) << " - Date: " << orders[i].date.toString() << endl;
            cout << "Products: ";
            for (const auto& product : orders[i].products) {
                cout << product.name << " (x" << product.quantity << ") ";
            }
            cout << endl << endl;
        }
    }

    void viewCart() {
        if (!currentUser || currentUser->isAdmin) {
            cout << "Access denied! Customer privileges required." << endl;
            return;
        }

        cout << "\n=== SHOPPING CART ===" << endl;
        if (cart.empty()) {
            cout << "Your cart is empty." << endl;
            return;
        }

        double total = 0.0;
        cout << left << setw(12) << "Barcode" << setw(20) << "Name"
             << setw(10) << "Quantity" << setw(10) << "Price" << setw(12) << "Subtotal" << endl;
        cout << string(64, '-') << endl;

        for (const auto& product : cart) {
            double subtotal = product.quantity * product.price;
            total += subtotal;
            cout << left << setw(12) << product.barcode << setw(20) << product.name
                 << setw(10) << product.quantity << setw(10) << fixed << setprecision(2) << product.price
                 << setw(12) << subtotal << endl;
        }
        cout << string(64, '-') << endl;
        cout << "Total: $" << fixed << setprecision(2) << total << endl << endl;
    }

    void addToCart(string barcode, int quantity) {
        if (!currentUser || currentUser->isAdmin) {
            cout << "Access denied! Customer privileges required." << endl;
            return;
        }

        Product* stockProduct = nullptr;
        for (auto& product : stock) {
            if (product.barcode == barcode) {
                stockProduct = &product;
                break;
            }
        }

        if (!stockProduct) {
            cout << "Product not found in stock!" << endl;
            return;
        }

        if (stockProduct->quantity < quantity) {
            cout << "Insufficient stock! Available: " << stockProduct->quantity << endl;
            return;
        }

        for (auto& cartProduct : cart) {
            if (cartProduct.barcode == barcode) {
                cartProduct.quantity += quantity;
                cout << "Product quantity updated in cart!" << endl;
                saveCart();
                return;
            }
        }

        Product cartProduct = *stockProduct;
        cartProduct.quantity = quantity;
        cart.push_back(cartProduct);
        cout << "Product added to cart!" << endl;
        saveCart();
    }

    void modifyCartProduct(string barcode, int newQuantity) {
        if (!currentUser || currentUser->isAdmin) {
            cout << "Access denied! Customer privileges required." << endl;
            return;
        }

        for (auto& product : cart) {
            if (product.barcode == barcode) {
                if (newQuantity <= 0) {
                    deleteFromCart(barcode);
                    return;
                }

                for (const auto& stockProduct : stock) {
                    if (stockProduct.barcode == barcode && stockProduct.quantity < newQuantity) {
                        cout << "Insufficient stock! Available: " << stockProduct.quantity << endl;
                        return;
                    }
                }

                product.quantity = newQuantity;
                cout << "Cart product quantity updated!" << endl;
                saveCart();
                return;
            }
        }
        cout << "Product not found in cart!" << endl;
    }

    void deleteFromCart(string barcode) {
        if (!currentUser || currentUser->isAdmin) {
            cout << "Access denied! Customer privileges required." << endl;
            return;
        }

        auto it = remove_if(cart.begin(), cart.end(),
                           [barcode](const Product& p) { return p.barcode == barcode; });

        if (it != cart.end()) {
            cart.erase(it, cart.end());
            cout << "Product removed from cart!" << endl;
            saveCart();
        } else {
            cout << "Product not found in cart!" << endl;
        }
    }

    void purchase() {
        if (!currentUser || currentUser->isAdmin) {
            cout << "Access denied! Customer privileges required." << endl;
            return;
        }

        if (cart.empty()) {
            cout << "Cart is empty!" << endl;
            return;
        }

        for (const auto& cartProduct : cart) {
            for (auto& stockProduct : stock) {
                if (stockProduct.barcode == cartProduct.barcode) {
                    if (stockProduct.quantity < cartProduct.quantity) {
                        cout << "Insufficient stock for " << cartProduct.name
                             << "! Available: " << stockProduct.quantity << endl;
                        return;
                    }
                    break;
                }
            }
        }

        Order newOrder(cart, Date::getCurrentDate());
        orders.push_back(newOrder);

        for (const auto& cartProduct : cart) {
            for (auto& stockProduct : stock) {
                if (stockProduct.barcode == cartProduct.barcode) {
                    stockProduct.quantity -= cartProduct.quantity;
                    break;
                }
            }
        }

        double total = 0.0;
        for (const auto& product : cart) {
            total += product.quantity * product.price;
        }

        cout << "Order placed successfully!" << endl;
        cout << "Total amount: $" << fixed << setprecision(2) << total << endl;
        cout << "Order date: " << newOrder.date.toString() << endl;

        cart.clear();
        saveCart();
        saveStock();
        saveOrders();
    }

    void showMenu() {
        if (!currentUser) return;

        cout << "\n=== MAIN MENU ===" << endl;
        if (currentUser->isAdmin) {
            cout << "1. View Stock Products" << endl;
            cout << "2. Add Product" << endl;
            cout << "3. Delete Product" << endl;
            cout << "4. Modify Product" << endl;
            cout << "5. View Orders" << endl;
            cout << "6. Clear Screen" << endl;
        } else {
            cout << "1. View Cart" << endl;
            cout << "2. Add Product to Cart" << endl;
            cout << "3. Modify Cart Product" << endl;
            cout << "4. Delete from Cart" << endl;
            cout << "5. Purchase" << endl;
            cout << "6. View Available Products" << endl;
            cout << "7. Clear Screen" << endl;
        }
        cout << "0. Logout" << endl;
        cout << "Choice: ";
    }

    void handleMenu() {
        if (!currentUser) return;

        int choice;
        cin >> choice;

        if (currentUser->isAdmin) {
            switch (choice) {
                case 1: viewStockProducts(); break;
                case 2: {
                    string barcode, name;
                    int quantity;
                    double price;
                    cout << "Enter barcode: "; cin >> barcode;
                    cout << "Enter name: "; cin >> name;
                    cout << "Enter quantity: "; cin >> quantity;
                    cout << "Enter price: "; cin >> price;
                    addProduct(barcode, name, quantity, price);
                    break;
                }
                case 3: {
                    string barcode;
                    cout << "Enter barcode to delete: "; cin >> barcode;
                    deleteProduct(barcode);
                    break;
                }
                case 4: {
                    string type, barcode;
                    double newValue;
                    cout << "Modify (price/quantity): "; cin >> type;
                    cout << "Enter barcode: "; cin >> barcode;
                    cout << "Enter new value: "; cin >> newValue;
                    modifyProduct(type, barcode, newValue);
                    break;
                }
                case 5: viewOrders(); break;
                case 6: clearScreen(); break;
                case 0: logout(); return;
                default: cout << "Invalid choice!" << endl;
            }
        } else {
            switch (choice) {
                case 1: viewCart(); break;
                case 2: {
                    string barcode;
                    int quantity;
                    cout << "Enter barcode: "; cin >> barcode;
                    cout << "Enter quantity: "; cin >> quantity;
                    addToCart(barcode, quantity);
                    break;
                }
                case 3: {
                    string barcode;
                    int quantity;
                    cout << "Enter barcode: "; cin >> barcode;
                    cout << "Enter new quantity: "; cin >> quantity;
                    modifyCartProduct(barcode, quantity);
                    break;
                }
                case 4: {
                    string barcode;
                    cout << "Enter barcode to remove: "; cin >> barcode;
                    deleteFromCart(barcode);
                    break;
                }
                case 5: purchase(); break;
                case 6: {
                    cout << "\n=== AVAILABLE PRODUCTS ===" << endl;
                    cout << left << setw(12) << "Barcode" << setw(20) << "Name"
                         << setw(10) << "Stock" << setw(10) << "Price" << endl;
                    cout << string(52, '-') << endl;
                    for (const auto& product : stock) {
                        if (product.quantity > 0) {
                            cout << left << setw(12) << product.barcode << setw(20) << product.name
                                 << setw(10) << product.quantity << setw(10) << fixed << setprecision(2) << product.price << endl;
                        }
                    }
                    cout << endl;
                    break;
                }
                case 7: clearScreen(); break;
                case 0: logout(); return;
                default: cout << "Invalid choice!" << endl;
            }
        }
    }

    void run() {
        cout << "Welcome to Store Management System!" << endl;
        cout << "Default users:" << endl;
        cout << "Admin: username='admin', password='admin123'" << endl;
        cout << "Admin: username='ion', password='ion123'" << endl;
        cout << "Customer: username='user', password='user123'" << endl;
        cout << "Customer: username='maria', password='maria123'" << endl;
        cout << endl; 

        while (true) {
            if (!currentUser) {
                if (!login()) {
                    cout << "Exiting..." << endl;
                    break;
                }
                loadCart();
            }

            showMenu();
            handleMenu();

            if (!currentUser) break;
        }
    }
};

int main() {
    StoreSystem store;
    store.run();
    return 0;
}