FROM debian:stable

RUN apt-get update

RUN apt-get install gcc g++ make vim git git-flow libssl-dev -y 

WORKDIR /opt/cmake
RUN git clone https://github.com/Kitware/CMake.git . 	\ 
		&& ./bootstrap 				\ 
		&& make -j4  				\
		&& make -j4 install

WORKDIR /opt/cjson
RUN git clone https://github.com/DaveGamble/cJSON.git .	\
		&& cmake . 				\
		&& make -j4 				\
		&& make -j4 install

WORKDIR /opt/googletest
RUN git clone https://github.com/google/googletest.git . \
		&& cmake -DBUILD_SHARED_LIBS=ON .	 \
		&& make -j4 				 \
		&& make -j4 install

WORKDIR /opt/rapidjson
RUN git clone https://github.com/Tencent/rapidjson.git .\
		&& cmake cmake -DRAPIDJSON_BUILD_CXX17=ON -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_HAS_STDSTRING=ON . \
		&& make -j4 				\
		&& make -j4 install

WORKDIR /opt/spdlog
RUN git clone https://github.com/gabime/spdlog.git .	\
		&& cmake -DSPDLOG_BUILD_EXAMPLE=OFF -DSPDLOG_BUILD_SHARED=ONi . \
		&& make -j4 				\
		&& make -j4 install

WORKDIR /opt/mosquitto
RUN git clone https://github.com/eclipse/mosquitto.git .\
		&& cmake -DDOCUMENTATION=OFF . \
		&& make -j4 				\
		&& make -j4 install

COPY .bashrc /root/.bashrc
CMD source /root/.bashrc

