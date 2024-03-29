FROM debian:stable

RUN apt-get update

RUN apt-get install gcc g++ gdb make			  \
			git git-flow 			 	          \
			vim clang-format exuberant-ctags 	  \
			doxygen graphviz			          \
			postgresql-client			          \
			libssl-dev 				              \
			sudo                                  \
            rsync                                 \
            bash-completion -y

# Install SSH
RUN apt install openssh-server -y
RUN service ssh start
EXPOSE 22

# Install CMake
WORKDIR /opt/cmake
ARG Version=v3.22.5
RUN git clone --depth 1 --branch $Version https://github.com/Kitware/CMake.git . 	\
		&& ./bootstrap 				                    \
		&& make -j4  				                    \
		&& make -j4 install

# Install CJson
WORKDIR /opt/cjson
RUN git clone https://github.com/DaveGamble/cJSON.git .	\
		&& cmake -DCMAKE_BUILD_TYPE=Debug . 	        \
		&& make -j4 				                    \
		&& make -j4 install

# Install GoogleTest
WORKDIR /opt/googletest
RUN git clone https://github.com/google/googletest.git .              \
		&& cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Debug .    \
		&& make -j4 				                                  \
		&& make -j4 install

# Install RapidJson
WORKDIR /opt/rapidjson
RUN git clone https://github.com/Tencent/rapidjson.git .                                                                            \
		&& cmake cmake -DRAPIDJSON_BUILD_CXX17=ON -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_HAS_STDSTRING=ON -DCMAKE_BUILD_TYPE=Debug . \
		&& make -j4 				                                                                                                \
		&& make -j4 install

# Install Spdlog
WORKDIR /opt/spdlog
RUN git clone https://github.com/gabime/spdlog.git .	                                        \
		&& cmake -DSPDLOG_BUILD_EXAMPLE=OFF -DCMAKE_BUILD_TYPE=Debug -DSPDLOG_BUILD_SHARED=ON . \
		&& make -j4 				                                                            \
		&& make -j4 install

# Install Mosquitto
WORKDIR /opt/mosquitto
RUN git clone https://github.com/eclipse/mosquitto.git .          \
		&& cmake -DDOCUMENTATION=OFF -DCMAKE_BUILD_TYPE=Debug .   \
		&& make -j4 				                              \
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

# Create User
ARG HOST_UID
ARG HOST_GID
ARG WHO

RUN groupadd ${WHO}
RUN groupmod -o -g ${HOST_GID} ${WHO}

RUN useradd -d /home/${WHO} -ms /bin/bash -p ${WHO} -g ${WHO} ${WHO}
RUN echo "${WHO}:${WHO}" | chpasswd
RUN usermod -u ${HOST_UID} ${WHO}
RUN usermod -g ${HOST_GID} ${WHO}
RUN usermod -a -G sudo ${WHO}

USER ${WHO}

# Add Environment Scripts
ARG HOME_DIR=/home/${WHO}
WORKDIR ${HOME_DIR}

COPY script/* 	${HOME_DIR}/
CMD source 	${HOME_DIR}/.bashrc

# Configure user specifics
RUN git config --global user.name "${WHO}"
RUN git config --global user.email "-private-"

USER root
CMD ["/usr/sbin/sshd","-D"]

