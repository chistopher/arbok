all:
	echo "hello"

fastestspeedrun:
	wget -O fastestspeedrun.zip https://owncloud.hpi.de/s/tGKfRrmTUejNhRH/download
	unzip fastestspeedrun.zip
	rm fastestspeedrun.zip
	./fastestspeedrun_convert.py

antilemon:
	./antilemon.py

josupo:
	mkdir -p josupo
	../build/josupo_generator 0 1 > ./josupo/random_00.wsoap
	../build/josupo_generator 1 1 > ./josupo/random_01.wsoap
	../build/josupo_generator 2 1 > ./josupo/random_02.wsoap
	../build/josupo_generator 3 1 > ./josupo/random_03.wsoap
	../build/josupo_generator 4 1 > ./josupo/random_04.wsoap
	../build/josupo_generator 0 2 > ./josupo/max_random_00.wsoap
	../build/josupo_generator 1 2 > ./josupo/max_random_01.wsoap
	../build/josupo_generator 2 2 > ./josupo/max_random_02.wsoap
	../build/josupo_generator 3 2 > ./josupo/max_random_03.wsoap
	../build/josupo_generator 4 2 > ./josupo/max_random_04.wsoap

konect:
	./konect_download_directed.py
	./konect_extrac_convert.py

networkrepository:
	./networkrepository_download_networks.sh
	./networkrepository_filter_convert.py

