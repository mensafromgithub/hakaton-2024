#include <torch/torch.h>
#include <opencv2/opencv.hpp>

int main() {
    using namespace std;
    cv::Mat image = cv::imread("b1Hei5UsKIc.jpg");
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
    image.convertTo(image, CV_32F, 1.0f / 255);

    torch::Tensor image_tensor = torch::from_blob(image.data, {1, image.rows, image.cols, 3}, torch::kFloat32);
    image_tensor = image_tensor.permute({0, 3, 1, 2});

    torch::Tensor numeric_tensor = torch::tensor({1.0, 2.0, 3.0}, torch::kFloat32);

    torch::jit::script::Module model = torch::jit::load("model.pt");

    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(image_tensor);
    inputs.push_back(numeric_tensor);

    at::Tensor output = model.forward(inputs).toTensor();

    cout << "Output: " << output << endl;

    return 0;
}