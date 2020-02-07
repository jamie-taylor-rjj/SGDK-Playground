# SGDK-PLayground

This is a project based on the SGDK project template. This template, in turn, is built to go along with my [SGDK tutorials](http://www.ohsat.com/tutorial/megapong/megapong-1/).

## Compiling

There are `.vscode` files set up in this directory which _SHOULD_ build the code base for us (whenever Ctrl+Shift+B are used), these are currently bugged out. For the time being, running the following command will compile and build the ROM for us:

``` bash
%GDK_WIN%\bin\make -f %GDK_WIN%\makefile.gen
```