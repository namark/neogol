
override CPPFLAGS	+= --std=c++1z
override CPPFLAGS	+= -MMD -MP
override CPPFLAGS	+= $(shell cat .cxxflags | xargs)
override CPPFLAGS	+= -I./include -I./source
override LDFLAGS	+= $(shell cat .ldflags | xargs)
override LDFLAGS	+= -L./lib
override LDLIBS		+= -lSDL2main -lSDL2 -lpthread

ifdef ENABLE_THEORA_RECORDER
override CPPFLAGS	+= -DENABLE_THEORA_RECORDER
override LDLIBS		+= -ltheoraenc -ltheoradec -logg
endif

PREFIX	:= $(DESTDIR)/usr/local
BINDIR	:= $(PREFIX)/bin

NAME := neogol

SRCDIR	:= ./source
TEMPDIR	:= temp
DISTDIR	:= out
SOURCES	:= $(shell find -wholename "$(SRCDIR)/*.cpp")
HEADERS	:= $(shell find -wholename "$(SRCDIR)/*.hpp" && find -wholename "$(SRCDIR)/*.h")
OUT		:= $(DISTDIR)/$(NAME)
TARGET	:= $(OUT:$(DISTDIR)/%=$(BINDIR)/%)
OBJECTS	:= $(SOURCES:%.cpp=$(TEMPDIR)/%.o)
OBJDIRS := $(shell dirname $(OBJECTS))
LOCALIB	:= $(wildcard lib/*.a)
DEPENDS	:= $(OBJECTS:.o=.d)

INSTALL_SRCDIR	:= $(PREFIX)/source
INSTALL_SOURCES	:= $(SOURCES:$(SRCDIR)/%=$(INSTALL_SRCDIR)/%) $(HEADERS:$(SRCDIR)/%=$(INSTALL_SRCDIR)/%)
INSTALL_SRCDIRS	:= $(shell dirname $(INSTALL_SOURCES))

build: $(OUT)

$(OUT): $(OBJECTS) $(LOCALIB) | $(DISTDIR)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LOCALIB) $(LDLIBS) -o $@

$(TEMPDIR)/%.o: %.cpp | $(TEMPDIR)
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(TEMPDIR):
	@mkdir -p $@

$(DISTDIR):
	@mkdir -p $@

clean:
	@rm $(DEPENDS) 2> /dev/null || true
	@rm $(OBJECTS) 2> /dev/null || true
	@rmdir -p $(OBJDIRS) 2> /dev/null || true
	@rmdir -p $(TEMPDIR) 2> /dev/null || true
	@echo Temporaries cleaned!

distclean: clean
	@rm $(OUT) 2> /dev/null || true
	@rmdir -p $(DISTDIR) 2> /dev/null || true
	@echo All clean!

install: $(TARGET)

$(BINDIR)/%: $(DISTDIR)/% | $(BINDIR)
	install --strip $< $@

$(BINDIR):
	@mkdir $@

uninstall:
	-rm $(TARGET)
	@rmdir -p $(BINDIR) 2> /dev/null || true
	@echo Uninstall complete!

install_source: $(INSTALL_SOURCES)

$(INSTALL_SRCDIR)/%: $(SRCDIR)/% ./COPYRIGHT
	@mkdir -p $(@D)
	cat ./COPYRIGHT > $@
	cat $< >> $@


uninstall_source:
	-rm $(INSTALL_SOURCES)
	@rmdir -p $(INSTALL_SRCDIRS) 2> /dev/null || true
	@echo Source code uninstalled!

-include $(DEPENDS)

.PRECIOUS : $(OBJECTS)
.PHONY : clean distclean uninstall uninstall_source
