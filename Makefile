NAME=fgielow/docker-ns3-ddfc
IP = $(shell ifconfig en0 | grep inet | awk '$$1=="inet" {print $$2}')

run-xhost: setup-xhost
	docker container run --rm -it -v `pwd`/ddfc-source/src/firefly_dynamic_clustering:/workspace/bake/source/ns-3.14.1/src/firefly_dynamic_clustering -e DISPLAY=$(IP):0 -v /tmp/.X11-unix:/tmp/.X11-unix $(NAME)

setup-xhost:
	xhost + $(IP)
	xhost + 127.0.0.1

build: Dockerfile
	tar cvjf ddfc-source.tar.bz2 ./ddfc-source/
	docker build -t $(NAME) .
	rm ddfc-source.tar.bz2