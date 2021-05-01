# docker buildx build --push --platform linux/amd64,linux/arm/v7 -t mouseketeers/king-reward-solver:1.0.1

ARG NODE_VERSION=16
ARG OS=buster
ARG BASE_IMAGE_TAG=node:${NODE_VERSION}-${OS}-slim

#### Stage BASE ######################################################################################################
FROM $BASE_IMAGE_TAG AS base
RUN apt-get update \ 
    && apt-get -q install -y --no-install-recommends \
       libpng-dev 

#### Stage OPENCV ######################################################################################################
FROM base AS opencv

ARG SRC_DIR=/tmp
ARG OPENCV_VERSION=4.5.1

# install build dependencies
RUN apt-get -q install -y --no-install-recommends \ 
    build-essential \
    ca-certificates \
    cmake \
    python3 \
    unzip \
    wget 

# download opencv source
RUN mkdir -p $SRC_DIR \
    && cd $SRC_DIR \
    && wget -q https://github.com/opencv/opencv/archive/$OPENCV_VERSION.zip \
    && unzip -q $OPENCV_VERSION.zip \
    && mv opencv-$OPENCV_VERSION opencv \
    && rm $OPENCV_VERSION.zip 

# build
RUN mkdir -p $SRC_DIR/opencv/build \
    && cd $SRC_DIR/opencv/build \
 	&& cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_CXX_FLAGS="-Wno-psabi" -DCMAKE_MESSAGE_LOG_LEVEL="WARNING"\
 	-DBUILD_LIST:STRING="ml,imgproc,imgcodecs" \
 	-DWITH_OPENCLAMDBLAS=OFF -DBUILD_JASPER=OFF -Dpackbits=OFF -DWITH_DIRECTX=OFF -DWITH_FFMPEG=OFF \
 	-Dthunder=OFF -DBUILD_opencv_js_bindings_generator=OFF -DWITH_PROTOBUF=OFF -DBUILD_opencv_dnn=OFF \
 	-DBUILD_ZLIB=OFF -DBUILD_opencv_java_bindings_generator=OFF -DWITH_OPENEXR=OFF -DBUILD_opencv_video=OFF \
 	-DWITH_IMGCODEC_SUNRASTER=OFF -DBUILD_opencv_photo=OFF -DWITH_JASPER=OFF -DWITH_GSTREAMER=OFF \
 	-DOPENCV_DNN_OPENCL=OFF -DBUILD_IPP_IW=OFF -DWITH_LAPACK=OFF -DBUILD_opencv_ts=OFF -DWITH_MSMF_DXVA=OFF \
 	-DWITH_OPENCL_D3D11_NV=OFF -DBUILD_PROTOBUF=OFF -Dmdi=OFF -DBUILD_opencv_python_bindings_generator=OFF \
 	-Dlzw=OFF -DWITH_QUIRC=OFF -DWITH_OPENJPEG=OFF -DBUILD_opencv_python_tests=OFF -DBUILD_opencv_features2d=OFF \
 	-DWITH_MSMF=OFF -Dccitt=OFF -DBUILD_OPENEXR=OFF -DBUILD_opencv_objdetect=OFF -DWITH_1394=OFF -DWITH_DSHOW=OFF \
 	-DWITH_ITT=OFF -DBUILD_opencv_apps=OFF -DWITH_WIN32UI=OFF -DWITH_IPP=OFF -Dlogluv=OFF -DWITH_IMGCODEC_PXM=OFF \
 	-DBUILD_opencv_objc_bindings_generator=OFF -DWITH_OPENCLAMDFFT=OFF -DBUILD_WEBP=OFF -DBUILD_PERF_TESTS=OFF \
 	-DBUILD_opencv_highgui=OFF -DWITH_IMGCODEC_PFM=OFF -DBUILD_JPEG=ON -DBUILD_OPENJPEG=OFF -Dnext=OFF \
 	-DBUILD_ITT=OFF -DBUILD_opencv_gapi=OFF -DBUILD_opencv_flann=OFF -DBUILD_JAVA=OFF -DWITH_WEBP=OFF \
 	-DWITH_JPEG=ON -DBUILD_opencv_stitching=OFF -DBUILD_TESTS=OFF -DWITH_V4L=OFF \
 	-DVIDEOIO_ENABLE_PLUGINS=OFF -DWITH_GTK=OFF \
 	-DWITH_TIFF=OFF -DWITH_OPENCL=OFF -DWITH_EIGEN=OFF -DBUILD_TIFF=OFF -DWITH_ADE=OFF -DWITH_VTK=OFF \
 	-DBUILD_opencv_calib3d=OFF -DBUILD_opencv_videoio=OFF -DWITH_IMGCODEC_HDR=OFF -DWITH_CUDA=OFF\
    .. \
    && make -j4 \
    && make install \
    && rm -rf ${SRC_DIR} 
    
#### Stage BUILD #######################################################################################################
FROM opencv AS build

WORKDIR /usr/king-reward-solver/

COPY . .

ENV KING_REWARD_SOLVER_ENABLE_REBUILD=1
RUN npm i

#### Stage RELEASE #####################################################################################################
FROM base AS RELEASE

RUN rm -rf /var/lib/apt/lists/* \
    && apt-get -qq autoremove \
    && apt-get -qq clean

ARG BUILD_DATE
ARG BUILD_VERSION
ARG BUILD_REF
ARG ARCH

LABEL org.label-schema.build-date=${BUILD_DATE} \
    org.label-schema.license="Apache-2.0" \
    org.label-schema.name="King Reward Solver" \
    org.label-schema.version=${BUILD_VERSION} \
    org.label-schema.description="Captcha solver for a certain mouse-related web game." \
    org.label-schema.vcs-ref=${BUILD_REF} \
    org.label-schema.vcs-type="Git" \
    org.label-schema.vcs-url="https://github.com/mouseketeers-dev/king-reward-solver" \
    org.label-schema.arch=${ARCH} \
    authors="kentnek"
   
WORKDIR /usr/king-reward-solver

COPY --from=build /usr/local/lib/libopencv* /usr/local/lib/
COPY --from=build /usr/king-reward-solver/ /usr/king-reward-solver/

RUN rm -r build node_modules src

ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

CMD ["yarn", "test"]
