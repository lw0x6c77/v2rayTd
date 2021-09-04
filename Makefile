CC = clang
CFLAGS = -g -Wall -O2 -std=gnu99

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJS = $(SOURCES:.c=.o)
TARGET = v2rayTd v2rayTctl

all: $(TARGET)

v2rayTd: cmdline.o signal.o system.o engine.o config.o unix_domain.o \
	daemonize.o log.o v2rayTd.o
	$(CC) $^ -o $@

v2rayTctl: log.o v2rayTctl.o
	$(CC) $^ -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

.PHONY: release
release:
	@rm -f release/*
	@mkdir -p ./release
	cp -f v2rayTd release/
	cp -f v2rayTctl release/
	cp -f v2rayTd.cfg release/
	cp -f tool.sh release/
	@echo "done"

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -rf ./release/
