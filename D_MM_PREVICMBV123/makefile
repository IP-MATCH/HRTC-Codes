SYSTEM = x86-64_sles10_4.1
LIBFORMAT = static_pic

GUROBI_DIR = /home/mdelorme/gurobi752/linux64
CLIB     = -L$(GUROBI_DIR)/lib/ -lgurobi75
INC      = $(GUROBI_DIR)/include/
CPPLIB   = -L$(GUROBI_DIR)/lib/ -lgurobi_c++ $(CLIB)
GUROBI_OPTS = -I$(INC) $(CPPLIB) -lpthread -lm -m64 

CC = g++ 
CFLAGS = -O2 -Wall -ansi -pedantic -DIL_STD 
DEBUG = -pg -g -Wall -ansi -pedantic -DIL_STD 
OBJECTS = main.o Allocation.o time.o
 
exec : $(OBJECTS) 
	$(CC) $(CFLAGS) -o MM_PREVICMBV3 $(OBJECTS) $(GUROBI_OPTS)
 
.cpp.o : 
	$(CC) $(CFLAGS) $(GUROBI_OPTS) -c $< -o $@ 
 
clean : 
	rm -f $(OBJECTS) MM_PREVICMBV3