#include <opencv2/opencv.hpp>

#include "./include/Processing.NDI.Lib.h"

#define ASYNC 1

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {
  // Init opencv
  VideoCapture capture(0);
  Mat cameraFrame;
  Mat ndiFrame;
  if (!capture.isOpened()) {
    cout << "Could not open camera" << endl;
    return -1;
  }

  // Init NDI
  if (!NDIlib_initialize()) {
    cout << "Failed to init NDI" << endl;
    return -1;
  }

  // Sender
  const char* name = "NDI-Test";
  const NDIlib_send_create_t opts = NDIlib_send_create_t(name, NULL, true, true);
  NDIlib_send_instance_t pNDI_send = NDIlib_send_create(&opts);
  if (!pNDI_send) {
    cout << "Failed to create a NDI sender" << endl;
    return -1;
  }

  // NDI frame
  // size of the grabbed camera stream
  const int W = 1280;
  const int H = 720;
  const int pixelSize = 4;
  NDIlib_video_frame_v2_t NDI_video_frame;
  NDI_video_frame.xres = W;
  NDI_video_frame.yres = H;
  NDI_video_frame.FourCC = NDIlib_FourCC_type_RGBA;
  NDI_video_frame.line_stride_in_bytes = W * pixelSize;
#if ASYNC
  void* p_frame_buffers[2] = {malloc(W * H * pixelSize), malloc(W * H * pixelSize)};
  bool first = true;
#else
  NDI_video_frame.p_data = (uint8_t*)malloc(W * H * pixelSize);
#endif

  while (true) {
    capture.read(cameraFrame);
    cvtColor(cameraFrame, ndiFrame, COLOR_BGRA2RGBA);
#if ASYNC
    memcpy((void*)p_frame_buffers[first ? 0 : 1], (const void*)ndiFrame.data, W * H * pixelSize);
    NDI_video_frame.p_data = (uint8_t*)p_frame_buffers[first ? 0 : 1];
    NDIlib_send_send_video_async_v2(pNDI_send, &NDI_video_frame);
    first = !first;
#else
    NDI_video_frame.p_data = (uint8_t*)ndiFrame.data;
    NDIlib_send_send_video_async_v2(pNDI_send, &NDI_video_frame);
#endif
    imshow("output", cameraFrame);
    if (cv::waitKey(50) >= 0) break;
  }

  // Free the video frames
  free(NDI_video_frame.p_data);
  free(&cameraFrame);
  free(&ndiFrame);
#if ASYNC
  NDIlib_send_send_video_async_v2(pNDI_send, NULL);
  free(p_frame_buffers[0]);
  free(p_frame_buffers[1]);
#endif
  // Destroy the NDI sender
  NDIlib_send_destroy(pNDI_send);
  // Not required, but nice
  NDIlib_destroy();
  // Success
  return 0;
}
