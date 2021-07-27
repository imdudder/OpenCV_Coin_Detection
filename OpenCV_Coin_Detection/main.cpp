//--------------------------------------------------------------------------------------------------------------------
// Coin Detection
// Authors: Ian Dudder and Karran Singh
// CSS 487 A, Autumn 2020
//--------------------------------------------------------------------------------------------------------------------
// This program seeks to find and identify US coins within a given image. The coins must be pennies, nickels, dimes, or
// quarters and they must be standard front and back (eg. eagle back quarters and lincoln memorial backed pennies).
//
// Project objectives:
//      1. Recognize elliptical objects in an image.
//      2. Identify which elliptical objects are coins, and which are not.
//      3. Determine if the coin is heads-up, or tails-up.
//      4. Identify which type of coin was found (i.e. penny, nickel, dime, or quarter).
//      5. Report on the total value of the coins in the image.
//
// This program will read in all ".jpg" images which are contained in the "Test Images" directory local to the program.
// Any images you do not wish to run should be put into the "Additional Images" folder in the "Test Images" directory. 
// After the input images are processed, the corresponding output images will be saved to the "Output Images" directory
// local to the program and displayed to the screen.
//--------------------------------------------------------------------------------------------------------------------
// Project Pre-conditions:
//   -- Must be compiled using C++ 17 standard in order to use std::filesystem.
//
//   -- Must be compiled using x64 in debug mode, OpenCV must be installed and property sheet imported.
//
//   -- Three directories must exist local to the program. The first directory must be called "Test Images" from where 
//      the program will read all non-directory files with the extension ".jpg". The Second directory must be called
//      "Output Images". The third directory must be called "Template Images" where 8 images must exist serving as the
//      template coins images. In "Template Images" two images for each type of coin (Quarter, Dime, Nickle, Penny) 
//      must exist with one for heads and the other for tails.
//
//   -- Any images you wish to test are in the "Test Images" directory. All ".jpg" files in this directory will be ran.
//--------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <map>
#include <filesystem>
#include <thread>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

/* findNumberOfEdges
 * Precondition: edgeImage must be a single channel, binary image with edges a value of 255 and non-edges as 0.
 * Postcondition: The number of edge pixels are counted and the sum is returned as an int.
 */
int findNumberOfEdges(cv::Mat& edgeImage);

/* resizeSourceImage
 * Precondition: None
 * Postcondition: If sourceImg row or col is greater than maxDim, The larger dimension of the sourceImg will be resized
 *                to the maxDim size and the smaller dimension will be scaled proportionally and the resulting image
 *                will be assigned to the outputImg.
 */
void resizeSourceImage(cv::Mat sourceImg, cv::Mat &outputImg, const unsigned int maxDim);

/* findCoins
 * Precondition: A valid sourceImg is provided which has rows and cols greater than 0. A directory must be called
 *               "Template Images" where 8 images must exist serving as the template images (Two images for each type
 *               of coin (Quarter, Dime, Nickle, Penny) one for heads and one for tails). The 8 images must have the
 *               following names: dimeHeads.jpg, dimeTails.jpg, nickelHeads.jpg, nickelTails.jpg, pennyHeads.jpg,
 *               pennyTails.jpg, quarterHeads.jpg, quarterTails.jpg.
 * Postcondition: All the elliptical objects in the sourceImg will be enclosed in a red ellipse on the outputImg. The
 *                elliptical objects which are deemed to be coins will have a green bounding rectangle drawn on the
 *                outputImg along with text labeling what type of coin it is, whether its heads or tails up, and the
 *                percentage of closest match it was to the matched template. Lastly, the image will include the
 *                estimated value of the coins at the top of the output image.
 */
void findCoins(cv::Mat &sourceImg, cv::Mat &outputImg);

// global constants, local directories
const std::string inputDirectory{"Test Images/"};
const std::string outputDirectory{"Output Images/"};
const std::string templateDirectory{"Template Images/"};

/* main
 * Precondition: Three directories local to the program must exist. The first directory must be called "Test Images"
 *               from where the program will read all the non-directory files with the extension ".jpg". The second
 *               directory must be called "Output Images". The third directory must be called "Template Images" where 8
 *               images must exist serving as the template images. Two images for each type of coin (Quarter, Dime,
 *               Nickle, Penny) one for heads and one for tails.
 * Postcondition: The function will go through every file that is not a directory in the local "Test Images" directory
 *                and read all the ones which have a ".jpg" extension. Then it will call the findCoins function on each
 *                ".jpg" image which was read in. Once this has been done for all the images in the "Test Images"
 *                directory, both the source image and the corresponding processed output image showing the coins will
 *                be displayed to the screen one at a time. After all images are displayed, the outputs image will be
 *                saved to the local "Output Images" directory with the same name as the input image it was generated
 *                from with the text "_output" appended to it before the ".jpg" extension.
 * */
int main(int argc, char *argv[]) {

    // inputPath to .jpg file or dir containing .jpg file(s)
    std::string inputPath;

    // if no arguments are provided, default to inputDirectory
    if (argc == 1) {
        inputPath = inputDirectory;

    } else {    // else, inputPath is the first argument provided
        std::cout << "argv: " << argv[1] << std::endl;
        inputPath = argv[1];
    }
    std::cout << "The input Path is: " << inputPath << std::endl;

    std::map<std::string, cv::Mat> sourceImgs;  // map to store input images
    std::map<std::string, cv::Mat> outputImgs;  // map to store output images

    // if inputPath is to directory
    if (std::filesystem::is_directory(inputPath)) {
        // go through each entry in the inputPath directory
        for (const auto &entry : std::filesystem::directory_iterator(inputPath)) {

            // if the entry is a jpg file
            if (entry.path().extension() == ".jpg") {

                // trim '/' from name file name
                std::string imgName = entry.path().string();
                auto slashPos = static_cast<int>(imgName.find_last_of("/\\"));
                imgName = imgName.substr(slashPos + 1);

                // read image into sourceImgs map
                sourceImgs[imgName] = cv::imread(entry.path().string());
            }
        }

        // if inputPath is to file and file is a .jpg
    } else if (inputPath.find(".jpg") != std::string::npos) {

        std::string imgName = inputPath;
        // trim '/' from name
        auto slashPos = (int)imgName.find_last_of("/\\");
        imgName = imgName.substr(slashPos + 1);

        sourceImgs[imgName] = cv::imread(inputPath);

    } else {
        std::cout << "Please provide path to jpg image or dir containing jpg(s)" << std::endl;
        return -1;
    }


    // vector to store threads
    std::vector<std::thread> processingThreads;

    // for each input image in sourImgs map
    for (auto &img : sourceImgs) {

        // start findCoins method on a new thread
        std::thread t(findCoins, std::ref(img.second), std::ref(outputImgs[img.first]));

        // move thread to the threads vector
        processingThreads.push_back(std::move(t));
    }

    // wait for all the threads from processingThreads vector to finish
    for (std::thread &t : processingThreads) {
        if (t.joinable())
            t.join();
    }

    // names windows for the source and output images
    cv::namedWindow("sourceImg", cv::WINDOW_NORMAL);
    cv::namedWindow("outImg", cv::WINDOW_NORMAL);

    std::string outImgFileName;
    int nameExtention;

    // for ever image in outputImgs, write outputImg to local Output Images directory and
    // display outputImg and corresponding sourceImg
    for (auto &outImg : outputImgs) {

        cv::resizeWindow("sourceImg", outImg.second.cols / 2, outImg.second.rows / 2);
        cv::imshow("sourceImg", sourceImgs[outImg.first]);

        cv::resizeWindow("outImg", outImg.second.cols / 2, outImg.second.rows / 2);
        cv::imshow("outImg", outImg.second);

        outImgFileName = outImg.first;
        nameExtention = (int)outImgFileName.find(".jpg");
        outImgFileName.insert(nameExtention, "_output");
        imwrite(outputDirectory + outImgFileName, outImg.second);

        cv::waitKey();
    }
    return 0;
}

/* findCoins
 * Precondition: A valid sourceImg is provided which has rows and cols greater than 0. A directory must be called
 *               "Template Images" where 8 images must exist serving as the template images (Two images for each type
 *               of coin (Quarter, Dime, Nickle, Penny) one for heads and one for tails). The 8 images must have the
 *               following names: dimeHeads.jpg, dimeTails.jpg, nickelHeads.jpg, nickelTails.jpg, pennyHeads.jpg,
 *               pennyTails.jpg, quarterHeads.jpg, quarterTails.jpg.
 * Postcondition: All the elliptical objects in the sourceImg will be enclosed in a red ellipse on the outputImg. The
 *                elliptical objects which are deemed to be coins will have a green bounding rectangle drawn on the
 *                outputImg along with text labeling what type of coin it is, whether its heads or tails up, and the
 *                percentage of closest match it was to the matched template. Lastly, the image will include the
 *                estimated value of the coins at the top of the output image.
 */
void findCoins(cv::Mat &sourceImg, cv::Mat &outputImg) {

    // Constants
    const int cannyThreshold{25};
    const int cannyThreshold2 = cannyThreshold * 2;

    const double ellipseAreaThreshold{0.997};
    const int minAreaOfCircle{750};

    const int numOfTimeToBlur{6};
    const int blurKernelSize{5};

    const int numberOfTemplates{8};

    // resize sourceImg to have max dimention of 2500 pixels
    resizeSourceImage(sourceImg.clone(), sourceImg, 2500);

    // clone sourceImg to outputImg
    outputImg = sourceImg.clone();

    // Coin Templates
    int pennyHeads = 0, pennyTails = 1, nickelHeads = 2, nickelTails = 3,
            dimeHeads = 4, dimeTails = 5, quarterHeads = 6, quarterTails = 7;

    cv::Mat templateImage[numberOfTemplates]{};
    templateImage[pennyHeads] = cv::imread(templateDirectory + "pennyHeads.jpg");
    templateImage[pennyTails] = cv::imread(templateDirectory + "pennyTails.jpg");
    templateImage[nickelHeads] = cv::imread(templateDirectory + "nickelHeads.jpg");
    templateImage[nickelTails] = cv::imread(templateDirectory + "nickelTails.jpg");
    templateImage[dimeHeads] = cv::imread(templateDirectory + "dimeHeads.jpg");
    templateImage[dimeTails] = cv::imread(templateDirectory + "dimeTails.jpg");
    templateImage[quarterHeads] = cv::imread(templateDirectory + "quarterHeads.jpg");
    templateImage[quarterTails] = cv::imread(templateDirectory + "quarterTails.jpg");

    int coinTypeCount[numberOfTemplates] = {0};

    /*--------------------------------Step 1: BLUR_&_CANNY----------------------------------*/

    //1.1 - crease grayscale image from sourceImg
    cv::Mat sourceImgGray;
    cvtColor(sourceImg, sourceImgGray, cv::COLOR_BGR2GRAY);

    //1.2 - Blur the Image to reduce noise
    for (int i = 0; i < numOfTimeToBlur; i++) {
        cv::GaussianBlur(sourceImgGray, sourceImgGray, cv::Size(blurKernelSize, blurKernelSize), 2.0, 2.0);
    }

    //1.3 - Canny Edge detection
    cv::Mat sourceImgCanny;
    Canny(sourceImgGray, sourceImgCanny, cannyThreshold, cannyThreshold2);

    //1.4 - Dilate Canny Image
    cv::Mat dilationKernel = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3), cv::Point(1, 1));
    cv::dilate(sourceImgCanny, sourceImgCanny, dilationKernel);

    //1.5 - Find contours from edges
    std::vector<std::vector<cv::Point>> contours;
    findContours(sourceImgCanny, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    /*cv::namedWindow("Canny", cv::WINDOW_NORMAL);
    cv::resizeWindow("Canny", cv::Size(sourceImg.cols / 2, sourceImg.rows / 2));
    imshow("Canny", sourceImgCanny);
    cv::waitKey();*/

    // create image where contours will be filled solid with color
    cv::Mat contourFilledWithColorImg = sourceImg.clone();

    /*----------------------Step 2: Cycle through each Detected Contour---------------------*/
    for (int currentContour = 0; currentContour < contours.size(); currentContour++) {

        // If contour has less than 5 points or area is less than threshold, skip
        if (contours[currentContour].size() < 5 || (contourArea(contours[currentContour]) <= minAreaOfCircle)) {
            continue;
        }

        //2.1 - Create Shapes to Enclose the Contour
        //  Get Bounding Rectangle around Contour
        cv::Rect_<int> boundingRectVals = boundingRect(contours[currentContour]);
        cv::Point p1(boundingRectVals.x, boundingRectVals.y);
        cv::Point p2(boundingRectVals.x + boundingRectVals.width, boundingRectVals.y + boundingRectVals.height);

        //  Find enclosing ellipse of contour
        cv::RotatedRect ellipse = cv::fitEllipse(contours[currentContour]);
        double areaOfMinEnclosingEllipse = (ellipse.size.height / 2.0 * ellipse.size.width / 2.0 * CV_PI);
        double ellipseAreaRatio = contourArea(contours[currentContour]) / areaOfMinEnclosingEllipse;

        //  Skip contour if it does not fit well in an enclosing ellipse
        if (ellipseAreaRatio < ellipseAreaThreshold || ellipseAreaRatio > (1 - ellipseAreaThreshold) + 1) {
            continue;
        }

        // Draw red enclosing ellipse around contour on outputImg
        cv::ellipse(outputImg, ellipse, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);

        /*-------------------------Step 3: Get Patch Around the Contour------------------------*/

        // create patch image the same size as Bounding Rectangle around Contour
        cv::Mat patch(boundingRectVals.height, boundingRectVals.width, CV_8UC3, cv::Scalar(0, 0, 0));

        // fill contour with color cv::Scalar(111, 222, 121) on contourFilledWithColorImg to ignore background
        cv::ellipse(contourFilledWithColorImg, ellipse, cv::Scalar(111, 222, 121), cv::FILLED, cv::LINE_AA);

        // create filled patch image used to ignore background when extracting coin from image
        cv::Mat patchFilled = patch.clone();

        // Extract object found by the contour and store it in a patch
        for (int sourceRow = boundingRectVals.y, patchRow = 0; patchRow < patch.rows; sourceRow++, patchRow++) {
            for (int sourceCol = boundingRectVals.x, patchCol = 0; patchCol < patch.cols; sourceCol++, patchCol++) {
                patchFilled.at<cv::Vec3b>(patchRow, patchCol) = contourFilledWithColorImg.at<cv::Vec3b>(sourceRow,
                                                                                                        sourceCol);

                //Only take area enclosed by the ellipse, not the background by comparing with patchFilled 
                if (patchFilled.at<cv::Vec3b>(patchRow, patchCol)[0] == 111 &&
                    patchFilled.at<cv::Vec3b>(patchRow, patchCol)[1] == 222 &&
                    patchFilled.at<cv::Vec3b>(patchRow, patchCol)[2] == 121) {

                    patch.at<cv::Vec3b>(patchRow, patchCol) = sourceImg.at<cv::Vec3b>(sourceRow, sourceCol);
                }
            }
        }
        /*cv::namedWindow("Patch", cv::WINDOW_NORMAL);
        cv::resizeWindow("Patch", patch.cols * 2, patch.rows * 2);
        imshow("Patch", patch);
        cv::imwrite("patch.jpg", patch);
        cv::waitKey();*/


        /*---------------------------Step 4: Compare to Template Coins-------------------------*/
        // 4.1 - Create edge image of patch
        cv::Mat patchEdges;
        cv::cvtColor(patch, patchEdges, cv::COLOR_BGR2GRAY);
        GaussianBlur(patchEdges, patchEdges, cv::Size(3, 3), 2.0, 2.0);
        GaussianBlur(patchEdges, patchEdges, cv::Size(3, 3), 2.0, 2.0);
        GaussianBlur(patchEdges, patchEdges, cv::Size(3, 3), 2.0, 2.0);
        Canny(patchEdges, patchEdges, 20, 40);

        /*cv::namedWindow("patchedges", cv::WINDOW_NORMAL);
        cv::resizeWindow("patchedges", patchEdges.rows * 2, patchEdges.cols * 2);
        imshow("patchedges", patchEdges);
        cv::imwrite("patchEdges.jpg", patchEdges);
        cv::waitKey();*/

        // 4.2 - Calculate ratio of edges to non-edges for the patch
        int numPatchEdges = findNumberOfEdges(patchEdges);
        double percentEdges = ((double) numPatchEdges / ((double) patchEdges.rows * (double) patchEdges.cols)) * 100;

        // 4.3 - Loop through the template array to compare each template to the current patch
        double coinMatchPercent[numberOfTemplates] = {0.0}; //Percentage of edges that match between patch and template
        for (int currentCoin = 0; currentCoin < numberOfTemplates; currentCoin++) {

            //  4.3.1 - Resize template coin to match scale of patch
            cv::Mat resizedCoin;
            double ratio = (double)patch.cols / templateImage[currentCoin].cols;
            int width = patch.cols;
            int height = patch.rows;

            cv::Size_<int> dim = cv::Size(width, height);
            cv::resize(templateImage[currentCoin], resizedCoin, dim, ratio, ratio, cv::INTER_AREA);

            //  4.3.2 - Create an edge image for the current template coin
            cv::Mat templateEdges;
            cv::cvtColor(resizedCoin, templateEdges, cv::COLOR_BGR2GRAY);
            GaussianBlur(templateEdges, templateEdges, cv::Size(3, 3), 2.0, 2.0);
            GaussianBlur(templateEdges, templateEdges, cv::Size(3, 3), 2.0, 2.0);
            GaussianBlur(templateEdges, templateEdges, cv::Size(3, 3), 2.0, 2.0);
            Canny(templateEdges, templateEdges, 20, 40);
            int numTemplateEdges = findNumberOfEdges(templateEdges);

            //  4.3.3 - Try every rotation of template image and count the number of matching edges
            const int degreeIncrement = 5;
            const int matchCountSize = 360 / degreeIncrement;
            int rotationMatchCount[matchCountSize] = {0};
            cv::Mat rotatedTemplate;

            for (int degrees = 0, countIndex = 0;
                 degrees < 360 && countIndex < matchCountSize; degrees += degreeIncrement, countIndex++) {
                cv::Mat rotationMat = cv::getRotationMatrix2D(cv::Point(templateEdges.cols / 2,
                                                                        templateEdges.rows / 2), degrees, 1.0);

                cv::warpAffine(templateEdges, rotatedTemplate, rotationMat, templateEdges.size());

                // Check for match between the patch and the current rotation of the template
                for (int r = 0; r < rotatedTemplate.rows; r++) {
                    for (int c = 0; c < rotatedTemplate.cols; c++) {
                        //If there is an edge in the template image, check for a match in the patch
                        if (rotatedTemplate.at<uchar>(r, c) > 0) {
                            //If corresponding pixel in the patch is also an edge, increment the count
                            if (patchEdges.at<uchar>(r, c) > 0) {
                                rotationMatchCount[countIndex]++;
                            }
                        }
                    }
                }

                //Find the percentage of matching edges for this rotation
                double rotationMatchPercent =
                        ((double) rotationMatchCount[countIndex] / (double) numTemplateEdges) * 100.0;

                if (rotationMatchPercent > coinMatchPercent[currentCoin]) {
                    //Save the highest percentage of matches
                    coinMatchPercent[currentCoin] = rotationMatchPercent;
                }
            }
        }

        /*-----------------------Step 5: Determine which template gave the best match----------------------*/
        //Find which coin type and orientation (heads/tails) gave the best match count
        double percentMatchingEdges = 0.0;
        int bestTemplateMatch = 0;
        for (int voteIndex = 0; voteIndex < numberOfTemplates; voteIndex++) {
            if (coinMatchPercent[voteIndex] > percentMatchingEdges) {
                percentMatchingEdges = coinMatchPercent[voteIndex];
                bestTemplateMatch = voteIndex;
            }
        }

        // if more than 38% of the edges match, then its a coin
        if (percentMatchingEdges > 38.0) {

            /*-------------------Step 6: Determine which type of Coin it is-------------------------*/
            std::cout << "Patch Closest to ";
            std::string coinName = "Coin";
            switch (bestTemplateMatch) {
                case 0:
                    coinName = "Penny (Heads Up)";
                    coinTypeCount[pennyHeads]++;
                    break;
                case 1:
                    coinName = "Penny (Tails Up)";
                    coinTypeCount[pennyTails]++;
                    break;
                case 2:
                    coinName = "Nickel (Heads Up)";
                    coinTypeCount[nickelHeads]++;
                    break;
                case 3:
                    coinName = "Nickel (Tails Up)";
                    coinTypeCount[nickelTails]++;
                    break;
                case 4:
                    coinName = "Dime (Heads Up)";
                    coinTypeCount[dimeHeads]++;
                    break;
                case 5:
                    coinName = "Dime (Tails Up)";
                    coinTypeCount[dimeTails]++;
                    break;
                case 6:
                    coinName = "Quarter (Heads Up)";
                    coinTypeCount[quarterHeads]++;
                    break;
                case 7:
                    coinName = "Quarter (Tails Up)";
                    coinTypeCount[quarterTails]++;
                    break;
            }
            std::cout << coinName << " with " << percentMatchingEdges << "% matching edges" << std::endl;
            /*cv::namedWindow("Patch", cv::WINDOW_NORMAL);
            cv::resizeWindow("Patch", patch.cols * 2, patch.rows * 2);
            imshow("Patch", patch);
            cv::waitKey();*/


            /*----------------Step 7: Draw Identifying Shapes and Annotate Findings----------------*/
            // Draw enclosing rectangle
            cv::rectangle(outputImg, p1, p2, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);

            // Write Annotation for Object Identification
            cv::Point textOrigin(p1.x, p2.y + 30);
            cv::putText(outputImg, coinName, textOrigin, cv::FONT_HERSHEY_SIMPLEX, 1.0,
                        cv::Scalar(0, 0, 0), 4);
            cv::putText(outputImg, coinName, textOrigin, cv::FONT_HERSHEY_SIMPLEX, 1.0,
                        cv::Scalar(255, 255, 255), 2);

            std::stringstream ss;
            ss << percentMatchingEdges << "% Match";
            std::string percentMatch = ss.str();

            cv::Point matchTextOrigin(p1.x, p2.y + 60);
            cv::putText(outputImg, percentMatch, matchTextOrigin, cv::FONT_HERSHEY_SIMPLEX, 1.0,
                        cv::Scalar(0, 0, 0), 4);

            cv::putText(outputImg, percentMatch, matchTextOrigin, cv::FONT_HERSHEY_SIMPLEX, 1.0,
                        cv::Scalar(255, 255, 255), 2);
        }
    }

    // Determine value of the collection
    double valueOfCollection = 0.0;
    valueOfCollection += (coinTypeCount[pennyHeads] * 0.01);
    valueOfCollection += (coinTypeCount[pennyTails] * 0.01);
    valueOfCollection += (coinTypeCount[nickelHeads] * 0.05);
    valueOfCollection += (coinTypeCount[nickelTails] * 0.05);
    valueOfCollection += (coinTypeCount[dimeHeads] * 0.10);
    valueOfCollection += (coinTypeCount[dimeTails] * 0.10);
    valueOfCollection += (coinTypeCount[quarterHeads] * 0.25);
    valueOfCollection += (coinTypeCount[quarterTails] * 0.25);

    std::stringstream ss;
    ss << "Total Value of Collection: $" << std::setprecision(2) << std::fixed << valueOfCollection;
    std::string collectionValue = ss.str();

    cv::Point textOrigin(30, 60);
    cv::putText(outputImg, collectionValue, textOrigin, cv::FONT_HERSHEY_SIMPLEX, 2.0,
                cv::Scalar(0, 0, 0), 6);
    cv::putText(outputImg, collectionValue, textOrigin, cv::FONT_HERSHEY_SIMPLEX, 2.0,
                cv::Scalar(100, 255, 0), 2);
}