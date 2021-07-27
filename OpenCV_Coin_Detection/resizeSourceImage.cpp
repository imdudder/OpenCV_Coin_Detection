#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

/* resizeSourceImage
 * Precondition: None
 * Postcondition: If sourceImg row or col is greater than maxDim, The larger dimension of the sourceImg will be resized
 *                to the maxDim size and the smaller dimension will be scaled proportionally and the resulting image
 *                will be assigned to the outputImg.
 */
void resizeSourceImage(cv::Mat sourceImg, cv::Mat &outputImg, const unsigned int maxDim) {
    if (sourceImg.rows > maxDim || sourceImg.cols > maxDim) {
        double ratio;

        if (sourceImg.rows > sourceImg.cols) {
            ratio = static_cast<double>(maxDim) / sourceImg.rows;
        } else {
            ratio = static_cast<double>(maxDim) / sourceImg.cols;
        }

        int height = (int) std::round(sourceImg.rows * ratio);
        int width = (int) std::round(sourceImg.cols * ratio);

        cv::resize(sourceImg, outputImg, cv::Size(width, height), cv::INTER_AREA);
    }
}