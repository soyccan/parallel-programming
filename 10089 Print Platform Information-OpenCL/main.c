#include <CL/cl.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF 1024
#define MAX_DEVICES 24
#define MAX_PLATFORMS 24

#define arrsize(arr) (sizeof(arr) / sizeof(arr[0]))

#define FG_RED "\e[31m"
#define FG_RESET "\e[0m"

#define clCheckError(command)                                                  \
  {                                                                            \
    cl_int ret = (command);                                                    \
    if (ret != CL_SUCCESS) {                                                   \
      fprintf(stderr, FG_RED "Error %s: Line %u in file %s\n" FG_RESET,        \
              clErrorStr(ret), __LINE__, __FILE__);                            \
    }                                                                          \
  }

char *clErrorStr(cl_int err) {
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

void startCL() {
  cl_platform_id platforms[MAX_PLATFORMS];
  cl_uint platformN = 0;
  clCheckError(clGetPlatformIDs(arrsize(platforms), platforms, &platformN));
  printf("%d platform found\n", platformN);

  for (int i = 0; i < platformN; i++) {
    char buf[MAX_BUF];
    size_t bufN = 0;
    cl_platform_id platform = platforms[i];

    bufN = 0;
    clCheckError(
        clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(buf), buf, &bufN));
    printf("Platform Name %s\n", buf);
    bufN = 0;
    clCheckError(clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(buf),
                                   buf, &bufN));
    printf("Platform Vendor %s\n", buf);
    bufN = 0;
    clCheckError(clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(buf),
                                   buf, &bufN));
    printf("Platform Version %s\n", buf);
    bufN = 0;
    clCheckError(clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, sizeof(buf),
                                   buf, &bufN));
    printf("Platform Profile %s\n", buf);

    cl_device_id devices[MAX_DEVICES];
    cl_uint devicesN = 0, devCpuN = 0, devGpuN = 0;
    clCheckError(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, arrsize(devices),
                                devices, &devicesN));
    printf("%u Devices\n", devicesN);
    clCheckError(
        clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &devCpuN));
    printf("%u CPU Devices\n", devCpuN);
    clCheckError(
        clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &devGpuN));
    printf("%u GPU Devices\n", devGpuN);

    for (int j = 0; j < devicesN; j++) {
      cl_device_id device = devices[j];
      cl_ulong tmp = 0;
      clCheckError(
          clGetDeviceInfo(device, CL_DEVICE_NAME, MAX_BUF, buf, &bufN));
      printf("Device name %s\n", buf);
      tmp = 0;
      clCheckError(clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE,
                                   sizeof(tmp), &tmp, NULL));
      printf("Global memory size %lu\n", tmp);
      tmp = 0;
      clCheckError(clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE,
                                   sizeof(tmp), &tmp, NULL));
      printf("Local memory size %lu\n", tmp);
      tmp = 0;
      clCheckError(clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS,
                                   sizeof(tmp), &tmp, NULL));
      printf("# of compute units %lu\n", tmp);
      tmp = 0;
      clCheckError(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE,
                                   sizeof(tmp), &tmp, NULL));
      printf("max # of work items in a work group %lu\n", tmp);
    }
  }
}

int main() {
  startCL();
  return 0;
}
