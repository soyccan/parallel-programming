#include <CL/cl.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 4096
#define MAX_DEVICES 16
#define MAX_PLATFORMS 8

#define FG_RED "\e[31m"
#define FG_RESET "\e[0m"

#define arrsize(arr) (sizeof(arr) / sizeof(arr[0]))

#define logerr(fmt, ...)                                                       \
  fprintf(stderr, FG_RED fmt "\n" FG_RESET, ##__VA_ARGS__)

#ifndef NDEBUG
#define logdbg(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define logdbg(...)
#endif

#define clCheckError(command)                                                  \
  {                                                                            \
    cl_int _ret = (command);                                                   \
    if (_ret != CL_SUCCESS) {                                                  \
      logerr("Error %s: Line %u in file %s", clErrorStr(_ret), __LINE__,       \
             __FILE__);                                                        \
    }                                                                          \
  }

#define syscallCheckError(command)                                             \
  {                                                                            \
    ssize_t _ret = (command);                                                  \
    if (_ret != 0) {                                                           \
      logerr("Error %s: Line %u in file %s", strerror(_ret), __LINE__,         \
             __FILE__);                                                        \
    }                                                                          \
  }

const char *clErrorStr(cl_int err) {
  switch (err) {
#define CaseReturnString(x)                                                    \
  case x:                                                                      \
    return #x;

    CaseReturnString(CL_SUCCESS);
    CaseReturnString(CL_DEVICE_NOT_FOUND);
    CaseReturnString(CL_DEVICE_NOT_AVAILABLE);
    CaseReturnString(CL_COMPILER_NOT_AVAILABLE);
    CaseReturnString(CL_MEM_OBJECT_ALLOCATION_FAILURE);
    CaseReturnString(CL_OUT_OF_RESOURCES);
    CaseReturnString(CL_OUT_OF_HOST_MEMORY);
    CaseReturnString(CL_PROFILING_INFO_NOT_AVAILABLE);
    CaseReturnString(CL_MEM_COPY_OVERLAP);
    CaseReturnString(CL_IMAGE_FORMAT_MISMATCH);
    CaseReturnString(CL_IMAGE_FORMAT_NOT_SUPPORTED);
    CaseReturnString(CL_BUILD_PROGRAM_FAILURE);
    CaseReturnString(CL_MAP_FAILURE);
    CaseReturnString(CL_MISALIGNED_SUB_BUFFER_OFFSET);
    CaseReturnString(CL_COMPILE_PROGRAM_FAILURE);
    CaseReturnString(CL_LINKER_NOT_AVAILABLE);
    CaseReturnString(CL_LINK_PROGRAM_FAILURE);
    CaseReturnString(CL_DEVICE_PARTITION_FAILED);
    CaseReturnString(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
    CaseReturnString(CL_INVALID_VALUE);
    CaseReturnString(CL_INVALID_DEVICE_TYPE);
    CaseReturnString(CL_INVALID_PLATFORM);
    CaseReturnString(CL_INVALID_DEVICE);
    CaseReturnString(CL_INVALID_CONTEXT);
    CaseReturnString(CL_INVALID_QUEUE_PROPERTIES);
    CaseReturnString(CL_INVALID_COMMAND_QUEUE);
    CaseReturnString(CL_INVALID_HOST_PTR);
    CaseReturnString(CL_INVALID_MEM_OBJECT);
    CaseReturnString(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
    CaseReturnString(CL_INVALID_IMAGE_SIZE);
    CaseReturnString(CL_INVALID_SAMPLER);
    CaseReturnString(CL_INVALID_BINARY);
    CaseReturnString(CL_INVALID_BUILD_OPTIONS);
    CaseReturnString(CL_INVALID_PROGRAM);
    CaseReturnString(CL_INVALID_PROGRAM_EXECUTABLE);
    CaseReturnString(CL_INVALID_KERNEL_NAME);
    CaseReturnString(CL_INVALID_KERNEL_DEFINITION);
    CaseReturnString(CL_INVALID_KERNEL);
    CaseReturnString(CL_INVALID_ARG_INDEX);
    CaseReturnString(CL_INVALID_ARG_VALUE);
    CaseReturnString(CL_INVALID_ARG_SIZE);
    CaseReturnString(CL_INVALID_KERNEL_ARGS);
    CaseReturnString(CL_INVALID_WORK_DIMENSION);
    CaseReturnString(CL_INVALID_WORK_GROUP_SIZE);
    CaseReturnString(CL_INVALID_WORK_ITEM_SIZE);
    CaseReturnString(CL_INVALID_GLOBAL_OFFSET);
    CaseReturnString(CL_INVALID_EVENT_WAIT_LIST);
    CaseReturnString(CL_INVALID_EVENT);
    CaseReturnString(CL_INVALID_OPERATION);
    CaseReturnString(CL_INVALID_GL_OBJECT);
    CaseReturnString(CL_INVALID_BUFFER_SIZE);
    CaseReturnString(CL_INVALID_MIP_LEVEL);
    CaseReturnString(CL_INVALID_GLOBAL_WORK_SIZE);
    CaseReturnString(CL_INVALID_PROPERTY);
    CaseReturnString(CL_INVALID_IMAGE_DESCRIPTOR);
    CaseReturnString(CL_INVALID_COMPILER_OPTIONS);
    CaseReturnString(CL_INVALID_LINKER_OPTIONS);
    CaseReturnString(CL_INVALID_DEVICE_PARTITION_COUNT);
#undef CaseReturnString
  default:
    return "Unknown OpenCL error code";
  }
}

int main() {
  cl_int ret = 0;
  char buf[MAX_BUF];
  size_t bufN = 0;

  // get platforms
  cl_platform_id platforms[MAX_PLATFORMS];
  cl_uint platformN = 0;
  clCheckError(clGetPlatformIDs(arrsize(platforms), platforms, &platformN));
  logdbg("Found %d platforms", platformN);

  // find CUDA platform
  cl_platform_id platform;
  bool cuda_found = false;
  for (int i = 0; i < platformN; i++) {
    platform = platforms[i];

    bufN = 0;
    clCheckError(
        clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(buf), buf, &bufN));

    const char TARGET_PLATFORM_NAME[] = "NVIDIA CUDA";
    if (memcmp(buf, TARGET_PLATFORM_NAME, sizeof(TARGET_PLATFORM_NAME)) == 0) {
      logdbg("Platform Name: %s", buf);
      cuda_found = true;
      break;
    }
  }
  assert(cuda_found);

  // get devices
  cl_device_id gpus[MAX_DEVICES];
  cl_uint gpuN = 0;
  clCheckError(
      clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, arrsize(gpus), gpus, &gpuN));
  logdbg("Found %u GPUs", gpuN);

  // context
  logdbg("Creating context");
  cl_context context = clCreateContext(NULL, gpuN, gpus, NULL, NULL, &ret);
  clCheckError(ret);

  // command queues
  logdbg("Creating command queue");
  cl_command_queue cmdQueue =
      clCreateCommandQueueWithProperties(context, gpus[0], NULL, &ret);
  clCheckError(ret);

  // kernel source
  logdbg("Reading kernel source");
  const char *kernelSrcs[1];
  size_t kernelSrcSizes[1];
  int kernelFd;
  syscallCheckError(kernelFd = open("vecdot.cl", O_RDONLY));
  syscallCheckError(kernelSrcSizes[0] = lseek(kernelFd, 0, SEEK_END));
  kernelSrcs[0] =
      mmap(NULL, kernelSrcSizes[0], PROT_READ, MAP_PRIVATE, kernelFd, 0);
  if (kernelSrcs[0] == MAP_FAILED) {
    perror("");
    assert(kernelSrcs[0] != MAP_FAILED);
  }
  close(kernelFd);

  // build program
  logdbg("Creating program");
  cl_program program =
      clCreateProgramWithSource(context, 1, kernelSrcs, kernelSrcSizes, &ret);
  clCheckError(ret);

  logdbg("Building program");
  clCheckError(ret = clBuildProgram(program, gpuN, gpus, NULL, NULL, NULL));
  if (ret != CL_SUCCESS) {
    // print log if building failed
    logdbg("Building info of GPU %d", 0);
    size_t buildLogSize = 0;
    clCheckError(clGetProgramBuildInfo(program, gpus[0], CL_PROGRAM_BUILD_LOG,
                                       0, NULL, &buildLogSize));
    buildLogSize++;
    char *buildLog = calloc(buildLogSize, sizeof(char));
    clCheckError(clGetProgramBuildInfo(program, gpus[0], CL_PROGRAM_BUILD_LOG,
                                       buildLogSize, buildLog, NULL));
    printf("%s", buildLog);
    free(buildLog);
  }

  munmap(kernelSrcs[0], kernelSrcSizes[0]);
  clReleaseContext(context);
  clReleaseCommandQueue(cmdQueue);
  clReleaseProgram(program);
}

// vim: sw=2
