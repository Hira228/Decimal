CC = gcc
LIBRARY = s21_decimal.a
REPORT = gcov_report
PREF_SRC_TEST = ./New_Test/
FLAGS = -Wall -Werror -Wextra -std=c11 
GCOV_FLAGS = -fprofile-arcs -ftest-coverage -lgcov 
SRC = $(wildcard *.c)
UNAME := $(shell uname)
OBJ =  $(SRC:%.c=%.o)
SRC_TEST = $(wildcard $(PREF_SRC_TEST)*.c)

OS = $(shell uname)
ifeq ($(OS), Linux)
	TEST_FLAGS = -lcheck -lsubunit -lpthread -lm
else
	TEST_FLAGS = -lcheck -lpthread -lm
endif

all: clean $(LIBRARY) test $(REPORT)

$(LIBRARY): ${OBJ}
	ar rc $(LIBRARY) ${OBJ}
	ranlib $(LIBRARY)

$(REPORT): $(SRC_TEST) $(SRC) build_file
	$(CC) $(FLAGS) $(GCOV_FLAGS) $(SRC_TEST) $(SRC) $(TEST_FLAGS) -o Build/$(REPORT)
	Build/./$(REPORT)
	lcov -o $(REPORT).info -c -d .
	genhtml $(REPORT).info -o html_report
	open html_report/index.html

test : $(LIBRARY) $(SRC_TEST)
	$(CC) $(FLAGS) $(SRC_TEST) -L. $(LIBRARY)  $(TEST_FLAGS) -o test
	./test
	
%.o : %.c
	$(CC) -c $< -o $@
build_file: 
	mkdir Build
clean: 
	rm -rf Build test *.o *.a ./html_report/ gcov_report *.gcno *.gcda *.info *.out

