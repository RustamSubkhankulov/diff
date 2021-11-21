CC = g++

CFLAGS = -lubsan -D DEBUG -g -std=c++14 -fmax-errors=1 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align  -Wchar-subscripts -Wconditionally-supported -Wconversion  -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline  -Wlogical-op -Wmissing-declarations  -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192  -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types  -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef  -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation  -fstack-protector -fstrict-overflow   -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address  -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum  -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -lm -pie

all: global

global: obj/diff.o obj/general.o obj/main.o obj/logs.o obj/text.o
	$(CC) obj/diff.o obj/general.o obj/main.o obj/logs.o obj/text.o -o diff.exe

obj/diff.o: diff/diff.cpp diff/diff.h diff/diff_config.h
	$(CC) diff/diff.cpp -c -o obj/diff.o $(CFLAGS)

obj/general.o: general/general.cpp general/general.h
	$(CC) general/general.cpp -c -o obj/general.o $(CFLAGS)

obj/main.o: main.cpp
	$(CC) main.cpp -c -o obj/main.o $(CFLAGS) 

obj/logs.o: logs/errors_and_logs.cpp logs/errors_and_logs.h logs/errors.h logs/log_definitions.h include/errors.txt
	$(CC) logs/errors_and_logs.cpp -c -o obj/logs.o $(CFLAGS) 

obj/text.o: text_processing/text_processing.cpp text_processing/text_processing.h
	$(CC) text_processing/text_processing.cpp -c -o obj/text.o $(CFLAGS)

clean:
	rm obj/*.o  

deletepng:
	rm /tmp/diff_images/*.png

makeimagesdir: 
	mkdir /tmp/diff_images