/*
* Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
* Released to public domain under terms of the BSD Simplified license.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*   * Neither the name of the organization nor the names of its contributors
*     may be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
*   See <http://www.opensource.org/licenses/bsd-license>
*/

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"


#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

static Mat norm_0_255(InputArray _src) {
	Mat src = _src.getMat();
	// Create and return normalized image:
	Mat dst;
	switch(src.channels()) {
	case 1:
		cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
		break;
	case 3:
		cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
		break;
	default:
		src.copyTo(dst);
		break;
	}
	return dst;
}

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
	std::ifstream file(filename.c_str(), ifstream::in);
	cout << "dddddd" << endl;
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		cout << "Line: " << line << endl;
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		cout << "Label: " << classlabel << endl;
		if(!path.empty() && !classlabel.empty()) {
			images.push_back(imread(path, 0));	//新增元素至 vector 的尾端，必要時會進行記憶體配置
			cout << "AddSize: " << images.size() << endl;
			labels.push_back(atoi(classlabel.c_str()));
		}
	}

}

int main(int argc, const char *argv[]) {
	// Check for valid command line arguments, print usage
	// if no arguments were given.

	if (argc != 3) {
		cout << "usage: " << argv[0] << " <train.ext> <test.ext>" << endl;
		exit(1);
	}
	// Get the path to your CSV.
	string train = string(argv[1]);
	string test = string(argv[2]);

	// These vectors hold the images and corresponding labels.
	vector<Mat> train_images;
	vector<int> train_labels;

	vector<Mat> test_images;
	vector<int> test_labels;
	// Read in the data. This can fail if no valid
	// input filename is given.



	// --------training----------
	read_csv(train, train_images, train_labels);
	cout << "read file ok!!!" << endl;
	int height = train_images[0].rows;
	Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
	//Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
	model->train(train_images, train_labels);
	model->save("D:/data/small/model0.yml");
	train_images.clear();
	train_labels.clear();




	// The following line predicts the label of a given
	// --------testing----------

	read_csv(test, test_images, test_labels);
	cout << "read file ok!!!" << endl;
	//	int height = test_images[0].rows;

	//confusion table
	const int row = 6;
	const int column = 6;
	int arr[row][column];
	int P = 0;
	float accuracy;

	//init
	for(int x=0; x <row; x++)
	{
		for(int y=0; y<column; y++)
		{
			arr[x][y] = 0;
		}
	}

	//Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
	model->load("D:/data/small/model0.yml");
	for(int i = 0; i<test_images.size(); i++)
	{
		int predictedLabel = model->predict(test_images[i]);
		printf("Predicted class = %d / Actual class = %d. \n", predictedLabel, test_labels[i]);
		if(predictedLabel==test_labels[i])
		{
			P++;
		}
	}

	accuracy = (float)P/test_labels.size();
	printf("Accuracy = %f \n", accuracy);

	test_images.clear();
	test_labels.clear();	
	waitKey(0);
	return 0;
}
