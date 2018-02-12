#include <caffe/caffe.hpp>
#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif  // USE_OPENCV
#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <sstream>

#ifdef USE_OPENCV
using namespace caffe;  // NOLINT(build/namespaces)
using std::string;

DEFINE_bool(GPU, true, "Use GPU or not");

class Segmentation {
 public:
	Segmentation(const string& model_file,
             	 const string& trained_file,
				 const string& colormap_file);

	shared_ptr<cv::Mat> Classify(const cv::Mat& img);

 private:
   const shared_ptr<Blob<float> > Predict(const cv::Mat& img);
   void WrapInputLayer(std::vector<cv::Mat>* input_channels);
   void Preprocess(const cv::Mat& img,
                     std::vector<cv::Mat>* input_channels);

 private:
   shared_ptr<Net<float> > net_;
   cv::Size input_geometry_;
   int num_channels_;
   cv::Mat mean_;
   std::vector<cv::Vec3b> labels_;
};

// Get all available GPU devices
static void get_gpus(vector<int>* gpus) {
    int count = 0;
#ifndef CPU_ONLY
    count = Caffe::EnumerateDevices(true);
#else
    NO_GPU;
#endif
    for (int i = 0; i < count; ++i) {
      gpus->push_back(i);
    }
}

Segmentation::Segmentation(const string& model_file,
                       const string& trained_file,
                       const string& label_file) {
  #ifdef CPU_ONLY
  if(FLAGS_GPU)
	  LOG(FATAL) << "Can't use GPU in CPU only caffe";
  #endif
  // Set device id and mode
  vector<int> gpus;
  get_gpus(&gpus);
  if (gpus.size() != 0 && FLAGS_GPU) 
  {
	std::cout << "Use GPU with device ID " << gpus[0] << std::endl;
    Caffe::SetDevices(gpus);
    Caffe::set_mode(Caffe::GPU);
    Caffe::SetDevice(gpus[0]);
  }
  else{
    std::cout << "Use CPU" << std::endl;
    Caffe::set_mode(Caffe::CPU);
  }

  /* Load the network. */
  net_.reset(new Net<float>(model_file, TEST, Caffe::GetDefaultDevice()));
  net_->CopyTrainedLayersFrom(trained_file);

  CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

  Blob<float>* input_layer = net_->input_blobs()[0];
  num_channels_ = input_layer->channels();
  CHECK(num_channels_ == 3 || num_channels_ == 1)
    << "Input layer should have 1 or 3 channels.";
  input_geometry_ = cv::Size(input_layer->width(), input_layer->height());

  cv::Scalar channel_mean(104.0, 117.0, 123.0);
  mean_ = cv::Mat(input_geometry_, CV_32FC3, channel_mean);

  /* Load labels. */
  std::ifstream labels(label_file.c_str());
  CHECK(labels) << "Unable to open labels file " << label_file;
  string line;
  while (std::getline(labels, line))
  {
	  std::istringstream istr(line);
	  float r, g, b;
	  istr >> r >> g >> b;
	  cv::Vec3b pix(int(b*255), int(g*255), int(r*255));
	  labels_.push_back(pix);
  }
}

shared_ptr<cv::Mat> Segmentation::Classify(const cv::Mat& img) {
	// Convert the caffe blog as CV Mat
	const shared_ptr<Blob<float> > score_blob = Predict(img);
	float * score_data = score_blob->mutable_cpu_data();
	std::vector<cv::Mat> channels;
	// B,C,H,W order
	int width = score_blob->width();
	int height = score_blob->height();
	int num_channel = score_blob->channels();
	for(int i = 0; i < num_channel; i++)
	{
		cv::Mat channel(height, width, CV_32FC1, score_data);
	    channels.push_back(channel);
	    score_data += height * width;
	}
	// resize the predicted score map as same big as input image
	cv::Mat mat, mat_resized;
	cv::merge(channels, mat);
	cv::resize(mat, mat_resized, input_geometry_);

	// Map pixel into colors
	shared_ptr<cv::Mat> maskMat(new cv::Mat(input_geometry_, CV_8UC3));
	for(int row = 0; row < input_geometry_.height ; row++)
		for(int col = 0; col < input_geometry_.width; col++)
		{
			float * score_pix = mat_resized.ptr<float>(row, col);
			std::vector<float> vScores(score_pix, score_pix + num_channel );
			std::vector<float>::iterator it = max_element(vScores.begin(), vScores.end());
			int classId = it - vScores.begin();
			maskMat->at< cv::Vec3b >(row, col) = labels_[classId];
		}

	return maskMat;
}

const shared_ptr<Blob<float> > Segmentation::Predict(const cv::Mat& img) {
  Blob<float>* input_layer = net_->input_blobs()[0];
  input_layer->Reshape(1, num_channels_,
                       input_geometry_.height, input_geometry_.width);
  /* Forward dimension change to all layers. */
  net_->Reshape();

  std::vector<cv::Mat> input_channels;
  WrapInputLayer(&input_channels);

  Preprocess(img, &input_channels);

  net_->Forward();

  const shared_ptr<Blob<float> > score = net_->blob_by_name("conv_score");
  return score;
}

/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
void Segmentation::WrapInputLayer(std::vector<cv::Mat>* input_channels) {
  Blob<float>* input_layer = net_->input_blobs()[0];

  int width = input_layer->width();
  int height = input_layer->height();
  float* input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_layer->channels(); ++i) {
    cv::Mat channel(height, width, CV_32FC1, input_data);
    input_channels->push_back(channel);
    input_data += width * height;
  }
}

void Segmentation::Preprocess(const cv::Mat& img,
                            std::vector<cv::Mat>* input_channels) {
  /* Convert the input image to the input image format of the network. */
  cv::Mat sample;
  if (img.channels() == 3 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
  else if (img.channels() == 4 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
  else if (img.channels() == 4 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
  else if (img.channels() == 1 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
  else
    sample = img;

  cv::Mat sample_resized;
  if (sample.size() != input_geometry_)
    cv::resize(sample, sample_resized, input_geometry_);
  else
    sample_resized = sample;

  cv::Mat sample_float;
  if (num_channels_ == 3)
    sample_resized.convertTo(sample_float, CV_32FC3);
  else
  {
    //sample_resized.convertTo(sample_float, CV_32FC1);
    LOG(FATAL) << " Single channel segmentation not supported yet ";
  }

  cv::Mat sample_normalized;
  cv::subtract(sample_float, mean_, sample_normalized);

  /* This operation will write the separate BGR planes directly to the
   * input layer of the network because it is wrapped by the cv::Mat
   * objects in input_channels. */
  cv::split(sample_normalized, *input_channels);

  CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
        == net_->input_blobs()[0]->cpu_data())
    << "Input channels are not wrapping the input layer of the network.";
}

int main(int argc, char** argv) {

  google::ParseCommandLineFlags(&argc, &argv, true);
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " deploy.prototxt network.caffemodel"
			  << " colormap.txt"
              << " img.jpg" << std::endl;
    return 1;
  }

  ::google::InitGoogleLogging(argv[0]);

  string model_file   = argv[1];
  string trained_file = argv[2];
  string colormap_file = argv[3];
  string file = argv[4];
  Segmentation segmentation(model_file, trained_file, colormap_file);

  std::cout << "---------- Segmentation for "
            << file << " ----------" << std::endl;

  cv::Mat img = cv::imread(file, -1);
  shared_ptr<cv::Mat> mask = segmentation.Classify(img);

  int pos = file.find_last_of('.');
  string seg_file = file.substr(0, pos) + "_seg" + file.substr(pos);
  cv::imwrite(seg_file, *mask);
/*
  cv::namedWindow("wnd", cv::WINDOW_AUTOSIZE);
  cv::imshow("wnd", *mask);
  cv::waitKey(0);
*/
}
#else
int main(int argc, char** argv) {
  LOG(FATAL) << "This example requires OpenCV; compile with USE_OPENCV.";
}
#endif  // USE_OPENCV
