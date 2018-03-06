NAME=fgielow/ns3-ddfc
IP = $(shell hostname)
USER = $(shell whoami)

run-xhost: setup-xhost
	docker container run --rm -it -v `pwd`/ddfc-source/src/firefly_dynamic_clustering:/workspace/bake/source/ns-3.14.1/src/firefly_dynamic_clustering -e DISPLAY=$(IP):0 -v /tmp/.X11-unix:/tmp/.X11-unix $(NAME)

run:
	docker container run --rm -it -v `pwd`/ddfc-source/src/firefly_dynamic_clustering:/workspace/bake/source/ns-3.14.1/src/firefly_dynamic_clustering $(NAME)

setup-xhost:
	xhost + $(IP)
	xhost + 127.0.0.1
	xhost +SI:localuser:root
	xhost +SI:localuser:$(USER)

build: Dockerfile ddfc-source.tar.bz2
	docker build -t $(NAME) .

ddfc-source.tar.bz2:
	tar cvjf ./container-dependencies/ddfc-source.tar.bz2 ./ddfc-source/
