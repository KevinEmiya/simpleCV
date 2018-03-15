#include "PatternDetector.h"

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "util/QCvDataUtils.h"
#include <QDebug>

PatternDetector::PatternDetector(QObject* parent) : QObject(parent)
{
    m_detector = Ptr<FeatureDetector>(xfeatures2d::SURF::create());
    m_matcher = Ptr<DescriptorMatcher>(new FlannBasedMatcher());
    m_patternTracker = new PatternTracker(this);
}

PatternDetector::~PatternDetector()
{
}

void PatternDetector::train(const Mat& img, Mat& featureImg)
{
    if (!img.empty())
    {
        featureImg = img.clone();

        m_pattern = generatePattern(img);

        // train matcher
        m_matcher->clear();
        m_matcher->add(m_pattern->descriptors());
        m_matcher->train();

        // generate retMat
        for (size_t i = 0; i < m_pattern->keypoints().size(); i++)
        {
            cv::putText(featureImg, "+", m_pattern->keypoints()[i].pt, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255));
        }
    }
}

bool PatternDetector::findPatternFromScene(const Mat& sceneImg)
{
    if (m_pattern == NULL)
    {
        qWarning() << "No valid pattern!";
        return false;
    }

    std::vector<cv::KeyPoint> keypoints;
    extractFeature(sceneImg, keypoints, m_queryDescriptors);
    if (keypoints.size() >= m_pattern->keypoints().size())
    {
        vector<DMatch> matches;
        getMatches(m_queryDescriptors, matches);
        //homography refinement
        bool homographyFound = refineMatchesWithHomography(keypoints, matches, m_homography);
        if (homographyFound)
        {
            refineHomography(sceneImg);
            vector<Point2f> trackerPt2D;
            perspectiveTransform(m_pattern->points2d(), trackerPt2D, m_homography);
            m_patternTracker->setPoints2D(trackerPt2D);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool PatternDetector::computePose(const CameraIntrinsic& intrinsic)
{
    if (m_pattern == NULL)
    {
        qWarning() << "No valid pattern!";
        return false;
    }
    if (m_patternTracker->ponits2d().empty())
    {
        qWarning() << "Pattern tracker 2D perspective not valid!";
        return false;
    }

    Mat rotation, translation;
    if (!solvePnP(m_pattern->points3d(), m_patternTracker->ponits2d(),
                  intrinsic.cameraMat, intrinsic.distortCoeff,
                  rotation, translation))
    {
        qWarning() << "Failed solving PnP!";
        return false;
    }
    Pose pose(rotation, translation);
    m_patternTracker->setPose(pose);

    return true;
}

Ptr<Pattern> PatternDetector::generatePattern(const Mat& img)
{
    Mat grayImg;
    if (img.type() == CV_8UC1)
    {
        grayImg = img.clone();
    }
    else
    {
        cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);
    }
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    extractFeature(img, keypoints, descriptors);
    // generate pattern
    vector<Point2f> points2d = vector<Point2f>(4);
    vector<Point3f> points3d = vector<Point3f>(4);

    const float w = img.cols;
    const float h = img.rows;
    const float maxSize = std::max(w, h);
    const float unitW = w / maxSize;
    const float unitH = h / maxSize;

    points2d[0] = Point2f(0, 0);
    points2d[1] = Point2f(w, 0);
    points2d[2] = Point2f(w, h);
    points2d[3] = Point2f(0, h);

    points3d[0] = Point3f(-unitW, unitH, 0);
    points3d[1] = Point3f(unitW, unitH, 0);
    points3d[2] = Point3f(unitW, -unitH, 0);
    points3d[3] = Point3f(-unitW, -unitH, 0);
    cv::Size imgSize = Size(img.cols, img.rows);
    cv::Mat origImg = img.clone();
    return Ptr<Pattern>(new Pattern(imgSize, origImg,
                                    keypoints, descriptors,
                                    points2d, points3d));
}

void PatternDetector::extractFeature(const Mat& img, vector<KeyPoint>& keypoints, Mat& descriptors)
{
    m_detector->detect(img, keypoints);
    // filter duplicated keypoints and retain best 500
    KeyPointsFilter::removeDuplicated(keypoints);
    KeyPointsFilter::retainBest(keypoints, 500);
    // compute descriptor
    m_detector->compute(img, keypoints, descriptors);
}

void PatternDetector::getMatches(const Mat& descriptors, vector<DMatch>& matches)
{
    const float minRatio = 1.f / 1.5f;
    vector<vector<DMatch>> knnMatches;
    m_matcher->knnMatch(descriptors, knnMatches, 2);
    for (size_t i = 0; i < knnMatches.size(); i++)
    {
        const DMatch& bestMatch = knnMatches[i][0];
        const DMatch& betterMatch = knnMatches[i][1];

        float distRatio = bestMatch.distance / betterMatch.distance;
        if (distRatio < minRatio)
        {
            matches.push_back(bestMatch);
        }
    }
}

bool PatternDetector::refineMatchesWithHomography(const vector<KeyPoint>& queryKeypoints, vector<DMatch>& matches, Mat& homography)
{
    const int minMatchesAllowed = 8;
    if (matches.size() < minMatchesAllowed)
    {
        return false;
    }
    vector<Point2f> srcPts(matches.size());
    vector<Point2f> dstPts(matches.size());
    for (size_t i = 0; i < matches.size(); i++)
    {
        srcPts[i] = m_pattern->keypoints()[matches[i].trainIdx].pt;
        dstPts[i] = queryKeypoints[matches[i].queryIdx].pt;
    }
    vector<unsigned char> inliersMask(srcPts.size());
    homography = cv::findHomography(srcPts, dstPts, CV_FM_RANSAC, 3, inliersMask);
    vector<DMatch> inliers;
    for (size_t i = 0; i < inliersMask.size(); i++)
    {
        if (inliersMask[i])
            inliers.push_back(matches[i]);
    }
    matches.swap(inliers);
    return matches.size() >= minMatchesAllowed;
}

void PatternDetector::refineHomography(const Mat& sceneImg)
{
    Mat wrappedImg;
    warpPerspective(sceneImg, wrappedImg,
                    m_homography, m_pattern->size(),
                    WARP_INVERSE_MAP | INTER_CUBIC);
    vector<KeyPoint> warpedKeypoints;
    vector<DMatch> refinedMatches;
    extractFeature(wrappedImg, warpedKeypoints, m_queryDescriptors);
    getMatches(m_queryDescriptors, refinedMatches);
    Mat refinedHomography;
    bool homographyFound = refineMatchesWithHomography(warpedKeypoints,
                                                       refinedMatches,
                                                       refinedHomography);
    if (homographyFound)
    {
        m_homography = m_homography * refinedHomography;
        //        Mat matchImg;
        //        drawMatches(m_pattern->data(), m_pattern->keypoints(), wrappedImg, warpedKeypoints, refinedMatches, matchImg);
        //        imshow("match image", matchImg);
    }
}

void PatternDetector::showHomography()
{
    qDebug() << "Homography:";
    QCvDataUtils::showCvMat2D(m_homography);
}
