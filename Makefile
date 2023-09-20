LLVM_FLAGS=`llvm-config --cxxflags --ldflags --libs`

all:
	flex -o calc.yy.c calc.l 
	bison -Hgramatica.h calc.y
	#bison -d calc.y && mv calc.tab.h gramatica.h
	#sed s/calc.tab.h/gramatica.h/ -i calc.tab.c 
	clang++ -O0 -ggdb $(LLVM_FLAGS) calc.tab.c calc.yy.c -o compilador

clean:
	rm -f compilador gramatica.h calc.tab.c calc.yy.c
