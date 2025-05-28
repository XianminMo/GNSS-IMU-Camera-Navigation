#pragma once

#include <opencv2/core.hpp>

namespace gici {

struct YoloDetection {
  cv::Rect_<float> bbox;
  std::string label;
  float score;
  int class_id;
  double timestamp;
};

} // namespace gici
