
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
};

class Arit: public Node {
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
};

class Int: public Node {
};

