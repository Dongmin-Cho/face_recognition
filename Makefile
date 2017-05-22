install:
	mkdir src/build; cp src/*.dat src/build; cd src/build; cmake ..; make;cd ../../
clean:
	rm -rf src/build
