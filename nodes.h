
#include <iostream>
#include <vector>
#include <map>
#include <set>

#include "backllvm.h"

using namespace std;

int errorcount = 0;

// symbol table
map<string, Value*> symbols;

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

	virtual Value* codegen() {
		for(Node *n : children) {
			n->codegen();
		}
		return NULL;
	}
};

class Program: public Node {
public:
	virtual string toStr() override {
		return "program";
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

	virtual Value* codegen() override {
		Value *exprv = children[0]->codegen();

		Value *address = NULL;
		if (symbols.count(ident) == 0) {
			address = backend.CreateAlloca(
				exprv->getType(), 0, NULL,
				ident);
			symbols[ident] = address;
		} else {
			address = symbols[ident];
		}

		return backend.CreateStore(exprv, address);
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

	virtual Value* codegen() override {
		Value *exprv = children[0]->codegen();
		vector<Value*> args;
		args.push_back(exprv);
		return backend.CreateCall(printfloat, args);
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

	virtual Value* codegen() override {
		Value *lv = children[0]->codegen();
		Value *rv = children[1]->codegen();
		switch (op) {
			case '+': return backend.CreateFAdd(lv, rv);
			case '-': return backend.CreateFSub(lv, rv);
			case '*': return backend.CreateFMul(lv, rv);
			case '/': return backend.CreateFDiv(lv, rv);

			default: cerr << "Fail! Operador não implementado: " << op << endl;
		}
		return NULL;
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

	virtual Value* codegen() override {
		Value *symbol = symbols[name];
		AllocaInst* ai = dyn_cast<AllocaInst>(symbol);
		Type *st = ai->getAllocatedType();
		return backend.CreateLoad(st, symbol, name);
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
	virtual Value* codegen() override {
		return ConstantFP::get(ctx, APFloat(value));
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
	virtual Value* codegen() override {
		return ConstantFP::get(ctx, APFloat((double)value));
	}
};

class While: public Node {
public:
	While(Node *logical, Node *stmts) {
		children.push_back(logical);
		children.push_back(stmts);
	}

	virtual string toStr() override {
		return "while";
	}

	virtual Value* codegen() override {
		BasicBlock *condition = BasicBlock::Create(ctx, "cond", current_func);
		BasicBlock *body = BasicBlock::Create(ctx, "body", current_func);
		BasicBlock *contin = BasicBlock::Create(ctx, "contin", current_func);

		// setup entry block, goto condition
		backend.CreateBr(condition);

		// setup condition block
		backend.SetInsertPoint(condition);
		Value *expr = children[0]->codegen();
		backend.CreateCondBr(expr, body, contin);

		// setup body block
		backend.SetInsertPoint(body);
		children[1]->codegen();
		backend.CreateBr(condition);

		backend.SetInsertPoint(contin);
		return contin;
	}

};

class Logical: public Node {
protected:
	char oper;
public:
	Logical(Node *le, char op, Node *re) {
		children.push_back(le);
		children.push_back(re);
		oper = op;
	}
	virtual string toStr() override {
		string r;
		r.push_back(oper);
		return r;
	}
	virtual Value* codegen() override {
		Value *lv = children[0]->codegen();
		Value *rv = children[1]->codegen();
		switch (oper) {
			case '>': return backend.CreateFCmpOGT(lv, rv);
			default: cerr << "Fail! Operador não implementado: " << oper << endl;
		}
		return NULL;
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
					errorcount++;
				}
			}
		}
		
	}

	void check(Node *n) {
		checkRecursive(n);
	}
};


class CodeGen {
public:
	void generate(Node *p) {
		setup_llvm();
		p->codegen();

		// terminate main function
		Value *retv = ConstantInt::get(ctx, APInt(16, 0));
		backend.CreateRet(retv);

		module->print(outs(), nullptr);
		print_llvm_ir();
	}
};

