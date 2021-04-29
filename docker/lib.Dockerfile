ARG NODE_VERSION=16
ARG OS=buster

FROM node:${NODE_VERSION}-${OS}

RUN apt-get update \
    && apt-get -q install -y --no-install-recommends \ 
        build-essential \
        cmake \
        git \
        wget \
        unzip \
        libpng-dev \
        python3

WORKDIR /usr/king-reward-solver

COPY . .

ENV KING_REWARD_SOLVER_ENABLE_REBUILD=1
RUN yarn

ENV LD_LIBRARY_PATH=/usr/king-reward-solver/opencv-prebuilt/linux/lib:$LD_LIBRARY_PATH

CMD ["yarn", "test"]
# CMD ["sh"]
