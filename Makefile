################################################################################

MAKE        = mingw32-make

CXX			= g++
RM			= del /s
MKDIR       = md
EXT			= .exe

INCLUDE_PATHS	= -I./include -I./source
LIB_PATHS		= -L./lib
LINKER_FLAGS	= -lCatch2 

CXX_FLAGS	= -m64 -std=c++20 $(INCLUDE_PATHS)
DBG_FLAGS	= -O0 -Wall -ggdb -Wno-reorder
TST_FLAGS	= -O0 -Wall -ggdb -Wno-reorder

################################################################################

define make_dir
	$(foreach var, $(1), if not exist $(var) $(md) $(var))
endef

################################################################################

SRC_DIR		:= source
SRC_SB_DIRS	:= $(patsubst %/, %, $(filter %/, $(wildcard $(SRC_DIR)/*/)) )
SRC_DIRS	:= $(SRC_DIR)$(SRC_SB_DIRS)
OBJ_DIR		:= obj

SRCS		:= $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
DBG_DEPS    := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.d)
DBG_OBJS	:= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TST_DIR		:= test
TST_SB_DIRS	:= $(patsubst %/, %, $(filter %/, $(wildcard $(TST_DIR)/*/)) )
TST_DIRS    := $(TST_DIR)$(TST_SB_DIRS)
TST_OBJ_DIR := test/obj

TST_CPPS	:= $(foreach dir, $(TST_DIRS), $(wildcard $(dir)/*.cpp))
TSTS		:= $(filter %.test.cpp, $(TST_CPPS))
TST_SRCS	:= $(filter-out %.test.cpp, $(TST_CPPS)) $(filter-out %Main.cpp, $(SRCS))
TST_DEPS	:= $(TST_SRCS:$(TST_DIR)/%.cpp=$(TST_OBJ_DIR)/%.d)
TST_OBJS	:= $(TST_SRCS:$(TST_DIR)/%.cpp=$(TST_OBJ_DIR)/%.o)
TST_EXES	:= $(TSTS:$(TST_DIR)/%.test.cpp=$(TST_DIR)/%$(EXT))

################################################################################

APPNAME	= prog
EXE_DBG	= $(APPNAME)$(EXT)

################################################################################
	
all: debug

pre-debug:
	@$(call make_dir, $(SRC_SB_DIRS:$(SRC_DIR)/%="%") )
	$(info )
	$(info Compiling debug 64-bit ...)
debug: CXX_FLAGS += $(DBG_FLAGS) $(LIB_PATHS)
debug: pre-debug $(EXE_DBG)
	$(info Debug compiled to "$(EXE_DBG)".)
	
pre-tests:
	$(info )
	$(info Compiling tests ...)
tests: CXX_FLAGS += $(TST_FLAGS) $(LIB_PATHS)
tests: clean-tests pre-tests $(TST_EXES)
	$(info )
	$(info Test compilation complete.)
	
clean-objs:
	$(RM) *.o *.d
clean-all: clean-objs
	$(RM) *$(EXT)
	
clean-tests:
	$(info )
	$(info Cleaning test environment ...)
	$(RM) $(TST_DIR)\*$(EXT)

################################################################################

$(EXE_DBG): $(DBG_OBJS)
	$(info )
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LINKER_FLAGS)

-include $(DBG_DEPS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(info $< -> $@)
	@$(CXX) $(CXX_FLAGS) -MMD -MP -o $@ -c $<
	
$(TST_DIR)/%.exe: $(TST_DIR)/%.test.cpp
	$(info Compiling $(patsubst $(TST_DIR)/%.test.cpp,%.test.cpp, $<))
	@$(CXX) $(CXX_FLAGS) -o $@ $< $(TST_SRCS) $(LINKER_FLAGS)
	
################################################################################