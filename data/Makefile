all:
	echo "hello"

fastestspeedrun:
	wget -O fastestspeedrun.zip https://owncloud.hpi.de/s/tGKfRrmTUejNhRH/download
	unzip fastestspeedrun.zip
	rm fastestspeedrun.zip
	./fastestspeedrun_convert.py

antilemon:
	./antilemon.py

yosupo:
	mkdir -p yosupo
	../build/yosupo_generator 0 1 > ./yosupo/random_00.wsoap
	../build/yosupo_generator 1 1 > ./yosupo/random_01.wsoap
	../build/yosupo_generator 2 1 > ./yosupo/random_02.wsoap
	../build/yosupo_generator 3 1 > ./yosupo/random_03.wsoap
	../build/yosupo_generator 4 1 > ./yosupo/random_04.wsoap
	../build/yosupo_generator 0 2 > ./yosupo/max_random_00.wsoap
	../build/yosupo_generator 1 2 > ./yosupo/max_random_01.wsoap
	../build/yosupo_generator 2 2 > ./yosupo/max_random_02.wsoap
	../build/yosupo_generator 3 2 > ./yosupo/max_random_03.wsoap
	../build/yosupo_generator 4 2 > ./yosupo/max_random_04.wsoap

konect:
	mkdir -p konect
	./konect_download_directed.py
	./konect_extrac_convert.py

networkrepository:
	mkdir -p networkrepository
	./networkrepository_download_networks.sh
	./networkrepository_filter_convert.py
	# they have floating point weights
	rm networkrepository/advogato.wsoap
	rm networkrepository/soc-advogato.wsoap
	rm networkrepository/eco-foodweb-baydry.wsoap
	rm networkrepository/foodweb-baydry.wsoap
	rm networkrepository/foodweb-baywet.wsoap

girgs:
	./girgs_generate.py
	./girgs_convert.py

