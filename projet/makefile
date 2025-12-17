CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = bin/program

SRCS := $(shell find . -name '*.c' -type f | sed 's|^\./||')
OBJS := $(patsubst %.c,bin/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

define make-compile-rule
$(1): $(2)
	@mkdir -p $$(dir $(1))
	$(CC) $(CFLAGS) -c $(2) -o $(1)
endef

$(foreach src,$(SRCS),$(eval $(call make-compile-rule,bin/$(src:.c=.o),$(src))))

clean:
	rm -rf bin

rebuild: clean all

run: $(TARGET)
	./$(TARGET)

doc:
	doxygen Doxyfile

clean-doc:
	rm -rf doc

.PHONY: all clean rebuild run doc clean-doc