#include "opencv_ptest/include/opencv2/ts/ts.hpp"

#include <iostream>

#include "skeleton_filter.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace perf;
using namespace cv;
using std::tr1::make_tuple;
using std::tr1::get;


#define PATH "./bin/testdata"

#define IMAGES testing::Values( std::string(PATH"/sla.png"),\
                                std::string(PATH"/page.png"),\
                                std::string(PATH"/schedule.png") )

typedef perf::TestBaseWithParam<std::string> ImageName;

//
// Test(s) for the ConvertColor_BGR2GRAY_BT709 function
//

PERF_TEST_P(ImageName, ConvertColor_BGR2GRAY_BT709, IMAGES)
{
	Mat input = cv::imread(GetParam());
	Mat result(input.size(), CV_8UC1);
	declare.in(input, WARMUP_RNG).out(result).time(500).iterations(10);

	TEST_CYCLE()
	{
		ConvertColor_BGR2GRAY_BT709(input, result);
	}
	SANITY_CHECK(result, 1 + 1e-6);
}

//
// Test(s) for the ImageResize function
//

#define MAT_SIZES  ::perf::szVGA, ::perf::sz720p, ::perf::sz1080p

typedef perf::TestBaseWithParam<Size> Size_Only;

PERF_TEST_P(Size_Only, ImageResize, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();
    Size sz_to(sz.width / 2, sz.height / 2);

    cv::Mat src(sz, CV_8UC1);
    cv::Mat dst(Size(sz_to), CV_8UC1);
    declare.in(src, WARMUP_RNG).out(dst);

    cv::RNG rng(234231412);
    rng.fill(src, CV_8UC1, 0, 255);

    TEST_CYCLE()
    {
        ImageResize(src, dst, sz_to);
    }

    SANITY_CHECK(dst);
}

//
// Test(s) for the skeletonize function
//

PERF_TEST_P(Size_Only, Thinning, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();

    cv::Mat image(sz, CV_8UC1);
    declare.in(image, WARMUP_RNG).out(image);
    declare.time(40);

    cv::RNG rng(234231412);
    rng.fill(image, CV_8UC1, 0, 255);
    cv::threshold(image, image, 240, 255, cv::THRESH_BINARY_INV);

    cv::Mat gold; GuoHallThinning(image, gold);

    cv::Mat thinned_image;
    TEST_CYCLE()
    {
        GuoHallThinning_optimized(image, thinned_image);
    }

    cv::Mat diff; cv::absdiff(thinned_image, gold, diff);
    ASSERT_EQ(0, cv::countNonZero(diff));

    SANITY_CHECK(image);
}

PERF_TEST_P(ImageName, skeletonize_without_saving, IMAGES)
{
	Mat input = cv::imread(GetParam());
	Mat result(input.size(), CV_8UC1);
	declare.in(input, WARMUP_RNG).out(result).time(500).iterations(10);

	TEST_CYCLE()
    {
        skeletonize(input, result, false);
    }

    SANITY_CHECK(result, 1 + 1e-6);
}

PERF_TEST_P(ImageName, skeletonize_with_saving, IMAGES)
{
	Mat input = cv::imread(GetParam());
	Mat result(input.size(), CV_8UC1);
	declare.in(input, WARMUP_RNG).out(result).time(500).iterations(10);

	TEST_CYCLE()
    {
        skeletonize(input, result, true);
    }

    SANITY_CHECK(result, 1 + 1e-6);
}

PERF_TEST_P(Size_Only, GuoHallThinning, testing::Values(MAT_SIZES))
{
    Size sz_input = GetParam();

    Mat input(sz_input, CV_8UC1);
	randu(input, Scalar(0), Scalar(255));
	Mat result(input.size(), CV_8UC1);
	declare.in(input, WARMUP_RNG).out(result).time(500).iterations(10);

    TEST_CYCLE()
    {
        GuoHallThinning(input, result);
    }

    SANITY_CHECK(result, 1 + 1e-6);
}
