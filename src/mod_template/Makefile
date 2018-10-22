OUTPUT = bin/example.dll

COMMON_PATH = ../common

CFLAGS = -O3 -static -Wall -Wextra -std=c99 -fno-ident -MMD -MP -MF $@.d
CXXFLAGS = -O3 -static -Wall -Wextra -std=c++98 -fno-ident -MMD -MP -MF $@.d
ALL_CFLAGS = -I$(COMMON_PATH) $(CFLAGS)
ALL_CXXFLAGS = -I$(COMMON_PATH) $(CXXFLAGS)

SOURCES = \
	common/mod_loader \
	main
OBJECTS = $(addprefix obj/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addsuffix .d, $(OBJECTS))

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@ -shared
	@strip $@ --strip-unneeded

obj/%.o: src/%.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) $< -o $@ -c

obj/common/%.o: $(COMMON_PATH)/%.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) $< -o $@ -c

clean:
	@rm -rf obj bin

include $(wildcard $(DEPENDENCIES))
