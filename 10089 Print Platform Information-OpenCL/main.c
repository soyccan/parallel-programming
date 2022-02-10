#include <CL/cl.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF 1024
#define MAX_DEVICES 24
#define MAX_PLATFORMS 24

#define arrsize(arr) (sizeof(arr) / sizeof(arr[0]))

void startCL() {
  cl_uint platformN;
  cl_platform_id platforms[MAX_PLATFORMS];
  clGetPlatformIDs(arrsize(platforms), platforms, &platformN);
  printf("%d platform found\n", platformN);

  for (int i = 0; i < platformN; i++) {
    char buf[MAX_BUF];
    size_t bufN;
    cl_platform_id platform = platforms[i];

    clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(buf), buf, &bufN);
    printf("Platform Name %s\n", buf);
    clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(buf), buf, &bufN);
    printf("Platform Vendor %s\n", buf);
    clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(buf), buf, &bufN);
    printf("Platform Version %s\n", buf);
    clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, sizeof(buf), buf, &bufN);
    printf("Platform Profile %s\n", buf);

    cl_device_id devices[24];
    cl_uint devicesN;

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, arrsize(devices), devices,
                   &devicesN);
    printf("%u Devices\n", devicesN);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, arrsize(devices), devices,
                   &devicesN);
    printf("%u CPU Devices\n", devicesN);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, arrsize(devices), devices,
                   &devicesN);
    printf("%u GPU Devices\n", devicesN);

    for (int j = 0; j < devicesN; j++) {
      cl_device_id device = devices[j];
      cl_ulong tmp;
      clGetDeviceInfo(device, CL_DEVICE_NAME, MAX_BUF, buf, &bufN);
      printf("Device name %s\n", buf);
      clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(tmp), &tmp,
                      NULL);
      printf("Global memory size %lld\n", (long long)tmp);
      clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(tmp), &tmp,
                      NULL);
      printf("Local memory size %lld\n", (long long)tmp);
      clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(tmp), &tmp,
                      NULL);
      printf("# of compute units %lld\n", (long long)tmp);
      clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(tmp), &tmp,
                      NULL);
      printf("max # of work items in a work group %lld\n", (long long)tmp);
    }
  }
}

int main() {
  startCL();
  return 0;
}
