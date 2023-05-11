
include ../CONFIG.mine

CC = g++

SRCDIR = source
SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
ARCH = -mtune=native -maes -mpclmul -msse4.1 -mavx -mbmi2 -madx -march=core2
CFLAGS = -Wall -std=c++14 -pedantic -Wextra  -pthread -I$(ROOT)/src $(ARCH) $(FLAGS) $(OPT)
CPPFLAGS = $(CFLAGS)
LDLIBS = -L/$(OSSL)/lib -lm -lssl -lcrypto -lmpirxx -lmpir -lcryptopp -lmysqlcppconn $(LDFLAGS)

all: Test.x Test-FHE.x Test-Adv-FHE.x Test-Circuit.x FHE-P.x Test-Simplify.x Test-Convert.x  Test-FP.x Test-gfp.x Test-Mod2-CAS.x Test-Merge.x Test-Serialization.x Test-Choicebits.x Test-GC_Storage.x

LIB = ../src/libMPC.a

.PRECIOUS: %.o
%.o: %.cpp
	$(CC) $(CFLAGS) $(SOURCES) -o $@ -c $<

%.x: %.o
	$(CC) $(CFLAGS) -o $@ $< $(LIB) $(LDLIBS)

.PHONY: all test clean

clean:
	- rm *.o *.x
