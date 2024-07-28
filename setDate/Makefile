##############################################################################
##
##  Makefile for Detours Programs.
##

!include ..\common.mak

LIBS=$(LIBS) kernel32.lib
CFLAGS=$(CFLAGS) /EHsc

all: dirs \
    $(BIND)\setDate.dll \
    $(BIND)\setDate.exe

clean:
    -del *~ *.obj *.sbr 2> nul
    -del $(BIND)\setDate.* $(BIND)\setDate*.* 2> nul
    -rmdir /q /s $(OBJD) 2>nul
	
realclean: clean
    -rmdir /q /s $(OBJDS) 2>nul
	
dirs:
    @if not exist $(BIND) mkdir $(BIND) && echo.   Created $(BIND)
    @if not exist $(OBJD) mkdir $(OBJD) && echo.   Created $(OBJD)

$(OBJD)\hooks.obj : hooks.cpp setDate.h
    cl $(CFLAGS) /c /Fo$(OBJD)\hooks.obj hooks.cpp

$(OBJD)\setDate.obj : setDate.cpp setDate.h
    cl $(CFLAGS) /c /Fo$(OBJD)\setDate.obj setDate.cpp

$(BIND)\setDate.dll $(BIND)\setDate.lib: \
        $(OBJD)\hooks.obj $(DEPS)
    cl /LD $(CFLAGS) /Fe$(@R).dll /Fd$(@R).pdb \
        $(OBJD)\hooks.obj \
        /link $(LINKFLAGS) /DLL /subsystem:console \
        /export:GetSystemTime_hook \
        /export:GetLocalTime_hook \
        $(LIBS)

$(BIND)\setDate.exe : $(OBJD)\setDate.obj $(BIND)\setDate.lib $(DEPS)
    cl $(CFLAGS) /Fe$@ /Fd$(@R).pdb $(OBJD)\setDate.obj \
        /link $(LINKFLAGS) $(LIBS) \
        /subsystem:console /fixed:no $(BIND)\setDate.lib

# test-target.exe: test-target.o
# 	gcc target.o -o test-target.exe
