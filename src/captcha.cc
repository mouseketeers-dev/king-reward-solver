#include "captcha.h"

Ptr<ml::ANN_MLP> mlp;
string characters;
string opencv_ver;

namespace Captcha {

  string initialize(const string& model_input) {
    if (has_initialized()) return characters;

    FileStorage fs(model_input, FileStorage::READ);
    if (!fs.isOpened()) throw runtime_error(UNABLE_TO_OPEN_DATA_ERROR);

    mlp = ml::ANN_MLP::load(model_input);

    fs["characters"] >> characters;
    fs.release();

    if (mlp.empty()) throw runtime_error(UNABLE_TO_LOAD_DATA_ERROR);
    if (characters.empty()) throw runtime_error(INVALID_CHARACTERS);

    return characters;
  }

  bool has_initialized() {
    return !mlp.empty();
  }

  string solve_captcha(const Mat& src) {
    if (src.empty()) throw runtime_error(UNABLE_TO_READ_IMAGE_ERROR);

    auto preprocessed = Solver::preprocess(src);
    auto segments = Solver::segment(preprocessed);

    Mat input;

    // generate features from each segment
    for (const auto& segment : segments) {
      if (segment.span == 1) {
        input.push_back(Solver::generateFeatures(segment.data));
      }
    }

    Mat output;
    mlp->predict(input, output);

    stringstream ss;
    auto letter_index = 0;

    for (const auto& segment : segments) {
      if (segment.span == 1) {
        auto predictions = output.row(letter_index++);
        Point max_location;
        minMaxLoc(predictions, nullptr, nullptr, nullptr, &max_location);
        ss << characters[max_location.x];
      }
      else {
        ss << string(segment.span, '?');
      }
    }

    return ss.str();
  }
  
  string solve_captcha_image(const string& filename) {
    return solve_captcha(imread(filename));
  }

  string solve_captcha_buffer(const char* buffer, const size_t& buffer_size) {
    Mat rawData(1, buffer_size, CV_8UC1, (void*)buffer);
    return solve_captcha(imdecode(rawData, IMREAD_COLOR));
  }

  string get_opencv_ver() {
    if (opencv_ver.empty()) {
      stringstream ss;
      ss << CV_VERSION_MAJOR << "." << CV_VERSION_MINOR << "." << CV_SUBMINOR_VERSION;
      opencv_ver = ss.str();
    }

    return opencv_ver;
  }
}

namespace Solver {

  Mat preprocess(const Mat& src) {
    Mat img;

    cvtColor(src, img, COLOR_BGR2GRAY);
    inRange(img, 0, 200, img);

    auto crop_region = Rect(0, 0, 160, src.rows);
    img = img(crop_region).clone();

    // remove lines
    auto openKernel = getStructuringElement(MORPH_RECT, Size(1, 3));
    morphologyEx(img, img, MORPH_OPEN, openKernel);

    // remove blobs of area < 20
    auto temp = Mat(img);
    vector<vector<Point>> contours;
    findContours(temp, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours) {
      if (contourArea(contour) < 20) {
        auto rect = boundingRect(Mat(contour));
        img(rect) = 0;
      }
    }

    // close vertical gaps
    auto closeKernel = getStructuringElement(MORPH_RECT, Size(2, 1));
    morphologyEx(img, img, MORPH_CLOSE, closeKernel);

    return img;
  }

  vector<Segment> segment(const Mat& src) {
    auto img_contours(src);
    vector<vector<Point>> contours;
    findContours(img_contours, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    vector<Rect> rects;
    for (const auto& contour : contours) rects.push_back(boundingRect(contour));

    // sort horizontally
    sort(rects.begin(), rects.end(), [](const auto& a, const auto& b) { return a.x < b.x; });

    vector<Segment> segments;
    for (const auto& rect : rects) {
      if (rect.area() >= 20) { // only process countours with area >= 20
        auto span = rect.width <= 35 ? 1 : 2;
        segments.push_back({ src(rect), span });
      }
    }

    return segments;
  }

  Mat normalizeRange(const Mat& src) {
    double max;
    minMaxLoc(src, nullptr, &max);

    Mat ret;

    if (max > 0) src.convertTo(ret, -1, 1.0f / max);
    return ret;
  }

  Mat resample(const Mat& src, const int& size) {
    Mat resampled;
    resize(src, resampled, Size(size, size), 0, 0, INTER_NEAREST);
    return normalizeRange(resampled);
  }

  Mat generateFeatures(const Mat& segment) {
    auto features = resample(segment, 15);

    features.convertTo(features, CV_32FC1);
    features = features.reshape(0, 1);

    return features;
  }
}
