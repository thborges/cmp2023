
#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace std;

class Node {
protected:
	vector<Node*> children;

public:
	void addChild(Node *n) {
		children.push_back(n);
	}

	vector<Node*> const& getChildren() {
		return children;
	}

	virtual string toStr() {
		return "node";
	}
};

class Program: public Node {
public:
	virtual string toStr() override {
		return "program";
	}
};

class Stmt: public Node {
public:
	virtual string toStr() override {
		return "stmt";
	}
};

class Attr: public Node {
protected:
	string ident;
public:
	Attr(string id, Node *expr) {
		ident = id;
		children.push_back(expr);
	}

	virtual string toStr() override {
		string r(ident);
		r.append("=");
		return r;
	}

	const string getName() {
		return ident;
	}
};

class Print: public Node {
public:
	Print(Node *expr) {
		children.push_back(expr);
	}

	virtual string toStr() override {
		return "print";
	}
};

class BinaryOp : public Node {
protected:
	char op;
public:
	BinaryOp(Node *left, Node *right,
		char oper) {
		children.push_back(left);
		children.push_back(right);
		op = oper;
	}

	virtual string toStr() override {
		string r;
		r.push_back(op);
		return r;
	}
};

class Ident: public Node {
protected:
	string name;
public:
	Ident(string name) {
		this->name = name;
	}
	
	virtual string toStr() override {
		return name;
	}

	const string getName() {
		return name;
	}
};

class Float: public Node {
protected:
	double value;
public:
	Float(double v) {
		value = v;
	}
	virtual string toStr() override {
		return to_string(value);
	}
};

class Int: public Node {
protected:
	int value;
public:
	Int(int v) {
		value = v;
	}
	virtual string toStr() override {
		return to_string(value);
	}
};

class PrintTree {
public:
	void printRecursive(Node *n) {
		for(Node *c : n->getChildren()) {
			printRecursive(c);
		}

		cout << "n" << (long)n;
		cout << "[label=\"" << n->toStr() << "\"]";
		cout << ";" << endl;

		for(Node *c : n->getChildren()) {
			cout << "n" << (long)n << " -- " <<
					"n" << (long)c << ";" << endl;
		}
	}

	void print(Node *n) {
		cout << "graph {\n";
		printRecursive(n);
		cout << "}\n";
	}
};


class CheckUndeclaredVar {
private:
	set<string> vars;
public:
	void checkRecursive(Node *n) {
		// visit left and right
		for(Node *c : n->getChildren()) {
			checkRecursive(c);
		}

		// visit root
		Attr *a = dynamic_cast<Attr*>(n);
		if (a) {
			// visiting an Attr node, new var
			vars.insert(a->getName());
		} else {
			// visiting an Ident(load) node,
			// check if var exists
			Ident *i = dynamic_cast<Ident*>(n);
			if (i) {
				if (vars.count(i->getName()) == 0) {
					// undeclared var
					cout << "Undeclared var " <<
						i->getName() << endl;
				}
			}
		}
		
	}

	void check(Node *n) {
		checkRecursive(n);
	}
};

