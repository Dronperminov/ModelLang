FILES=main.cpp entities/*.cpp analyzers/*.cpp
STD=14
OPTIMIZE_LEVEL=3
FLAGS=-Wall -Wunreachable-code -pedantic
CLANG_FLAGS=-Wunreachable-code-break -Wunreachable-code-loop-increment -Wunreachable-code-return

all: clang_release

clang_release:
	clang++ $(FLAGS) $(CLANG_FLAGS) -O$(OPTIMIZE_LEVEL) -std=c++$(STD) $(FILES) -o modelLang

clang_debug:
	clang++ $(FLAGS) $(CLANG_FLAGS) -O$(OPTIMIZE_LEVEL) -std=c++$(STD) -fsanitize=address $(FILES) -o modelLang

gcc_release:
	g++ $(FLAGS) -O$(OPTIMIZE_LEVEL) -std=c++$(STD) $(FILES) -o modelLang

gcc_debug:
	g++ $(FLAGS) -O$(OPTIMIZE_LEVEL) -std=c++$(STD) -fsanitize=address $(FILES) -o modelLang

clean:
	rm -rf modelLang
