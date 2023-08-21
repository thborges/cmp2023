
#include <iostream>
#include <vector>
#include <map>

using namespace std;

class Node {
protected:
	vector<Node*> children;

public:
	void addChild(Node *n) {
		children.push_back(n);
	}
};

class Program: public Node {
};

class Stmt: public Node {
};

class Attr: public Node {
protected:
	string ident;
public:
	Attr(string id, Node *expr) {
		ident = id;
		children.push_back(expr);
	}
};

class Print: public Node {
public:
	Print(Node *expr) {
		children.push_back(expr);
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
};

class Ident: public Node {
protected:
	string name;
public:
	Ident(string name) {
		this->name = name;
	}
};

class Float: public Node {
protected:
	double value;
public:
	Float(double v) {
		value = v;
	}
};

class Int: public Node {
protected:
	int value;
public:
	Int(int v) {
		value = v;
	}
};

