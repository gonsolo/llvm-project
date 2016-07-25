/******************** GPUJIT.c - GPUJIT Execution Engine **********************/
/*                                                                            */
/*                     The LLVM Compiler Infrastructure                       */
/*                                                                            */
/* This file is dual licensed under the MIT and the University of Illinois    */
/* Open Source License. See LICENSE.TXT for details.                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*  This file implements GPUJIT, a ptx string execution engine for GPU.       */
/*                                                                            */
/******************************************************************************/

#include "GPUJIT.h"
#include <cuda.h>
#include <cuda_runtime.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static int DebugMode;

static void debug_print(const char *format, ...) {
  if (!DebugMode)
    return;

  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}
#define dump_function() debug_print("-> %s\n", __func__)

/* Define Polly's GPGPU data types. */
struct PollyGPUContextT {
  CUcontext Cuda;
};

struct PollyGPUFunctionT {
  CUfunction Cuda;
  CUmodule CudaModule;
};

struct PollyGPUDevicePtrT {
  CUdeviceptr Cuda;
};

/* Dynamic library handles for the CUDA and CUDA runtime library. */
static void *HandleCuda;
static void *HandleCudaRT;

/* Type-defines of function pointer to CUDA driver APIs. */
typedef CUresult CUDAAPI CuMemAllocFcnTy(CUdeviceptr *, size_t);
static CuMemAllocFcnTy *CuMemAllocFcnPtr;

typedef CUresult CUDAAPI CuFuncSetBlockShapeFcnTy(CUfunction, int, int, int);
static CuFuncSetBlockShapeFcnTy *CuFuncSetBlockShapeFcnPtr;

typedef CUresult CUDAAPI CuParamSetvFcnTy(CUfunction, int, void *,
                                          unsigned int);
static CuParamSetvFcnTy *CuParamSetvFcnPtr;

typedef CUresult CUDAAPI CuParamSetSizeFcnTy(CUfunction, unsigned int);
static CuParamSetSizeFcnTy *CuParamSetSizeFcnPtr;

typedef CUresult CUDAAPI CuLaunchGridFcnTy(CUfunction, int, int);
static CuLaunchGridFcnTy *CuLaunchGridFcnPtr;

typedef CUresult CUDAAPI CuMemcpyDtoHFcnTy(void *, CUdeviceptr, size_t);
static CuMemcpyDtoHFcnTy *CuMemcpyDtoHFcnPtr;

typedef CUresult CUDAAPI CuMemcpyHtoDFcnTy(CUdeviceptr, const void *, size_t);
static CuMemcpyHtoDFcnTy *CuMemcpyHtoDFcnPtr;

typedef CUresult CUDAAPI CuMemFreeFcnTy(CUdeviceptr);
static CuMemFreeFcnTy *CuMemFreeFcnPtr;

typedef CUresult CUDAAPI CuModuleUnloadFcnTy(CUmodule);
static CuModuleUnloadFcnTy *CuModuleUnloadFcnPtr;

typedef CUresult CUDAAPI CuCtxDestroyFcnTy(CUcontext);
static CuCtxDestroyFcnTy *CuCtxDestroyFcnPtr;

typedef CUresult CUDAAPI CuInitFcnTy(unsigned int);
static CuInitFcnTy *CuInitFcnPtr;

typedef CUresult CUDAAPI CuDeviceGetCountFcnTy(int *);
static CuDeviceGetCountFcnTy *CuDeviceGetCountFcnPtr;

typedef CUresult CUDAAPI CuCtxCreateFcnTy(CUcontext *, unsigned int, CUdevice);
static CuCtxCreateFcnTy *CuCtxCreateFcnPtr;

typedef CUresult CUDAAPI CuDeviceGetFcnTy(CUdevice *, int);
static CuDeviceGetFcnTy *CuDeviceGetFcnPtr;

typedef CUresult CUDAAPI CuModuleLoadDataExFcnTy(CUmodule *, const void *,
                                                 unsigned int, CUjit_option *,
                                                 void **);
static CuModuleLoadDataExFcnTy *CuModuleLoadDataExFcnPtr;

typedef CUresult CUDAAPI CuModuleLoadDataFcnTy(CUmodule *module,
                                               const void *image);
static CuModuleLoadDataFcnTy *CuModuleLoadDataFcnPtr;

typedef CUresult CUDAAPI CuModuleGetFunctionFcnTy(CUfunction *, CUmodule,
                                                  const char *);
static CuModuleGetFunctionFcnTy *CuModuleGetFunctionFcnPtr;

typedef CUresult CUDAAPI CuDeviceComputeCapabilityFcnTy(int *, int *, CUdevice);
static CuDeviceComputeCapabilityFcnTy *CuDeviceComputeCapabilityFcnPtr;

typedef CUresult CUDAAPI CuDeviceGetNameFcnTy(char *, int, CUdevice);
static CuDeviceGetNameFcnTy *CuDeviceGetNameFcnPtr;

typedef CUresult CUDAAPI CuLinkAddDataFcnTy(CUlinkState state,
                                            CUjitInputType type, void *data,
                                            size_t size, const char *name,
                                            unsigned int numOptions,
                                            CUjit_option *options,
                                            void **optionValues);
static CuLinkAddDataFcnTy *CuLinkAddDataFcnPtr;

typedef CUresult CUDAAPI CuLinkCreateFcnTy(unsigned int numOptions,
                                           CUjit_option *options,
                                           void **optionValues,
                                           CUlinkState *stateOut);
static CuLinkCreateFcnTy *CuLinkCreateFcnPtr;

typedef CUresult CUDAAPI CuLinkCompleteFcnTy(CUlinkState state, void **cubinOut,
                                             size_t *sizeOut);
static CuLinkCompleteFcnTy *CuLinkCompleteFcnPtr;

typedef CUresult CUDAAPI CuLinkDestroyFcnTy(CUlinkState state);
static CuLinkDestroyFcnTy *CuLinkDestroyFcnPtr;

/* Type-defines of function pointer ot CUDA runtime APIs. */
typedef cudaError_t CUDARTAPI CudaThreadSynchronizeFcnTy(void);
static CudaThreadSynchronizeFcnTy *CudaThreadSynchronizeFcnPtr;

static void *getAPIHandle(void *Handle, const char *FuncName) {
  char *Err;
  void *FuncPtr;
  dlerror();
  FuncPtr = dlsym(Handle, FuncName);
  if ((Err = dlerror()) != 0) {
    fprintf(stdout, "Load CUDA driver API failed: %s. \n", Err);
    return 0;
  }
  return FuncPtr;
}

static int initialDeviceAPILibraries() {
  HandleCuda = dlopen("libcuda.so", RTLD_LAZY);
  if (!HandleCuda) {
    printf("Cannot open library: %s. \n", dlerror());
    return 0;
  }

  HandleCudaRT = dlopen("libcudart.so", RTLD_LAZY);
  if (!HandleCudaRT) {
    printf("Cannot open library: %s. \n", dlerror());
    return 0;
  }

  return 1;
}

static int initialDeviceAPIs() {
  if (initialDeviceAPILibraries() == 0)
    return 0;

  /* Get function pointer to CUDA Driver APIs.
   *
   * Note that compilers conforming to the ISO C standard are required to
   * generate a warning if a conversion from a void * pointer to a function
   * pointer is attempted as in the following statements. The warning
   * of this kind of cast may not be emitted by clang and new versions of gcc
   * as it is valid on POSIX 2008.
   */
  CuFuncSetBlockShapeFcnPtr = (CuFuncSetBlockShapeFcnTy *)getAPIHandle(
      HandleCuda, "cuFuncSetBlockShape");

  CuParamSetvFcnPtr =
      (CuParamSetvFcnTy *)getAPIHandle(HandleCuda, "cuParamSetv");

  CuParamSetSizeFcnPtr =
      (CuParamSetSizeFcnTy *)getAPIHandle(HandleCuda, "cuParamSetSize");

  CuLaunchGridFcnPtr =
      (CuLaunchGridFcnTy *)getAPIHandle(HandleCuda, "cuLaunchGrid");

  CuMemAllocFcnPtr =
      (CuMemAllocFcnTy *)getAPIHandle(HandleCuda, "cuMemAlloc_v2");

  CuMemFreeFcnPtr = (CuMemFreeFcnTy *)getAPIHandle(HandleCuda, "cuMemFree_v2");

  CuMemcpyDtoHFcnPtr =
      (CuMemcpyDtoHFcnTy *)getAPIHandle(HandleCuda, "cuMemcpyDtoH_v2");

  CuMemcpyHtoDFcnPtr =
      (CuMemcpyHtoDFcnTy *)getAPIHandle(HandleCuda, "cuMemcpyHtoD_v2");

  CuModuleUnloadFcnPtr =
      (CuModuleUnloadFcnTy *)getAPIHandle(HandleCuda, "cuModuleUnload");

  CuCtxDestroyFcnPtr =
      (CuCtxDestroyFcnTy *)getAPIHandle(HandleCuda, "cuCtxDestroy");

  CuInitFcnPtr = (CuInitFcnTy *)getAPIHandle(HandleCuda, "cuInit");

  CuDeviceGetCountFcnPtr =
      (CuDeviceGetCountFcnTy *)getAPIHandle(HandleCuda, "cuDeviceGetCount");

  CuDeviceGetFcnPtr =
      (CuDeviceGetFcnTy *)getAPIHandle(HandleCuda, "cuDeviceGet");

  CuCtxCreateFcnPtr =
      (CuCtxCreateFcnTy *)getAPIHandle(HandleCuda, "cuCtxCreate_v2");

  CuModuleLoadDataExFcnPtr =
      (CuModuleLoadDataExFcnTy *)getAPIHandle(HandleCuda, "cuModuleLoadDataEx");

  CuModuleLoadDataFcnPtr =
      (CuModuleLoadDataFcnTy *)getAPIHandle(HandleCuda, "cuModuleLoadData");

  CuModuleGetFunctionFcnPtr = (CuModuleGetFunctionFcnTy *)getAPIHandle(
      HandleCuda, "cuModuleGetFunction");

  CuDeviceComputeCapabilityFcnPtr =
      (CuDeviceComputeCapabilityFcnTy *)getAPIHandle(
          HandleCuda, "cuDeviceComputeCapability");

  CuDeviceGetNameFcnPtr =
      (CuDeviceGetNameFcnTy *)getAPIHandle(HandleCuda, "cuDeviceGetName");

  CuLinkAddDataFcnPtr =
      (CuLinkAddDataFcnTy *)getAPIHandle(HandleCuda, "cuLinkAddData");

  CuLinkCreateFcnPtr =
      (CuLinkCreateFcnTy *)getAPIHandle(HandleCuda, "cuLinkCreate");

  CuLinkCompleteFcnPtr =
      (CuLinkCompleteFcnTy *)getAPIHandle(HandleCuda, "cuLinkComplete");

  CuLinkDestroyFcnPtr =
      (CuLinkDestroyFcnTy *)getAPIHandle(HandleCuda, "cuLinkDestroy");

  /* Get function pointer to CUDA Runtime APIs. */
  CudaThreadSynchronizeFcnPtr = (CudaThreadSynchronizeFcnTy *)getAPIHandle(
      HandleCudaRT, "cudaThreadSynchronize");

  return 1;
}

PollyGPUContext *polly_initContext() {
  DebugMode = getenv("POLLY_DEBUG") != 0;

  dump_function();
  PollyGPUContext *Context;
  CUdevice Device;

  int Major = 0, Minor = 0, DeviceID = 0;
  char DeviceName[256];
  int DeviceCount = 0;

  /* Get API handles. */
  if (initialDeviceAPIs() == 0) {
    fprintf(stdout, "Getting the \"handle\" for the CUDA driver API failed.\n");
    exit(-1);
  }

  if (CuInitFcnPtr(0) != CUDA_SUCCESS) {
    fprintf(stdout, "Initializing the CUDA driver API failed.\n");
    exit(-1);
  }

  /* Get number of devices that supports CUDA. */
  CuDeviceGetCountFcnPtr(&DeviceCount);
  if (DeviceCount == 0) {
    fprintf(stdout, "There is no device supporting CUDA.\n");
    exit(-1);
  }

  CuDeviceGetFcnPtr(&Device, 0);

  /* Get compute capabilities and the device name. */
  CuDeviceComputeCapabilityFcnPtr(&Major, &Minor, Device);
  CuDeviceGetNameFcnPtr(DeviceName, 256, Device);
  debug_print("> Running on GPU device %d : %s.\n", DeviceID, DeviceName);

  /* Create context on the device. */
  Context = (PollyGPUContext *)malloc(sizeof(PollyGPUContext));
  if (Context == 0) {
    fprintf(stdout, "Allocate memory for Polly GPU context failed.\n");
    exit(-1);
  }
  CuCtxCreateFcnPtr(&(Context->Cuda), 0, Device);

  return Context;
}

PollyGPUFunction *polly_getKernel(const char *PTXBuffer,
                                  const char *KernelName) {
  dump_function();

  PollyGPUFunction *Function = malloc(sizeof(PollyGPUFunction));

  if (Function == 0) {
    fprintf(stdout, "Allocate memory for Polly GPU function failed.\n");
    exit(-1);
  }

  CUresult Res;
  CUlinkState LState;
  CUjit_option Options[6];
  void *OptionVals[6];
  float Walltime = 0;
  unsigned long LogSize = 8192;
  char ErrorLog[8192], InfoLog[8192];
  void *CuOut;
  size_t OutSize;

  // Setup linker options
  // Return walltime from JIT compilation
  Options[0] = CU_JIT_WALL_TIME;
  OptionVals[0] = (void *)&Walltime;
  // Pass a buffer for info messages
  Options[1] = CU_JIT_INFO_LOG_BUFFER;
  OptionVals[1] = (void *)InfoLog;
  // Pass the size of the info buffer
  Options[2] = CU_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
  OptionVals[2] = (void *)LogSize;
  // Pass a buffer for error message
  Options[3] = CU_JIT_ERROR_LOG_BUFFER;
  OptionVals[3] = (void *)ErrorLog;
  // Pass the size of the error buffer
  Options[4] = CU_JIT_ERROR_LOG_BUFFER_SIZE_BYTES;
  OptionVals[4] = (void *)LogSize;
  // Make the linker verbose
  Options[5] = CU_JIT_LOG_VERBOSE;
  OptionVals[5] = (void *)1;

  memset(ErrorLog, 0, sizeof(ErrorLog));

  CuLinkCreateFcnPtr(6, Options, OptionVals, &LState);
  Res = CuLinkAddDataFcnPtr(LState, CU_JIT_INPUT_PTX, (void *)PTXBuffer,
                            strlen(PTXBuffer) + 1, 0, 0, 0, 0);
  if (Res != CUDA_SUCCESS) {
    fprintf(stdout, "PTX Linker Error:\n%s\n%s", ErrorLog, InfoLog);
    exit(-1);
  }

  Res = CuLinkCompleteFcnPtr(LState, &CuOut, &OutSize);
  if (Res != CUDA_SUCCESS) {
    fprintf(stdout, "Complete ptx linker step failed.\n");
    fprintf(stdout, "\n%s\n", ErrorLog);
    exit(-1);
  }

  debug_print("CUDA Link Completed in %fms. Linker Output:\n%s\n", Walltime,
              InfoLog);

  Res = CuModuleLoadDataFcnPtr(&(Function->CudaModule), CuOut);
  if (Res != CUDA_SUCCESS) {
    fprintf(stdout, "Loading ptx assembly text failed.\n");
    exit(-1);
  }

  Res = CuModuleGetFunctionFcnPtr(&(Function->Cuda), Function->CudaModule,
                                  KernelName);
  if (Res != CUDA_SUCCESS) {
    fprintf(stdout, "Loading kernel function failed.\n");
    exit(-1);
  }

  CuLinkDestroyFcnPtr(LState);

  return Function;
}

void polly_freeKernel(PollyGPUFunction *Kernel) {
  dump_function();

  if (Kernel->CudaModule)
    CuModuleUnloadFcnPtr(Kernel->CudaModule);

  if (Kernel)
    free(Kernel);
}

void polly_copyFromHostToDevice(void *HostData, PollyGPUDevicePtr *DevData,
                                long MemSize) {
  dump_function();

  CUdeviceptr CuDevData = DevData->Cuda;
  CuMemcpyHtoDFcnPtr(CuDevData, HostData, MemSize);
}

void polly_copyFromDeviceToHost(PollyGPUDevicePtr *DevData, void *HostData,
                                long MemSize) {
  dump_function();

  if (CuMemcpyDtoHFcnPtr(HostData, DevData->Cuda, MemSize) != CUDA_SUCCESS) {
    fprintf(stdout, "Copying results from device to host memory failed.\n");
    exit(-1);
  }
}

void polly_setKernelParameters(PollyGPUFunction *Kernel, int BlockWidth,
                               int BlockHeight, PollyGPUDevicePtr *DevData) {
  dump_function();

  int ParamOffset = 0;

  CuFuncSetBlockShapeFcnPtr(Kernel->Cuda, BlockWidth, BlockHeight, 1);
  CuParamSetvFcnPtr(Kernel->Cuda, ParamOffset, &(DevData->Cuda),
                    sizeof(DevData->Cuda));
  ParamOffset += sizeof(DevData->Cuda);
  CuParamSetSizeFcnPtr(Kernel->Cuda, ParamOffset);
}

void polly_launchKernel(PollyGPUFunction *Kernel, int GridWidth,
                        int GridHeight) {
  dump_function();

  if (CuLaunchGridFcnPtr(Kernel->Cuda, GridWidth, GridHeight) != CUDA_SUCCESS) {
    fprintf(stdout, "Launching CUDA kernel failed.\n");
    exit(-1);
  }
  CudaThreadSynchronizeFcnPtr();
  debug_print("CUDA kernel launched.\n");
}

void polly_freeDeviceMemory(PollyGPUDevicePtr *Allocation) {
  dump_function();
  CuMemFreeFcnPtr((CUdeviceptr)Allocation->Cuda);
  free(Allocation);
}

PollyGPUDevicePtr *polly_allocateMemoryForDevice(long MemSize) {
  dump_function();

  PollyGPUDevicePtr *DevData = malloc(sizeof(PollyGPUDevicePtr));

  if (DevData == 0) {
    fprintf(stdout, "Allocate memory for GPU device memory pointer failed.\n");
    exit(-1);
  }

  CUresult Res = CuMemAllocFcnPtr(&(DevData->Cuda), MemSize);

  if (Res != CUDA_SUCCESS) {
    fprintf(stdout, "Allocate memory for GPU device memory pointer failed.\n");
    exit(-1);
  }

  return DevData;
}

void polly_freeContext(PollyGPUContext *Context) {
  dump_function();

  if (Context->Cuda) {
    CuCtxDestroyFcnPtr(Context->Cuda);
    free(Context);
  }

  dlclose(HandleCuda);
  dlclose(HandleCudaRT);
}
