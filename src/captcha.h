#pragma once

#include <napi.h>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

#define UNABLE_TO_READ_IMAGE_ERROR "Unable to read image file."
#define UNABLE_TO_LOAD_DATA_ERROR "Unable to load model from file."
#define UNABLE_TO_OPEN_DATA_ERROR "Model file not found."
#define INVALID_CHARACTERS "Unable to retrieve characters from model file."

using namespace std;
using namespace cv;

namespace Captcha {
  string initialize(const string& model_input = "model.xml");
  bool has_initialized();
  string solve_captcha_image(const string& filename);
  string solve_captcha_buffer(const char* buffer, const size_t& buffer_size);
  string get_opencv_ver();
}

namespace Solver {
  struct Segment {
    Mat data;
    int span = 1;

    Segment() : span(0) {};
    Segment(const Mat& src, const int& span = 1) : data(src), span(span) {};
    explicit Segment(const Mat& src) : data(src) {}
  };

  Mat preprocess(const Mat& src);
  vector<Segment> segment(const Mat& src);
  Mat generateFeatures(const Mat& segment);
}
