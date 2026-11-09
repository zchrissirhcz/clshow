// opencl_shim_apis.cpp —— 由 tools/gen_opencl_shim.py 自动生成，勿手改
// 基于 OpenCL 3.1 头文件，共 104 个标准 cl API
#include "opencl_shim_apis.h"

#ifdef _WIN32
#include <windows.h>
#define SHIM_DLSYM(h, s) ((void*)GetProcAddress((HMODULE)(h), (s)))
#else
#include <dlfcn.h>
#define SHIM_DLSYM(h, s) dlsym((h), (s))
#endif

// 定义函数指针
cl_int (*p_clGetPlatformIDs)(cl_uint num_entries, cl_platform_id * platforms, cl_uint * num_platforms) = nullptr;
cl_int (*p_clGetPlatformInfo)(cl_platform_id platform, cl_platform_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_int (*p_clGetDeviceIDs)(cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id * devices, cl_uint * num_devices) = nullptr;
cl_int (*p_clGetDeviceInfo)(cl_device_id device, cl_device_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_int (*p_clCreateSubDevices)(cl_device_id in_device, const cl_device_partition_property * properties, cl_uint num_devices, cl_device_id * out_devices, cl_uint * num_devices_ret) = nullptr;
cl_int (*p_clRetainDevice)(cl_device_id device) = nullptr;
cl_int (*p_clReleaseDevice)(cl_device_id device) = nullptr;
cl_int (*p_clSetDefaultDeviceCommandQueue)(cl_context context, cl_device_id device, cl_command_queue command_queue) = nullptr;
cl_int (*p_clGetDeviceAndHostTimer)(cl_device_id device, cl_ulong* device_timestamp, cl_ulong* host_timestamp) = nullptr;
cl_int (*p_clGetHostTimer)(cl_device_id device, cl_ulong * host_timestamp) = nullptr;
cl_int (*p_clRetainContext)(cl_context context) = nullptr;
cl_int (*p_clReleaseContext)(cl_context context) = nullptr;
cl_int (*p_clGetContextInfo)(cl_context context, cl_context_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_command_queue (*p_clCreateCommandQueueWithProperties)(cl_context context, cl_device_id device, const cl_queue_properties * properties, cl_int * errcode_ret) = nullptr;
cl_int (*p_clRetainCommandQueue)(cl_command_queue command_queue) = nullptr;
cl_int (*p_clReleaseCommandQueue)(cl_command_queue command_queue) = nullptr;
cl_int (*p_clGetCommandQueueInfo)(cl_command_queue command_queue, cl_command_queue_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_mem (*p_clCreateBuffer)(cl_context context, cl_mem_flags flags, size_t size, void * host_ptr, cl_int * errcode_ret) = nullptr;
cl_mem (*p_clCreateSubBuffer)(cl_mem buffer, cl_mem_flags flags, cl_buffer_create_type buffer_create_type, const void * buffer_create_info, cl_int * errcode_ret) = nullptr;
cl_mem (*p_clCreateImage)(cl_context context, cl_mem_flags flags, const cl_image_format * image_format, const cl_image_desc * image_desc, void * host_ptr, cl_int * errcode_ret) = nullptr;
cl_mem (*p_clCreatePipe)(cl_context context, cl_mem_flags flags, cl_uint pipe_packet_size, cl_uint pipe_max_packets, const cl_pipe_properties * properties, cl_int * errcode_ret) = nullptr;
cl_mem (*p_clCreateBufferWithProperties)(cl_context context, const cl_mem_properties * properties, cl_mem_flags flags, size_t size, void * host_ptr, cl_int * errcode_ret) = nullptr;
cl_mem (*p_clCreateImageWithProperties)(cl_context context, const cl_mem_properties * properties, cl_mem_flags flags, const cl_image_format * image_format, const cl_image_desc * image_desc, void * host_ptr, cl_int * errcode_ret) = nullptr;
cl_int (*p_clRetainMemObject)(cl_mem memobj) = nullptr;
cl_int (*p_clReleaseMemObject)(cl_mem memobj) = nullptr;
cl_int (*p_clGetSupportedImageFormats)(cl_context context, cl_mem_flags flags, cl_mem_object_type image_type, cl_uint num_entries, cl_image_format * image_formats, cl_uint * num_image_formats) = nullptr;
cl_int (*p_clGetMemObjectInfo)(cl_mem memobj, cl_mem_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_int (*p_clGetImageInfo)(cl_mem image, cl_image_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_int (*p_clGetPipeInfo)(cl_mem pipe, cl_pipe_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
void * (*p_clSVMAlloc)(cl_context context, cl_svm_mem_flags flags, size_t size, cl_uint alignment) = nullptr;
void (*p_clSVMFree)(cl_context context, void * svm_pointer) = nullptr;
cl_sampler (*p_clCreateSamplerWithProperties)(cl_context context, const cl_sampler_properties * sampler_properties, cl_int * errcode_ret) = nullptr;
cl_int (*p_clRetainSampler)(cl_sampler sampler) = nullptr;
cl_int (*p_clReleaseSampler)(cl_sampler sampler) = nullptr;
cl_int (*p_clGetSamplerInfo)(cl_sampler sampler, cl_sampler_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_program (*p_clCreateProgramWithSource)(cl_context context, cl_uint count, const char ** strings, const size_t * lengths, cl_int * errcode_ret) = nullptr;
cl_program (*p_clCreateProgramWithBinary)(cl_context context, cl_uint num_devices, const cl_device_id * device_list, const size_t * lengths, const unsigned char ** binaries, cl_int * binary_status, cl_int * errcode_ret) = nullptr;
cl_program (*p_clCreateProgramWithBuiltInKernels)(cl_context context, cl_uint num_devices, const cl_device_id * device_list, const char * kernel_names, cl_int * errcode_ret) = nullptr;
cl_program (*p_clCreateProgramWithIL)(cl_context context, const void* il, size_t length, cl_int* errcode_ret) = nullptr;
cl_int (*p_clRetainProgram)(cl_program program) = nullptr;
cl_int (*p_clReleaseProgram)(cl_program program) = nullptr;
cl_int (*p_clSetProgramSpecializationConstant)(cl_program program, cl_uint spec_id, size_t spec_size, const void* spec_value) = nullptr;
cl_int (*p_clUnloadPlatformCompiler)(cl_platform_id platform) = nullptr;
cl_int (*p_clGetProgramInfo)(cl_program program, cl_program_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_int (*p_clGetProgramBuildInfo)(cl_program program, cl_device_id device, cl_program_build_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_kernel (*p_clCreateKernel)(cl_program program, const char * kernel_name, cl_int * errcode_ret) = nullptr;
cl_int (*p_clCreateKernelsInProgram)(cl_program program, cl_uint num_kernels, cl_kernel * kernels, cl_uint * num_kernels_ret) = nullptr;
cl_kernel (*p_clCloneKernel)(cl_kernel source_kernel, cl_int* errcode_ret) = nullptr;
cl_int (*p_clRetainKernel)(cl_kernel kernel) = nullptr;
cl_int (*p_clReleaseKernel)(cl_kernel kernel) = nullptr;
cl_int (*p_clSetKernelArg)(cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void * arg_value) = nullptr;
cl_int (*p_clSetKernelArgSVMPointer)(cl_kernel kernel, cl_uint arg_index, const void * arg_value) = nullptr;
cl_int (*p_clSetKernelExecInfo)(cl_kernel kernel, cl_kernel_exec_info param_name, size_t param_value_size, const void * param_value) = nullptr;
cl_int (*p_clGetKernelInfo)(cl_kernel kernel, cl_kernel_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_int (*p_clGetKernelArgInfo)(cl_kernel kernel, cl_uint arg_indx, cl_kernel_arg_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_int (*p_clGetKernelWorkGroupInfo)(cl_kernel kernel, cl_device_id device, cl_kernel_work_group_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_int (*p_clGetKernelSubGroupInfo)(cl_kernel kernel, cl_device_id device, cl_kernel_sub_group_info param_name, size_t input_value_size, const void* input_value, size_t param_value_size, void* param_value, size_t* param_value_size_ret) = nullptr;
cl_int (*p_clWaitForEvents)(cl_uint num_events, const cl_event * event_list) = nullptr;
cl_int (*p_clGetEventInfo)(cl_event event, cl_event_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_event (*p_clCreateUserEvent)(cl_context context, cl_int * errcode_ret) = nullptr;
cl_int (*p_clRetainEvent)(cl_event event) = nullptr;
cl_int (*p_clReleaseEvent)(cl_event event) = nullptr;
cl_int (*p_clSetUserEventStatus)(cl_event event, cl_int execution_status) = nullptr;
cl_int (*p_clGetEventProfilingInfo)(cl_event event, cl_profiling_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret) = nullptr;
cl_int (*p_clFlush)(cl_command_queue command_queue) = nullptr;
cl_int (*p_clFinish)(cl_command_queue command_queue) = nullptr;
cl_int (*p_clEnqueueReadBuffer)(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read, size_t offset, size_t size, void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueReadBufferRect)(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read, const size_t * buffer_origin, const size_t * host_origin, const size_t * region, size_t buffer_row_pitch, size_t buffer_slice_pitch, size_t host_row_pitch, size_t host_slice_pitch, void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueWriteBuffer)(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write, size_t offset, size_t size, const void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueWriteBufferRect)(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write, const size_t * buffer_origin, const size_t * host_origin, const size_t * region, size_t buffer_row_pitch, size_t buffer_slice_pitch, size_t host_row_pitch, size_t host_slice_pitch, const void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueFillBuffer)(cl_command_queue command_queue, cl_mem buffer, const void * pattern, size_t pattern_size, size_t offset, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueCopyBuffer)(cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer, size_t src_offset, size_t dst_offset, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueCopyBufferRect)(cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer, const size_t * src_origin, const size_t * dst_origin, const size_t * region, size_t src_row_pitch, size_t src_slice_pitch, size_t dst_row_pitch, size_t dst_slice_pitch, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueReadImage)(cl_command_queue command_queue, cl_mem image, cl_bool blocking_read, const size_t * origin, const size_t * region, size_t row_pitch, size_t slice_pitch, void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueWriteImage)(cl_command_queue command_queue, cl_mem image, cl_bool blocking_write, const size_t * origin, const size_t * region, size_t input_row_pitch, size_t input_slice_pitch, const void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueFillImage)(cl_command_queue command_queue, cl_mem image, const void * fill_color, const size_t * origin, const size_t * region, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueCopyImage)(cl_command_queue command_queue, cl_mem src_image, cl_mem dst_image, const size_t * src_origin, const size_t * dst_origin, const size_t * region, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueCopyImageToBuffer)(cl_command_queue command_queue, cl_mem src_image, cl_mem dst_buffer, const size_t * src_origin, const size_t * region, size_t dst_offset, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueCopyBufferToImage)(cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_image, size_t src_offset, const size_t * dst_origin, const size_t * region, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
void * (*p_clEnqueueMapBuffer)(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_map, cl_map_flags map_flags, size_t offset, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event, cl_int * errcode_ret) = nullptr;
void * (*p_clEnqueueMapImage)(cl_command_queue command_queue, cl_mem image, cl_bool blocking_map, cl_map_flags map_flags, const size_t * origin, const size_t * region, size_t * image_row_pitch, size_t * image_slice_pitch, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event, cl_int * errcode_ret) = nullptr;
cl_int (*p_clEnqueueUnmapMemObject)(cl_command_queue command_queue, cl_mem memobj, void * mapped_ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueMigrateMemObjects)(cl_command_queue command_queue, cl_uint num_mem_objects, const cl_mem * mem_objects, cl_mem_migration_flags flags, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueNDRangeKernel)(cl_command_queue command_queue, cl_kernel kernel, cl_uint work_dim, const size_t * global_work_offset, const size_t * global_work_size, const size_t * local_work_size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueMarkerWithWaitList)(cl_command_queue command_queue, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueBarrierWithWaitList)(cl_command_queue command_queue, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueSVMMemcpy)(cl_command_queue command_queue, cl_bool blocking_copy, void * dst_ptr, const void * src_ptr, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueSVMMemFill)(cl_command_queue command_queue, void * svm_ptr, const void * pattern, size_t pattern_size, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueSVMMap)(cl_command_queue command_queue, cl_bool blocking_map, cl_map_flags flags, void * svm_ptr, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueSVMUnmap)(cl_command_queue command_queue, void * svm_ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clEnqueueSVMMigrateMem)(cl_command_queue command_queue, cl_uint num_svm_pointers, const void ** svm_pointers, const size_t * sizes, cl_mem_migration_flags flags, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;
cl_int (*p_clGetKernelSuggestedLocalWorkSize)(cl_command_queue command_queue, cl_kernel kernel, cl_uint work_dim, const size_t * global_work_offset, const size_t * global_work_size, size_t * suggested_local_work_size) = nullptr;
void * (*p_clGetExtensionFunctionAddressForPlatform)(cl_platform_id platform, const char * func_name) = nullptr;
cl_int (*p_clSetCommandQueueProperty)(cl_command_queue command_queue, cl_command_queue_properties properties, cl_bool enable, cl_command_queue_properties * old_properties) = nullptr;
cl_mem (*p_clCreateImage2D)(cl_context context, cl_mem_flags flags, const cl_image_format * image_format, size_t image_width, size_t image_height, size_t image_row_pitch, void * host_ptr, cl_int * errcode_ret) = nullptr;
cl_mem (*p_clCreateImage3D)(cl_context context, cl_mem_flags flags, const cl_image_format * image_format, size_t image_width, size_t image_height, size_t image_depth, size_t image_row_pitch, size_t image_slice_pitch, void * host_ptr, cl_int * errcode_ret) = nullptr;
cl_int (*p_clEnqueueMarker)(cl_command_queue command_queue, cl_event * event) = nullptr;
cl_int (*p_clEnqueueWaitForEvents)(cl_command_queue command_queue, cl_uint num_events, const cl_event * event_list) = nullptr;
cl_int (*p_clEnqueueBarrier)(cl_command_queue command_queue) = nullptr;
cl_int (*p_clUnloadCompiler)(void) = nullptr;
void * (*p_clGetExtensionFunctionAddress)(const char * func_name) = nullptr;
cl_command_queue (*p_clCreateCommandQueue)(cl_context context, cl_device_id device, cl_command_queue_properties properties, cl_int * errcode_ret) = nullptr;
cl_sampler (*p_clCreateSampler)(cl_context context, cl_bool normalized_coords, cl_addressing_mode addressing_mode, cl_filter_mode filter_mode, cl_int * errcode_ret) = nullptr;
cl_int (*p_clEnqueueTask)(cl_command_queue command_queue, cl_kernel kernel, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event) = nullptr;

unsigned opencl_shim_export(void* handle)
{
    if (!handle) return 0;
    unsigned n = 0;
    struct { const char* name; void** p; } tab[] = {
        { "clGetPlatformIDs", (void**)&p_clGetPlatformIDs },
        { "clGetPlatformInfo", (void**)&p_clGetPlatformInfo },
        { "clGetDeviceIDs", (void**)&p_clGetDeviceIDs },
        { "clGetDeviceInfo", (void**)&p_clGetDeviceInfo },
        { "clCreateSubDevices", (void**)&p_clCreateSubDevices },
        { "clRetainDevice", (void**)&p_clRetainDevice },
        { "clReleaseDevice", (void**)&p_clReleaseDevice },
        { "clSetDefaultDeviceCommandQueue", (void**)&p_clSetDefaultDeviceCommandQueue },
        { "clGetDeviceAndHostTimer", (void**)&p_clGetDeviceAndHostTimer },
        { "clGetHostTimer", (void**)&p_clGetHostTimer },
        { "clRetainContext", (void**)&p_clRetainContext },
        { "clReleaseContext", (void**)&p_clReleaseContext },
        { "clGetContextInfo", (void**)&p_clGetContextInfo },
        { "clCreateCommandQueueWithProperties", (void**)&p_clCreateCommandQueueWithProperties },
        { "clRetainCommandQueue", (void**)&p_clRetainCommandQueue },
        { "clReleaseCommandQueue", (void**)&p_clReleaseCommandQueue },
        { "clGetCommandQueueInfo", (void**)&p_clGetCommandQueueInfo },
        { "clCreateBuffer", (void**)&p_clCreateBuffer },
        { "clCreateSubBuffer", (void**)&p_clCreateSubBuffer },
        { "clCreateImage", (void**)&p_clCreateImage },
        { "clCreatePipe", (void**)&p_clCreatePipe },
        { "clCreateBufferWithProperties", (void**)&p_clCreateBufferWithProperties },
        { "clCreateImageWithProperties", (void**)&p_clCreateImageWithProperties },
        { "clRetainMemObject", (void**)&p_clRetainMemObject },
        { "clReleaseMemObject", (void**)&p_clReleaseMemObject },
        { "clGetSupportedImageFormats", (void**)&p_clGetSupportedImageFormats },
        { "clGetMemObjectInfo", (void**)&p_clGetMemObjectInfo },
        { "clGetImageInfo", (void**)&p_clGetImageInfo },
        { "clGetPipeInfo", (void**)&p_clGetPipeInfo },
        { "clSVMAlloc", (void**)&p_clSVMAlloc },
        { "clSVMFree", (void**)&p_clSVMFree },
        { "clCreateSamplerWithProperties", (void**)&p_clCreateSamplerWithProperties },
        { "clRetainSampler", (void**)&p_clRetainSampler },
        { "clReleaseSampler", (void**)&p_clReleaseSampler },
        { "clGetSamplerInfo", (void**)&p_clGetSamplerInfo },
        { "clCreateProgramWithSource", (void**)&p_clCreateProgramWithSource },
        { "clCreateProgramWithBinary", (void**)&p_clCreateProgramWithBinary },
        { "clCreateProgramWithBuiltInKernels", (void**)&p_clCreateProgramWithBuiltInKernels },
        { "clCreateProgramWithIL", (void**)&p_clCreateProgramWithIL },
        { "clRetainProgram", (void**)&p_clRetainProgram },
        { "clReleaseProgram", (void**)&p_clReleaseProgram },
        { "clSetProgramSpecializationConstant", (void**)&p_clSetProgramSpecializationConstant },
        { "clUnloadPlatformCompiler", (void**)&p_clUnloadPlatformCompiler },
        { "clGetProgramInfo", (void**)&p_clGetProgramInfo },
        { "clGetProgramBuildInfo", (void**)&p_clGetProgramBuildInfo },
        { "clCreateKernel", (void**)&p_clCreateKernel },
        { "clCreateKernelsInProgram", (void**)&p_clCreateKernelsInProgram },
        { "clCloneKernel", (void**)&p_clCloneKernel },
        { "clRetainKernel", (void**)&p_clRetainKernel },
        { "clReleaseKernel", (void**)&p_clReleaseKernel },
        { "clSetKernelArg", (void**)&p_clSetKernelArg },
        { "clSetKernelArgSVMPointer", (void**)&p_clSetKernelArgSVMPointer },
        { "clSetKernelExecInfo", (void**)&p_clSetKernelExecInfo },
        { "clGetKernelInfo", (void**)&p_clGetKernelInfo },
        { "clGetKernelArgInfo", (void**)&p_clGetKernelArgInfo },
        { "clGetKernelWorkGroupInfo", (void**)&p_clGetKernelWorkGroupInfo },
        { "clGetKernelSubGroupInfo", (void**)&p_clGetKernelSubGroupInfo },
        { "clWaitForEvents", (void**)&p_clWaitForEvents },
        { "clGetEventInfo", (void**)&p_clGetEventInfo },
        { "clCreateUserEvent", (void**)&p_clCreateUserEvent },
        { "clRetainEvent", (void**)&p_clRetainEvent },
        { "clReleaseEvent", (void**)&p_clReleaseEvent },
        { "clSetUserEventStatus", (void**)&p_clSetUserEventStatus },
        { "clGetEventProfilingInfo", (void**)&p_clGetEventProfilingInfo },
        { "clFlush", (void**)&p_clFlush },
        { "clFinish", (void**)&p_clFinish },
        { "clEnqueueReadBuffer", (void**)&p_clEnqueueReadBuffer },
        { "clEnqueueReadBufferRect", (void**)&p_clEnqueueReadBufferRect },
        { "clEnqueueWriteBuffer", (void**)&p_clEnqueueWriteBuffer },
        { "clEnqueueWriteBufferRect", (void**)&p_clEnqueueWriteBufferRect },
        { "clEnqueueFillBuffer", (void**)&p_clEnqueueFillBuffer },
        { "clEnqueueCopyBuffer", (void**)&p_clEnqueueCopyBuffer },
        { "clEnqueueCopyBufferRect", (void**)&p_clEnqueueCopyBufferRect },
        { "clEnqueueReadImage", (void**)&p_clEnqueueReadImage },
        { "clEnqueueWriteImage", (void**)&p_clEnqueueWriteImage },
        { "clEnqueueFillImage", (void**)&p_clEnqueueFillImage },
        { "clEnqueueCopyImage", (void**)&p_clEnqueueCopyImage },
        { "clEnqueueCopyImageToBuffer", (void**)&p_clEnqueueCopyImageToBuffer },
        { "clEnqueueCopyBufferToImage", (void**)&p_clEnqueueCopyBufferToImage },
        { "clEnqueueMapBuffer", (void**)&p_clEnqueueMapBuffer },
        { "clEnqueueMapImage", (void**)&p_clEnqueueMapImage },
        { "clEnqueueUnmapMemObject", (void**)&p_clEnqueueUnmapMemObject },
        { "clEnqueueMigrateMemObjects", (void**)&p_clEnqueueMigrateMemObjects },
        { "clEnqueueNDRangeKernel", (void**)&p_clEnqueueNDRangeKernel },
        { "clEnqueueMarkerWithWaitList", (void**)&p_clEnqueueMarkerWithWaitList },
        { "clEnqueueBarrierWithWaitList", (void**)&p_clEnqueueBarrierWithWaitList },
        { "clEnqueueSVMMemcpy", (void**)&p_clEnqueueSVMMemcpy },
        { "clEnqueueSVMMemFill", (void**)&p_clEnqueueSVMMemFill },
        { "clEnqueueSVMMap", (void**)&p_clEnqueueSVMMap },
        { "clEnqueueSVMUnmap", (void**)&p_clEnqueueSVMUnmap },
        { "clEnqueueSVMMigrateMem", (void**)&p_clEnqueueSVMMigrateMem },
        { "clGetKernelSuggestedLocalWorkSize", (void**)&p_clGetKernelSuggestedLocalWorkSize },
        { "clGetExtensionFunctionAddressForPlatform", (void**)&p_clGetExtensionFunctionAddressForPlatform },
        { "clSetCommandQueueProperty", (void**)&p_clSetCommandQueueProperty },
        { "clCreateImage2D", (void**)&p_clCreateImage2D },
        { "clCreateImage3D", (void**)&p_clCreateImage3D },
        { "clEnqueueMarker", (void**)&p_clEnqueueMarker },
        { "clEnqueueWaitForEvents", (void**)&p_clEnqueueWaitForEvents },
        { "clEnqueueBarrier", (void**)&p_clEnqueueBarrier },
        { "clUnloadCompiler", (void**)&p_clUnloadCompiler },
        { "clGetExtensionFunctionAddress", (void**)&p_clGetExtensionFunctionAddress },
        { "clCreateCommandQueue", (void**)&p_clCreateCommandQueue },
        { "clCreateSampler", (void**)&p_clCreateSampler },
        { "clEnqueueTask", (void**)&p_clEnqueueTask },
    };
    for (unsigned i = 0; i < sizeof(tab)/sizeof(tab[0]); i++)
    {
        void* sym = SHIM_DLSYM(handle, tab[i].name);
        if (sym) { *tab[i].p = sym; n++; }
    }
    return n;
}
