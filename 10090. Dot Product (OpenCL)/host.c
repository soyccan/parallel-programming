// #define NDEBUG

#ifdef _WIN32
// Windows only
#include <windows.h>

#elif defined(__unix__) || defined(__unix) ||                                  \
    (defined(__APPLE__) && defined(__MACH__))
// Unix-like only
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#endif

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <CL/cl.h>

#define MAX_INFO_BUF 64
#define MAX_DEVICES 16
#define MAX_PLATFORMS 8
#define MAX_VECLEN 16777216 // 16MB
#define MAX_KERNELS 16

#define NANO2SEC 1000000000.0

#define FG_RED "\033[31m"
#define FG_RESET "\033[0m"

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
      logerr("Error at " __FILE__ " (" STRINGIZE(__LINE__) "): %s",            \
                                                 clErrorStr(_ret));            \
      abort();                                                                 \
    }                                                                          \
  }

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

#ifdef _WIN32
// Windows only
#define assert_showerr_win(expression)                                         \
  {                                                                            \
    if (!(expression)) {                                                       \
      logerr("Error at " __FILE__ " (" STRINGIZE(__LINE__) "): %s",            \
                                                 GetLastErrorStr());           \
      abort();                                                                 \
    }                                                                          \
  }
#endif

#define assert_showerr(expression)                                             \
  {                                                                            \
    if (!(expression)) {                                                       \
      logerr("Error at " __FILE__ " (" STRINGIZE(__LINE__) "): %s",            \
                                                 strerror(errno));             \
    }                                                                          \
  }

#ifdef _WIN32 // Windows only
LPTSTR GetLastErrorStr() {
  LPTSTR errstr;
  DWORD error = GetLastError();

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, error, 0, (LPTSTR)&errstr, 0, NULL);

  // require manual LocalFree() by caller
  return errstr;
}
#endif // ifdef _WIN32

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

uint32_t resvec[MAX_VECLEN];

int main() {
  cl_int ret = 0;

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

    size_t bufN = 0;
    char buf[MAX_INFO_BUF];
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

  // command queue
  logdbg("Creating command queue on GPU 0");
  cl_queue_properties queueProps[] = {CL_QUEUE_PROPERTIES,
                                      CL_QUEUE_PROFILING_ENABLE, 0};
  cl_command_queue cmdQueue =
      clCreateCommandQueueWithProperties(context, gpus[0], queueProps, &ret);
  clCheckError(ret);

  // kernel source
  logdbg("Reading kernel source");
#ifdef _WIN32
  // windows only
  HANDLE kernelFile, kernelFileMapping;
  DWORD kernelSrcSize;
  const char *kernelSrc;
  kernelFile = CreateFile("vecdot.cl", GENERIC_READ, 0, NULL, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, NULL);
  assert_showerr_win(kernelFile != INVALID_HANDLE_VALUE);
  kernelSrcSize = SetFilePointer(kernelFile, 0, NULL, FILE_END);
  assert_showerr_win(kernelSrcSize != INVALID_SET_FILE_POINTER);
  logdbg("kernel source size = %d", kernelSrcSize);
  kernelFileMapping =
      CreateFileMapping(kernelFile, NULL, PAGE_READONLY, 0, 0, NULL);
  assert_showerr_win(kernelFileMapping != NULL);
  kernelSrc = MapViewOfFile(kernelFileMapping, FILE_MAP_READ, 0, 0, 0);
  assert_showerr_win(kernelSrc != NULL);
  CloseHandle(kernelFileMapping);
  CloseHandle(kernelFile);

#elif defined(__unix__) || defined(__unix) ||                                  \
    (defined(__APPLE__) && defined(__MACH__))
  // unix-like only
  int kernelFd;
  off_t kernelSrcSize;
  const char *kernelSrc;
  kernelFd = open("vecdot.cl", O_RDONLY);
  assert_showerr(kernelFd >= 0);
  kernelSrcSize = lseek(kernelFd, 0, SEEK_END);
  assert_showerr(kernelSrcSize >= 0);
  logdbg("kernel source size = %d", kernelSrcSize);
  kernelSrc = mmap(NULL, kernelSrcSize, PROT_READ, MAP_PRIVATE, kernelFd, 0);
  assert_showerr(kernelSrc != MAP_FAILED);
  close(kernelFd);
#endif

  // program
  logdbg("Creating program");
  const char *kernelSrcs[1] = {kernelSrc};
  size_t kernelSrcSizes[1] = {kernelSrcSize};
  cl_program program =
      clCreateProgramWithSource(context, 1, kernelSrcs, kernelSrcSizes, &ret);
  clCheckError(ret);

  // build program for GPU 0 only
  logdbg("Building program");
  clCheckError(ret = clBuildProgram(program, 1, gpus, NULL, NULL, NULL));
  if (ret != CL_SUCCESS) {
    // print log if building failed
    logdbg("Building info of GPU 0:");
    size_t buildLogSize = 0;
    clCheckError(clGetProgramBuildInfo(program, gpus[0], CL_PROGRAM_BUILD_LOG,
                                       0, NULL, &buildLogSize));
    char *buildLog = malloc(buildLogSize);
    assert_showerr(buildLog != NULL);
    clCheckError(clGetProgramBuildInfo(program, gpus[0], CL_PROGRAM_BUILD_LOG,
                                       buildLogSize, buildLog, NULL));
    fprintf(stderr, "%s", buildLog);
    free(buildLog);
  }
#ifndef NDEBUG
  // dump compiled binary into ptx instructions file
  // compile ptx to cubin file with: nvcc -cubin -arch=sm_20 vecdot.ptx
  // show available arch & code by: nvcc --list-gpu-arch --list-gpu-code
  // then dissasseble cubin with: cuobjdump -sass vecdot.cubin
  logdbg("Dumping kernel binary to file");
  size_t programBinSize = 0;
  clCheckError(clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES,
                                sizeof(programBinSize), &programBinSize, NULL));
  logdbg("kernel binary size = %lu", programBinSize);
  char *programBin = malloc(programBinSize);
  assert_showerr(programBin != NULL);
  clCheckError(clGetProgramInfo(program, CL_PROGRAM_BINARIES, programBinSize,
                                &programBin, NULL));
  FILE *programFile = fopen("vecdot.ptx", "wb");
  assert_showerr(programFile != NULL);
  assert_showerr(fwrite(programBin, 1, programBinSize, programFile) ==
                 programBinSize);
  fclose(programFile);
  free(programBin);
#endif

  // kernel
  logdbg("Creating kernels");
  cl_kernel kernels[MAX_KERNELS];
  cl_uint kernelN = 0;
  clCheckError(
      clCreateKernelsInProgram(program, MAX_KERNELS, kernels, &kernelN));

  // buffer
  logdbg("Creating buffer");
  cl_mem bufResVec =
      clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                     sizeof(resvec), resvec, &ret);

  // events
  cl_event memRdEvent, kernelEvent;

  // main program loop
  logdbg("Entering main loop");
  int veclen = 0;
  uint32_t key1 = 0, key2 = 0;
  while (scanf("%d %" PRIu32 " %" PRIu32, &veclen, &key1, &key2) == 3) {
    logdbg("\n");

    for (int i = 0; i < kernelN; i++) {
      logdbg("\n");

      cl_kernel kernel = kernels[i];

      // kernel arguments
      logdbg("Setting kernel arguments");
      clCheckError(clSetKernelArg(kernel, 0, sizeof(key1), &key1));
      clCheckError(clSetKernelArg(kernel, 1, sizeof(key2), &key2));
      clCheckError(clSetKernelArg(kernel, 2, sizeof(bufResVec), &bufResVec));

      // start kernel
      logdbg("Starting kernel %d", i);
      size_t global_work_size[1] = {veclen};
      size_t local_work_size[1] = {1};
      clCheckError(clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL,
                                          global_work_size, local_work_size, 0,
                                          NULL, &kernelEvent));

      // retrieve result: device -> host
      logdbg("Retrieving result: device -> host");
      // clCheckError(clEnqueueMigrateMemObjects(cmdQueue, 1, &bufResVec,
      //                                         CL_MIGRATE_MEM_OBJECT_HOST, 1,
      //                                         &kernelEvent, &memRdEvent));
      clCheckError(clEnqueueReadBuffer(cmdQueue, bufResVec, CL_TRUE, 0,
                                       sizeof(resvec[0]), resvec, 1,
                                       &kernelEvent, &memRdEvent));

      logdbg("Waiting for all tasks to finish");
      clCheckError(clFinish(cmdQueue));

      // print result
      logdbg("Tasks finished");
      printf("%" PRIu32 "\n", resvec[0]);

      // time profiling
      cl_ulong timebase, time_;
      clCheckError(clGetEventProfilingInfo(kernelEvent,
                                           CL_PROFILING_COMMAND_QUEUED,
                                           sizeof(timebase), &timebase, NULL));
      // kernel
      clCheckError(clGetEventProfilingInfo(kernelEvent,
                                           CL_PROFILING_COMMAND_QUEUED,
                                           sizeof(time_), &time_, NULL));
      logdbg("kernel %d queued: %f", i, (time_ - timebase) / NANO2SEC);
      clCheckError(clGetEventProfilingInfo(kernelEvent,
                                           CL_PROFILING_COMMAND_SUBMIT,
                                           sizeof(time_), &time_, NULL));
      logdbg("kernel %d submit: %f", i, (time_ - timebase) / NANO2SEC);
      clCheckError(clGetEventProfilingInfo(kernelEvent,
                                           CL_PROFILING_COMMAND_START,
                                           sizeof(time_), &time_, NULL));
      logdbg("kernel %d start: %f", i, (time_ - timebase) / NANO2SEC);
      clCheckError(clGetEventProfilingInfo(kernelEvent,
                                           CL_PROFILING_COMMAND_COMPLETE,
                                           sizeof(time_), &time_, NULL));
      logdbg("kernel %d complete: %f", i, (time_ - timebase) / NANO2SEC);

      // retrieve result
      clCheckError(clGetEventProfilingInfo(memRdEvent,
                                           CL_PROFILING_COMMAND_QUEUED,
                                           sizeof(time_), &time_, NULL));
      logdbg("mem read queued: %f", (time_ - timebase) / NANO2SEC);
      clCheckError(clGetEventProfilingInfo(memRdEvent,
                                           CL_PROFILING_COMMAND_SUBMIT,
                                           sizeof(time_), &time_, NULL));
      logdbg("mem read submit: %f", (time_ - timebase) / NANO2SEC);
      clCheckError(clGetEventProfilingInfo(
          memRdEvent, CL_PROFILING_COMMAND_START, sizeof(time_), &time_, NULL));
      logdbg("mem read start: %f", (time_ - timebase) / NANO2SEC);
      clCheckError(clGetEventProfilingInfo(memRdEvent,
                                           CL_PROFILING_COMMAND_COMPLETE,
                                           sizeof(time_), &time_, NULL));
      logdbg("mem read complete: %f", (time_ - timebase) / NANO2SEC);

      kernels[i] = kernel;
    }
  }

  // release resources
  clReleaseContext(context);
  clReleaseCommandQueue(cmdQueue);
#ifdef _WIN32 // Windows only
  UnmapViewOfFile(kernelSrcs[0]);
#elif defined(__unix__) || defined(__unix) ||                                  \
    (defined(__APPLE__) && defined(__MACH__)) // Unix-like only
  munmap(kernelSrcs[0], kernelSrcSizes[0]);
#endif
  clReleaseProgram(program);
  for (int i = 0; i < kernelN; i++) {
    clReleaseKernel(kernels[i]);
  }
  clReleaseMemObject(bufResVec);
}

// vim: sw=2
