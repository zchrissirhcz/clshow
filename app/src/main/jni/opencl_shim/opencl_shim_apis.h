// opencl_shim_apis.h —— 由 tools/gen_opencl_shim.py 自动生成，勿手改
// 基于 OpenCL 3.1 头文件，共 104 个标准 cl API
#pragma once
#include <CL/cl.h>

extern "C" {

// 每个标准 cl API 的函数指针
extern cl_int (*p_clGetPlatformIDs)(cl_uint num_entries, cl_platform_id * platforms, cl_uint * num_platforms);
extern cl_int (*p_clGetPlatformInfo)(cl_platform_id platform, cl_platform_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_int (*p_clGetDeviceIDs)(cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id * devices, cl_uint * num_devices);
extern cl_int (*p_clGetDeviceInfo)(cl_device_id device, cl_device_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_int (*p_clCreateSubDevices)(cl_device_id in_device, const cl_device_partition_property * properties, cl_uint num_devices, cl_device_id * out_devices, cl_uint * num_devices_ret);
extern cl_int (*p_clRetainDevice)(cl_device_id device);
extern cl_int (*p_clReleaseDevice)(cl_device_id device);
extern cl_int (*p_clSetDefaultDeviceCommandQueue)(cl_context context, cl_device_id device, cl_command_queue command_queue);
extern cl_int (*p_clGetDeviceAndHostTimer)(cl_device_id device, cl_ulong* device_timestamp, cl_ulong* host_timestamp);
extern cl_int (*p_clGetHostTimer)(cl_device_id device, cl_ulong * host_timestamp);
extern cl_int (*p_clRetainContext)(cl_context context);
extern cl_int (*p_clReleaseContext)(cl_context context);
extern cl_int (*p_clGetContextInfo)(cl_context context, cl_context_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_command_queue (*p_clCreateCommandQueueWithProperties)(cl_context context, cl_device_id device, const cl_queue_properties * properties, cl_int * errcode_ret);
extern cl_int (*p_clRetainCommandQueue)(cl_command_queue command_queue);
extern cl_int (*p_clReleaseCommandQueue)(cl_command_queue command_queue);
extern cl_int (*p_clGetCommandQueueInfo)(cl_command_queue command_queue, cl_command_queue_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_mem (*p_clCreateBuffer)(cl_context context, cl_mem_flags flags, size_t size, void * host_ptr, cl_int * errcode_ret);
extern cl_mem (*p_clCreateSubBuffer)(cl_mem buffer, cl_mem_flags flags, cl_buffer_create_type buffer_create_type, const void * buffer_create_info, cl_int * errcode_ret);
extern cl_mem (*p_clCreateImage)(cl_context context, cl_mem_flags flags, const cl_image_format * image_format, const cl_image_desc * image_desc, void * host_ptr, cl_int * errcode_ret);
extern cl_mem (*p_clCreatePipe)(cl_context context, cl_mem_flags flags, cl_uint pipe_packet_size, cl_uint pipe_max_packets, const cl_pipe_properties * properties, cl_int * errcode_ret);
extern cl_mem (*p_clCreateBufferWithProperties)(cl_context context, const cl_mem_properties * properties, cl_mem_flags flags, size_t size, void * host_ptr, cl_int * errcode_ret);
extern cl_mem (*p_clCreateImageWithProperties)(cl_context context, const cl_mem_properties * properties, cl_mem_flags flags, const cl_image_format * image_format, const cl_image_desc * image_desc, void * host_ptr, cl_int * errcode_ret);
extern cl_int (*p_clRetainMemObject)(cl_mem memobj);
extern cl_int (*p_clReleaseMemObject)(cl_mem memobj);
extern cl_int (*p_clGetSupportedImageFormats)(cl_context context, cl_mem_flags flags, cl_mem_object_type image_type, cl_uint num_entries, cl_image_format * image_formats, cl_uint * num_image_formats);
extern cl_int (*p_clGetMemObjectInfo)(cl_mem memobj, cl_mem_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_int (*p_clGetImageInfo)(cl_mem image, cl_image_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_int (*p_clGetPipeInfo)(cl_mem pipe, cl_pipe_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern void * (*p_clSVMAlloc)(cl_context context, cl_svm_mem_flags flags, size_t size, cl_uint alignment);
extern void (*p_clSVMFree)(cl_context context, void * svm_pointer);
extern cl_sampler (*p_clCreateSamplerWithProperties)(cl_context context, const cl_sampler_properties * sampler_properties, cl_int * errcode_ret);
extern cl_int (*p_clRetainSampler)(cl_sampler sampler);
extern cl_int (*p_clReleaseSampler)(cl_sampler sampler);
extern cl_int (*p_clGetSamplerInfo)(cl_sampler sampler, cl_sampler_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_program (*p_clCreateProgramWithSource)(cl_context context, cl_uint count, const char ** strings, const size_t * lengths, cl_int * errcode_ret);
extern cl_program (*p_clCreateProgramWithBinary)(cl_context context, cl_uint num_devices, const cl_device_id * device_list, const size_t * lengths, const unsigned char ** binaries, cl_int * binary_status, cl_int * errcode_ret);
extern cl_program (*p_clCreateProgramWithBuiltInKernels)(cl_context context, cl_uint num_devices, const cl_device_id * device_list, const char * kernel_names, cl_int * errcode_ret);
extern cl_program (*p_clCreateProgramWithIL)(cl_context context, const void* il, size_t length, cl_int* errcode_ret);
extern cl_int (*p_clRetainProgram)(cl_program program);
extern cl_int (*p_clReleaseProgram)(cl_program program);
extern cl_int (*p_clSetProgramSpecializationConstant)(cl_program program, cl_uint spec_id, size_t spec_size, const void* spec_value);
extern cl_int (*p_clUnloadPlatformCompiler)(cl_platform_id platform);
extern cl_int (*p_clGetProgramInfo)(cl_program program, cl_program_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_int (*p_clGetProgramBuildInfo)(cl_program program, cl_device_id device, cl_program_build_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_kernel (*p_clCreateKernel)(cl_program program, const char * kernel_name, cl_int * errcode_ret);
extern cl_int (*p_clCreateKernelsInProgram)(cl_program program, cl_uint num_kernels, cl_kernel * kernels, cl_uint * num_kernels_ret);
extern cl_kernel (*p_clCloneKernel)(cl_kernel source_kernel, cl_int* errcode_ret);
extern cl_int (*p_clRetainKernel)(cl_kernel kernel);
extern cl_int (*p_clReleaseKernel)(cl_kernel kernel);
extern cl_int (*p_clSetKernelArg)(cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void * arg_value);
extern cl_int (*p_clSetKernelArgSVMPointer)(cl_kernel kernel, cl_uint arg_index, const void * arg_value);
extern cl_int (*p_clSetKernelExecInfo)(cl_kernel kernel, cl_kernel_exec_info param_name, size_t param_value_size, const void * param_value);
extern cl_int (*p_clGetKernelInfo)(cl_kernel kernel, cl_kernel_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_int (*p_clGetKernelArgInfo)(cl_kernel kernel, cl_uint arg_indx, cl_kernel_arg_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_int (*p_clGetKernelWorkGroupInfo)(cl_kernel kernel, cl_device_id device, cl_kernel_work_group_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_int (*p_clGetKernelSubGroupInfo)(cl_kernel kernel, cl_device_id device, cl_kernel_sub_group_info param_name, size_t input_value_size, const void* input_value, size_t param_value_size, void* param_value, size_t* param_value_size_ret);
extern cl_int (*p_clWaitForEvents)(cl_uint num_events, const cl_event * event_list);
extern cl_int (*p_clGetEventInfo)(cl_event event, cl_event_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_event (*p_clCreateUserEvent)(cl_context context, cl_int * errcode_ret);
extern cl_int (*p_clRetainEvent)(cl_event event);
extern cl_int (*p_clReleaseEvent)(cl_event event);
extern cl_int (*p_clSetUserEventStatus)(cl_event event, cl_int execution_status);
extern cl_int (*p_clGetEventProfilingInfo)(cl_event event, cl_profiling_info param_name, size_t param_value_size, void * param_value, size_t * param_value_size_ret);
extern cl_int (*p_clFlush)(cl_command_queue command_queue);
extern cl_int (*p_clFinish)(cl_command_queue command_queue);
extern cl_int (*p_clEnqueueReadBuffer)(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read, size_t offset, size_t size, void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueReadBufferRect)(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_read, const size_t * buffer_origin, const size_t * host_origin, const size_t * region, size_t buffer_row_pitch, size_t buffer_slice_pitch, size_t host_row_pitch, size_t host_slice_pitch, void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueWriteBuffer)(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write, size_t offset, size_t size, const void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueWriteBufferRect)(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_write, const size_t * buffer_origin, const size_t * host_origin, const size_t * region, size_t buffer_row_pitch, size_t buffer_slice_pitch, size_t host_row_pitch, size_t host_slice_pitch, const void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueFillBuffer)(cl_command_queue command_queue, cl_mem buffer, const void * pattern, size_t pattern_size, size_t offset, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueCopyBuffer)(cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer, size_t src_offset, size_t dst_offset, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueCopyBufferRect)(cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_buffer, const size_t * src_origin, const size_t * dst_origin, const size_t * region, size_t src_row_pitch, size_t src_slice_pitch, size_t dst_row_pitch, size_t dst_slice_pitch, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueReadImage)(cl_command_queue command_queue, cl_mem image, cl_bool blocking_read, const size_t * origin, const size_t * region, size_t row_pitch, size_t slice_pitch, void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueWriteImage)(cl_command_queue command_queue, cl_mem image, cl_bool blocking_write, const size_t * origin, const size_t * region, size_t input_row_pitch, size_t input_slice_pitch, const void * ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueFillImage)(cl_command_queue command_queue, cl_mem image, const void * fill_color, const size_t * origin, const size_t * region, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueCopyImage)(cl_command_queue command_queue, cl_mem src_image, cl_mem dst_image, const size_t * src_origin, const size_t * dst_origin, const size_t * region, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueCopyImageToBuffer)(cl_command_queue command_queue, cl_mem src_image, cl_mem dst_buffer, const size_t * src_origin, const size_t * region, size_t dst_offset, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueCopyBufferToImage)(cl_command_queue command_queue, cl_mem src_buffer, cl_mem dst_image, size_t src_offset, const size_t * dst_origin, const size_t * region, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern void * (*p_clEnqueueMapBuffer)(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking_map, cl_map_flags map_flags, size_t offset, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event, cl_int * errcode_ret);
extern void * (*p_clEnqueueMapImage)(cl_command_queue command_queue, cl_mem image, cl_bool blocking_map, cl_map_flags map_flags, const size_t * origin, const size_t * region, size_t * image_row_pitch, size_t * image_slice_pitch, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event, cl_int * errcode_ret);
extern cl_int (*p_clEnqueueUnmapMemObject)(cl_command_queue command_queue, cl_mem memobj, void * mapped_ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueMigrateMemObjects)(cl_command_queue command_queue, cl_uint num_mem_objects, const cl_mem * mem_objects, cl_mem_migration_flags flags, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueNDRangeKernel)(cl_command_queue command_queue, cl_kernel kernel, cl_uint work_dim, const size_t * global_work_offset, const size_t * global_work_size, const size_t * local_work_size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueMarkerWithWaitList)(cl_command_queue command_queue, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueBarrierWithWaitList)(cl_command_queue command_queue, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueSVMMemcpy)(cl_command_queue command_queue, cl_bool blocking_copy, void * dst_ptr, const void * src_ptr, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueSVMMemFill)(cl_command_queue command_queue, void * svm_ptr, const void * pattern, size_t pattern_size, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueSVMMap)(cl_command_queue command_queue, cl_bool blocking_map, cl_map_flags flags, void * svm_ptr, size_t size, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueSVMUnmap)(cl_command_queue command_queue, void * svm_ptr, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clEnqueueSVMMigrateMem)(cl_command_queue command_queue, cl_uint num_svm_pointers, const void ** svm_pointers, const size_t * sizes, cl_mem_migration_flags flags, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);
extern cl_int (*p_clGetKernelSuggestedLocalWorkSize)(cl_command_queue command_queue, cl_kernel kernel, cl_uint work_dim, const size_t * global_work_offset, const size_t * global_work_size, size_t * suggested_local_work_size);
extern void * (*p_clGetExtensionFunctionAddressForPlatform)(cl_platform_id platform, const char * func_name);
extern cl_int (*p_clSetCommandQueueProperty)(cl_command_queue command_queue, cl_command_queue_properties properties, cl_bool enable, cl_command_queue_properties * old_properties);
extern cl_mem (*p_clCreateImage2D)(cl_context context, cl_mem_flags flags, const cl_image_format * image_format, size_t image_width, size_t image_height, size_t image_row_pitch, void * host_ptr, cl_int * errcode_ret);
extern cl_mem (*p_clCreateImage3D)(cl_context context, cl_mem_flags flags, const cl_image_format * image_format, size_t image_width, size_t image_height, size_t image_depth, size_t image_row_pitch, size_t image_slice_pitch, void * host_ptr, cl_int * errcode_ret);
extern cl_int (*p_clEnqueueMarker)(cl_command_queue command_queue, cl_event * event);
extern cl_int (*p_clEnqueueWaitForEvents)(cl_command_queue command_queue, cl_uint num_events, const cl_event * event_list);
extern cl_int (*p_clEnqueueBarrier)(cl_command_queue command_queue);
extern cl_int (*p_clUnloadCompiler)(void);
extern void * (*p_clGetExtensionFunctionAddress)(const char * func_name);
extern cl_command_queue (*p_clCreateCommandQueue)(cl_context context, cl_device_id device, cl_command_queue_properties properties, cl_int * errcode_ret);
extern cl_sampler (*p_clCreateSampler)(cl_context context, cl_bool normalized_coords, cl_addressing_mode addressing_mode, cl_filter_mode filter_mode, cl_int * errcode_ret);
extern cl_int (*p_clEnqueueTask)(cl_command_queue command_queue, cl_kernel kernel, cl_uint num_events_in_wait_list, const cl_event * event_wait_list, cl_event * event);

// 解析全部 cl API 符号到 p_* 指针（从已加载的 OpenCL 库）。
// 返回解析成功的个数。
unsigned opencl_shim_export(void* handle);

} // extern "C"
