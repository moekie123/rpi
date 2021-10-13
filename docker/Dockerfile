FROM debian:stable

RUN apt-get update

RUN apt-get install gcc g++ make vim git git-flow libssl-dev -y 

# Install CMake
WORKDIR /opt/cmake
RUN git clone https://github.com/Kitware/CMake.git . 	\ 
		&& ./bootstrap 				\ 
		&& make -j4  				\
		&& make -j4 install

# Install CJson
WORKDIR /opt/cjson
RUN git clone https://github.com/DaveGamble/cJSON.git .	\
		&& cmake . 				\
		&& make -j4 				\
		&& make -j4 install

# Install GoogleTest
WORKDIR /opt/googletest
RUN git clone https://github.com/google/googletest.git . \
		&& cmake -DBUILD_SHARED_LIBS=ON .	 \
		&& make -j4 				 \
		&& make -j4 install

# Install RapidJson
WORKDIR /opt/rapidjson
RUN git clone https://github.com/Tencent/rapidjson.git .\
		&& cmake cmake -DRAPIDJSON_BUILD_CXX17=ON -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_HAS_STDSTRING=ON . \
		&& make -j4 				\
		&& make -j4 install

# Install Spdlog
WORKDIR /opt/spdlog
RUN git clone https://github.com/gabime/spdlog.git .	\
		&& cmake -DSPDLOG_BUILD_EXAMPLE=OFF -DSPDLOG_BUILD_SHARED=ON . \
		&& make -j4 				\
		&& make -j4 install

# Install Mosquitto
WORKDIR /opt/mosquitto
RUN git clone https://github.com/eclipse/mosquitto.git .\
		&& cmake -DDOCUMENTATION=OFF . \
		&& make -j4 				\
		&& make -j4 install

# Install Tinyfsm
WORKDIR /opt/tinyfsm
RUN git clone https://github.com/digint/tinyfsm.git .

WORKDIR /usr/local/include/tinyfsm
RUN cp /opt/tinyfsm/include/* .

# Update linker after installation of tools
WORKDIR /etc/ld.so.conf.d/
RUN echo '/usr/local/lib/' >> aarch64-linux-gnu.conf	\
		 && ldconfig

# Configure bash script at startup
WORKDIR /root
COPY .bashrc /root/.bashrc
CMD source /root/.bashrc
