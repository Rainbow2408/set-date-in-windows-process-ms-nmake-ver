##############################################################################
##
##  Makefile for Detours Programs.
##
##  Note:
##    Build set-date-in-windows-process
##

ROOT=..
!include .\common.mak

##############################################################################

all:
    cd "$(MAKEDIR)\setDate"
    @$(MAKE) /NOLOGO /$(MAKEFLAGS)

clean:
    cd "$(MAKEDIR)\setDate"
    @$(MAKE) /NOLOGO /$(MAKEFLAGS) clean
    -rmdir lib32 2>nul
    -rmdir lib64 2>nul
    -rmdir include 2>nul

realclean:
    cd "$(MAKEDIR)\setDate"
    @$(MAKE) /NOLOGO /$(MAKEFLAGS) realclean
    -rmdir lib32 2>nul
    -rmdir lib64 2>nul
    -rmdir include 2>nul

test:
    cd "$(MAKEDIR)\setDate"
    @$(MAKE) /NOLOGO /$(MAKEFLAGS) test

##
################################################################# End of File.
