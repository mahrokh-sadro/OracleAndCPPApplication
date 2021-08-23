#include <iostream>
#include <occi.h>

using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

struct ShoppingCart {
	int product_id;
	double price;
	int quantity;
};


int mainMenu();
int customerLogin(Connection* conn, int custId);
int addToCart(Connection* conn, ShoppingCart cart[]);
double findProduct(Connection* conn, int product_id);
void displayProducts(struct ShoppingCart cart[], int productCount);
int checkout(Connection* conn, struct ShoppingCart car[], int customerId, int productCount);

int main() {                             
	

	try {

		Environment* env{};
		Connection* conn{};
		string user = "dbs311_212a29";// oracle username
		string pass = "24544316";//oracle password
		string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(user, pass, constr);
		cout << "Connection is successful!" << endl;
	
		int customerID{};
		int option{-1};
		int noOfItems{};
		while (option != 0) {

			option = mainMenu();

			if (option == 1) {
				cout << "Enter the customer ID: ";
				cin >> customerID;

				if (customerLogin(conn, customerID) == 0) cout << "The customer does not exist."<<endl;		
				else if (customerLogin(conn, customerID) == 1) {
					ShoppingCart myCart[5];
					noOfItems = addToCart(conn, myCart);
					displayProducts(myCart, noOfItems);
					checkout(conn, myCart, customerID, noOfItems);
				}

			}
		}

		cout << "Thank you --- Good bye..." << endl;
		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
	}
	catch (SQLException& sqlExcp) {
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
	return 0;
}


int mainMenu() {

	int option{};

	do {
		cout << "*************Main Menu by Mahrokh ********************"<<endl
			 << "1)\tLogin"<<endl
			 << "0)\tExit"<<endl;

		if (option != 0 && option != 1)  cout << "You entered a wrong value. Please enter an option (0-1): ";
		else cout << "Enter an option (0-1): ";

		cin >> option;

	} while (option != 0 && option != 1);

	return option;
}

void displayProducts(ShoppingCart cart[], int productCount) {

	double amount{};

	cout << "------- Ordered Products -----------------------------" << endl;

	for (int i = 0; i < productCount; i++) {

		cout << "---Item " << i + 1
			<< "\tProduct ID: " << cart[i].product_id
			<< "\tPrice: " << cart[i].price
			<< "\tQuantity: " << cart[i].quantity << endl;
		amount += (cart[i].price * cart[i].quantity);
	}
	cout << "-------------------------------------------------------" << endl
		<< "Total: " << amount << endl
		<< "=======================================================" << endl;

}

int customerLogin(Connection* conn, int customerId) {
	int ret{};
	Statement* stmt = conn->createStatement();
	stmt->setSQL("BEGIN find_customer(:1, :2); END;");
	stmt->setInt(1, customerId);
	stmt->registerOutParam(2, Type::OCCIINT);
	stmt->executeUpdate();
	ret = stmt->getInt(2);
	conn->terminateStatement(stmt);
	return ret;
}


int addToCart(Connection* conn, ShoppingCart cart[]) {
	ShoppingCart myCart;
	
	int productQuantity{ -1 };
	int productID{-1};
	int	option{};
	int ret{};
	
	cout << "-------------- Add Products to Cart --------------" << endl;
	int i{};
	for (i = 0; i < 5; i++) {
		do{
			cout << "Enter the product ID: ";
			cin >> productID;
			if (findProduct(conn, productID) == 0) 	cout << "The product does not exist. Please try again..." << endl;
			
		} while (findProduct(conn, productID) == 0);
		cout << "Product Price: " << findProduct(conn, productID) << endl;
		cout << "Enter the product Quantity: ";
		cin >> productQuantity;
		myCart.product_id = productID;
		myCart.price = findProduct(conn, productID);
		myCart.quantity = productQuantity;
		cart[i] = myCart;

		if (i == 4) i = 5; 
		else {
			do {
				cout << "Enter 1 to add more products or 0 to checkout: ";
				cin >> option;
			} while (option != 0 && option != 1);
		}
		if (option == 0) i = 5; 
	
		ret++;
	}
	return ret;
}

double findProduct(Connection* conn, int product_id) {
	double ret{};
	Statement* stmt = conn->createStatement();
	stmt->setSQL("BEGIN find_product(:1, :2); END;");
	stmt->setInt(1, product_id);
	stmt->registerOutParam(2, OCCIDOUBLE);
	stmt->executeUpdate();
	ret = stmt->getDouble(2);
	conn->terminateStatement(stmt);
	return ret;
}

int checkout(Connection* conn, ShoppingCart cart[], int customerId, int productCount) {
	int ret{};
	int myOrder{};
	int	i{};
	char ch{};
	do {
		cout << "Would you like to checkout ? (Y / y or N / n) ";
		cin >> ch;
		if (ch != 'Y' && ch != 'y' && ch != 'N' && ch != 'n') 	cout << "Wrong input. Please try again..." << endl;
		
	} while (ch != 'Y' && ch != 'y' && ch != 'N' && ch != 'n');

	if (ch == 'Y' || ch == 'y') {

		Statement* stmt = conn->createStatement();
		stmt->setSQL("BEGIN add_order(:1, :2); END;");
		stmt->setInt(1, customerId);
		stmt->registerOutParam(2, Type::OCCIINT);
		stmt->executeUpdate();
		myOrder = stmt->getInt(2);
		for (i = 0; i < productCount; i++) {
			stmt->setSQL("BEGIN add_orderline(:1, :2, :3, :4, :5); END;");
			stmt->setInt(1, myOrder);
			stmt->setInt(2, i + 1);
			stmt->setInt(3, cart[i].product_id);
			stmt->setInt(4, cart[i].quantity);
			stmt->setDouble(5, cart[i].price);
			stmt->executeUpdate();
		}

		cout << "The order is successfully completed." << endl;
		conn->terminateStatement(stmt);
		ret = 1;
	}
	else if (ch == 'N' || ch == 'n') 	cout << "The order is cancelled." << endl;
	return ret;
}