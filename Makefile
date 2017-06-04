install:
	mkdir src/build; cp src/*.dat src/build; cd src/build; cmake -D CUDA_USE_STATIC_CUDA_RUNTIME=OFF ..; make;cd ../../
clean:
	rm -rf src/build
