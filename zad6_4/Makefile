CC=gcc
CFLAGS=-Wall -g -Wno-deprecated-declarations

BUILD_SUF=-build

%: %.c
		$(CC) $(CFLAGS) $< -o $@$(BUILD_SUF) $(LDFLAGS)
