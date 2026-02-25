all:
	make -f Makefile_lib
	make -f Makefile_app

clean:
	make -f Makefile_lib clean
	make -f Makefile_app clean