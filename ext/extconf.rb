makefile = <<-MAKEFILE
all: prepare build

build:
	gcc -Wall recommendify.c -lhiredis -o ../bin/recommendify

prepare:
	mkdir -p ../bin

clean:
	rm -f *.o

install: prepare
MAKEFILE
  
File.open(::File.expand_path("../Makefile", __FILE__), "w+") do |f|   
  f.write(makefile)
end