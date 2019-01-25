CC=gcc
CFLAGS= -g -Wall -Werror -I $(INCDIR)
LDFLAGS = $(libgl) -lm

SRCDIR		:= src
INCDIR		:= include
BUILDDIR	:= objects
TARGETDIR	:= bin
DEPDIR		:= dep
SRCEXT		:= c
DEPEXT		:= d
OBJEXT		:= o

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	libgl = -lGL -lglfw
endif
ifeq ($(UNAME_S),Darwin) # macOS
	libgl = -framework OpenGL -I/usr/local/include -lglfw
endif

APP = $(TARGETDIR)/rubiks
all: $(APP)
csrc = $(wildcard $(SRCDIR)/*.$(SRCEXT) $(SRCDIR)/**/*.$(SRCEXT))
obj = $(csrc:.$(SRCEXT)=.$(OBJEXT))
dep = $(obj:.$(OBJEXT)=.$(DEPEXT)) # one dependency file for each source

-include $(dep)	# include all dep files in the Makefile

$(APP): $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

# rule to generate a dep file by using the C preprocessor
%.$(DEPEXT): %.$(SRCEXT)
	@mkdir -p bin
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.$(DEPEXT)=.$(OBJEXT)) > $@

.PHONY: clean
clean:
	rm -f $(obj) $(APP) $(dep)

.PHONY: cleandep
cleandep:
	rm -f $(dep)
