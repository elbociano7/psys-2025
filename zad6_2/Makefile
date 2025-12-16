CC=gcc
CFLAGS=-Wall -g

BUILD_SUF=-build

all: operacja

%: %.c
	$(CC) $(CFLAGS) $< -o $@$(BUILD_SUF) $(LDFLAGS)

