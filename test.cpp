#include <nlohmann\json.hpp>
#include<torch/script.h>
#include <iostream>
#include <memory>
#include<torch/torch.h>
#include <fstream>
#include <string>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc.hpp>
using json = nlohmann::json;


using  namespace std;
using namespace cv;


double distance(std::vector<double> vec1, double vec2) {
    //std::vector<double> min ;
    double temp = 0;
    double total = 0;
    //double dist;
    auto dataSize = std::size(vec1);
    for (int i = 0; i < dataSize; i++) {
        temp = (std::abs(vec1[i] - vec2));
        //return std::abs(v2[i] - v1[i]);
        temp = pow(temp, 2);
        total += temp;
    }
    return sqrt(total);
}



int main() {

    Mat image = imread("E:/libfacedetection-master/Build/Debug/144.jpg");

    resize(image, image, Size(224, 224));

    torch::Tensor img_tensor = torch::from_blob(image.data, { 1,image.rows,image.cols ,3 }, torch::kByte);

    img_tensor = img_tensor.permute({ 0,3,1,2 });

    img_tensor = img_tensor.toType(torch::kFloat);

    img_tensor = img_tensor.div(255);

    using torch::jit::Module;

    Module module = torch::jit::load("E:/libfacedetection-master/Build/resnet50test6.pt");

    std::cout << " test pth ok\n";

    at::Tensor output = module.forward({ torch::ones({1, 3, 224, 224}) }).toTensor();

    std::ifstream file("person.json");

    json object = json::parse(file);


    double min_dis = 1000;
    std::string min_name;

    for (auto& x : object.items()) {
        auto dataSize = std::size(x.value());

        for (int i = 0; i < dataSize; i++) {
            std::vector<double> vec1 = x.value()[i];
            double res = distance(vec1, output[0][i].item().to<double>());
            /*std::cout << i << " " << res << std::endl;*/
            if (res <= min_dis) {
                min_dis = res;
                min_name = x.key();
            }
        }

    }

    std::cout << min_name << " " << min_dis << std::endl;

    while (1)
        ;

    return 0;
}
