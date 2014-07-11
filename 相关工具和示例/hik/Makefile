include ./Makefile.pre

#define DIRs
SRC_ROOT	=	./
COMMON_PATH	=	$(SRC_ROOT)
#define include DIRs
INC	=-I. \
		-I/usr/include/\	
			
#define librarys

SHAREDLIB_PATH = ./
SHAREDLIB=-lhcnetsdk

#define output 
ifeq ($(VER), debug)
OUTPUT = ./libvprctrl.so
else
OUTPUT = ./libvprctrl.so
endif

SHAREDLIB_EXE=-lhcnetsdk -lvprctrl  -lpthread
OUTPUT_EXE = ./hik

#define obj
OBJS_PATH       =	./
#列举所有cpp文件 
SRCS            =   $(wildcard $(COMMON_PATH)/*.c)  
#列举所有obj文件  
OBJS            :=  $(SRCS:%.c=%.o)
#列举修改全路径
OBJS            :=  $(OBJS:$(COMMON_PATH)/%=$(OBJS_PATH)/%)

#连接选项  shared 动态库  消除警告
LINKOPTION = -o $(OUTPUT) -shared -Wl,-rpath=./

LINKOPTION_EXE= -o $(OUTPUT_EXE) -Wl,-rpath=./

#编译选项  编译器   Release/Debug    显示警告	“$@” 目标 “$<”依赖  头文件目录 
COMPILE         =   $(CPPCOMPILE)  $(CFG_DEBUG) $(CFG_FLAGS) -c "$<" -o "$@" $(INC)

#COMPILE objs
$(OBJS_PATH)/%.o	:	$(COMMON_PATH)/%.c
	@echo "~~~~~~~~~~~~~~~~~~~~"
	@echo "| MAKE COMMON MODUL |"
	@echo "~~~~~~~~~~~~~~~~~~~~"
	$(COMPILE)	
	@echo "******************************************************"

all:$(OBJS)

	$(LINK) $(LINKOPTION) $(OBJS) -L$(SHAREDLIB_PATH) $(SHAREDLIB) 
	
	@echo "******************************************************"
	@echo "Compile Sucess!!"
	@echo "******************************************************"
	
	$(LINK) $(LINKOPTION_EXE)  -L$(SHAREDLIB_PATH) $(SHAREDLIB_EXE) main.o

ifneq ($(VER),debug)	
	$(STRIP) $(OUTPUT)
	$(STRIP) $(OUTPUT_EXE)
	@echo "Strip Sucess!!"
endif


clean:
	rm -f $(OBJS_PATH)/*.o $(OUTPUT)

