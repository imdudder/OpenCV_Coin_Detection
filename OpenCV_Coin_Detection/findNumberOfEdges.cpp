#include "opencv2/imgcodecs.hpp"

/* findNumberOfEdges
 * Precondition: edgeImage must be a single channel, binary image with edges a value of 255 and non-edges as 0.
 * Postcondition: The number of edge pixels are counted and the sum is returned as an int.
 */
int findNumberOfEdges(cv::Mat &edgeImage) {
    int numEdges = 0;
    for (int r = 0; r < edgeImage.rows; r++) {
        for (int c = 0; c < edgeImage.cols; c++) {
            uchar currentPixel = edgeImage.at<uchar>(r, c);
            if (currentPixel > 0) {
                numEdges++;
            }
        }
    }
    return numEdges;
}