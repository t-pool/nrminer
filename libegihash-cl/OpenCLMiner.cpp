/*
 * OpenCLMiner.cpp
 *
 *  Created on: Dec 13, 2017
 *      Author: ranjeet
 */

#include "libegihash-cl/OpenCLMiner.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#pragma GCC diagnostic ignored "-Wmissing-braces"
#include "../libegihash-cl/CL/cl2.hpp"
#pragma GCC diagnostic pop
#include "nrghash/nrghash.h"
#include "CLMiner_kernel.h"
#include "common/Log.h"

#include <algorithm>
#include <vector>
#include <iostream>

namespace {

const char* strClError(cl_int err)
{
    switch (err) {
        case CL_SUCCESS:
            return "CL_SUCCESS";
        case CL_DEVICE_NOT_FOUND:
            return "CL_DEVICE_NOT_FOUND";
        case CL_DEVICE_NOT_AVAILABLE:
            return "CL_DEVICE_NOT_AVAILABLE";
        case CL_COMPILER_NOT_AVAILABLE:
            return "CL_COMPILER_NOT_AVAILABLE";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case CL_OUT_OF_RESOURCES:
            return "CL_OUT_OF_RESOURCES";
        case CL_OUT_OF_HOST_MEMORY:
            return "CL_OUT_OF_HOST_MEMORY";
        case CL_PROFILING_INFO_NOT_AVAILABLE:
            return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case CL_MEM_COPY_OVERLAP:
            return "CL_MEM_COPY_OVERLAP";
        case CL_IMAGE_FORMAT_MISMATCH:
            return "CL_IMAGE_FORMAT_MISMATCH";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
            return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case CL_BUILD_PROGRAM_FAILURE:
            return "CL_BUILD_PROGRAM_FAILURE";
        case CL_MAP_FAILURE:
            return "CL_MAP_FAILURE";
        case CL_MISALIGNED_SUB_BUFFER_OFFSET:
            return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
            return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";

#ifdef CL_VERSION_1_2
        case CL_COMPILE_PROGRAM_FAILURE:
            return "CL_COMPILE_PROGRAM_FAILURE";
        case CL_LINKER_NOT_AVAILABLE:
            return "CL_LINKER_NOT_AVAILABLE";
        case CL_LINK_PROGRAM_FAILURE:
            return "CL_LINK_PROGRAM_FAILURE";
        case CL_DEVICE_PARTITION_FAILED:
            return "CL_DEVICE_PARTITION_FAILED";
        case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
            return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
#endif // CL_VERSION_1_2

        case CL_INVALID_VALUE:
                return "CL_INVALID_VALUE";
        case CL_INVALID_DEVICE_TYPE:
            return "CL_INVALID_DEVICE_TYPE";
        case CL_INVALID_PLATFORM:
            return "CL_INVALID_PLATFORM";
        case CL_INVALID_DEVICE:
            return "CL_INVALID_DEVICE";
        case CL_INVALID_CONTEXT:
            return "CL_INVALID_CONTEXT";
        case CL_INVALID_QUEUE_PROPERTIES:
            return "CL_INVALID_QUEUE_PROPERTIES";
        case CL_INVALID_COMMAND_QUEUE:
            return "CL_INVALID_COMMAND_QUEUE";
        case CL_INVALID_HOST_PTR:
            return "CL_INVALID_HOST_PTR";
        case CL_INVALID_MEM_OBJECT:
            return "CL_INVALID_MEM_OBJECT";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
            return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case CL_INVALID_IMAGE_SIZE:
            return "CL_INVALID_IMAGE_SIZE";
        case CL_INVALID_SAMPLER:
            return "CL_INVALID_SAMPLER";
        case CL_INVALID_BINARY:
            return "CL_INVALID_BINARY";
        case CL_INVALID_BUILD_OPTIONS:
            return "CL_INVALID_BUILD_OPTIONS";
        case CL_INVALID_PROGRAM:
            return "CL_INVALID_PROGRAM";
        case CL_INVALID_PROGRAM_EXECUTABLE:
            return "CL_INVALID_PROGRAM_EXECUTABLE";
        case CL_INVALID_KERNEL_NAME:
            return "CL_INVALID_KERNEL_NAME";
        case CL_INVALID_KERNEL_DEFINITION:
            return "CL_INVALID_KERNEL_DEFINITION";
        case CL_INVALID_KERNEL:
            return "CL_INVALID_KERNEL";
        case CL_INVALID_ARG_INDEX:
            return "CL_INVALID_ARG_INDEX";
        case CL_INVALID_ARG_VALUE:
            return "CL_INVALID_ARG_VALUE";
        case CL_INVALID_ARG_SIZE:
            return "CL_INVALID_ARG_SIZE";
        case CL_INVALID_KERNEL_ARGS:
            return "CL_INVALID_KERNEL_ARGS";
        case CL_INVALID_WORK_DIMENSION:
            return "CL_INVALID_WORK_DIMENSION";
        case CL_INVALID_WORK_GROUP_SIZE:
            return "CL_INVALID_WORK_GROUP_SIZE";
        case CL_INVALID_WORK_ITEM_SIZE:
                return "CL_INVALID_WORK_ITEM_SIZE";
        case CL_INVALID_GLOBAL_OFFSET:
            return "CL_INVALID_GLOBAL_OFFSET";
        case CL_INVALID_EVENT_WAIT_LIST:
            return "CL_INVALID_EVENT_WAIT_LIST";
        case CL_INVALID_EVENT:
            return "CL_INVALID_EVENT";
        case CL_INVALID_OPERATION:
            return "CL_INVALID_OPERATION";
        case CL_INVALID_GL_OBJECT:
            return "CL_INVALID_GL_OBJECT";
        case CL_INVALID_BUFFER_SIZE:
            return "CL_INVALID_BUFFER_SIZE";
        case CL_INVALID_MIP_LEVEL:
            return "CL_INVALID_MIP_LEVEL";
        case CL_INVALID_GLOBAL_WORK_SIZE:
            return "CL_INVALID_GLOBAL_WORK_SIZE";
        case CL_INVALID_PROPERTY:
            return "CL_INVALID_PROPERTY";

#ifdef CL_VERSION_1_2
        case CL_INVALID_IMAGE_DESCRIPTOR:
            return "CL_INVALID_IMAGE_DESCRIPTOR";
        case CL_INVALID_COMPILER_OPTIONS:
            return "CL_INVALID_COMPILER_OPTIONS";
        case CL_INVALID_LINKER_OPTIONS:
            return "CL_INVALID_LINKER_OPTIONS";
        case CL_INVALID_DEVICE_PARTITION_COUNT:
            return "CL_INVALID_DEVICE_PARTITION_COUNT";
#endif // CL_VERSION_1_2

#ifdef CL_VERSION_2_0
        case CL_INVALID_PIPE_SIZE:
            return "CL_INVALID_PIPE_SIZE";
        case CL_INVALID_DEVICE_QUEUE:
            return "CL_INVALID_DEVICE_QUEUE";
#endif // CL_VERSION_2_0

#ifdef CL_VERSION_2_2
        case CL_INVALID_SPEC_ID:
            return "CL_INVALID_SPEC_ID";
        case CL_MAX_SIZE_RESTRICTION_EXCEEDED:
            return "CL_MAX_SIZE_RESTRICTION_EXCEEDED";
#endif // CL_VERSION_2_2
    }
    return "Unknown CL error encountered";
}

std::string CLErrorHelper(const cl::Error& clerr)
{
    std::ostringstream osstream;
    osstream << ": " << clerr.what() << ": " << strClError(clerr.err())
        << " (" << clerr.err() << ")";
    return osstream.str();
}

inline void addDefinition(std::string& _source, char const* _id, unsigned _value)
{
    char buf[256];
    sprintf(buf, "#define %s %uu\n", _id, _value);
    _source.insert(_source.begin(), buf, buf + strlen(buf));
}

std::vector<cl::Platform> getPlatforms()
{
    std::vector<cl::Platform> platforms;
    try
    {
        cl::Platform::get(&platforms);
    }
    catch(cl::Error const& err)
    {
#if defined(CL_PLATFORM_NOT_FOUND_KHR)
        if (err.err() == CL_PLATFORM_NOT_FOUND_KHR)
            cwarn << "No OpenCL platforms found";
        else
#endif
            throw err;
    }
    return platforms;
}

std::vector<cl::Device> getDevices(std::vector<cl::Platform> const& _platforms, unsigned _platformId)
{
    std::vector<cl::Device> devices;
    size_t platform_num = std::min<size_t>(_platformId, _platforms.size() - 1);
    try
    {
        _platforms[platform_num].getDevices(
            CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_ACCELERATOR,
            &devices
        );
    }
    catch (cl::Error const& err)
    {
        // if simply no devices found return empty vector
        if (err.err() != CL_DEVICE_NOT_FOUND)
            throw err;
    }
    return devices;
}


} //unnamed namespace

using namespace energi;


std::mutex OpenCLMiner::m_device_mutex;

unsigned OpenCLMiner::s_workgroupSize = OpenCLMiner::c_defaultLocalWorkSize;
unsigned OpenCLMiner::s_initialGlobalWorkSize = OpenCLMiner::c_defaultGlobalWorkSizeMultiplier * OpenCLMiner::c_defaultLocalWorkSize;
unsigned OpenCLMiner::s_threadsPerHash = 8;
bool OpenCLMiner::s_adjustWorkSize = false;
constexpr size_t c_maxSearchResults = 1;

unsigned OpenCLMiner::s_platformId = 0;
unsigned OpenCLMiner::s_numInstances = 0;
// TODO: get smarter about how many miners we support. Why 16?
std::vector<int>  OpenCLMiner::s_devices(MAX_MINERS, -1);

struct CLChannel: public LogChannel
{
    static const char* name()
    {
        return EthOrange "cl";
    }

    static const int verbosity = 2;
    static const bool debug = false;
};

#define cllog clog(CLChannel)
#define ETHCL_LOG(_contents) cllog << _contents

struct OpenCLMiner::clInfo
{
    static std::tuple<bool, cl::Device, int, int, std::string> getDeviceInfo(int index, OpenCLMiner* clMiner);

    cl::Context             context_;
    cl::CommandQueue        queue_;
    cl::Kernel              kernelSearch_;
    cl::Kernel              kernelDag_;

    cl::Buffer              bufferDag_;
    cl::Buffer              bufferLight_;
    cl::Buffer              bufferHeader_;
    cl::Buffer              searchBuffer_;
};

OpenCLMiner::OpenCLMiner(const Plant& plant, unsigned index)
    : Miner("GPU/", plant, index)
{
}

OpenCLMiner::~OpenCLMiner()
{
    stopWorking();
}

unsigned OpenCLMiner::getNumDevices()
{
    std::vector<cl::Platform> platforms = getPlatforms();
    if (platforms.empty())
        return 0;

    std::vector<cl::Device> devices = getDevices(platforms, s_platformId);
    if (devices.empty()) {
        return 0;
    }
    return devices.size();
}

void OpenCLMiner::setNumInstances(unsigned _instances)
{
    try {
        s_numInstances = std::min<unsigned>(_instances, getNumDevices());
    } catch (const cl::Error& err) {
        cwarn << "OpenCL Error:" << CLErrorHelper(err);
        std::exit(0);
    }
}

void OpenCLMiner::listDevices()
{
    std::string outString ="\nListing OpenCL devices.\nFORMAT: [platformID] [deviceID] deviceName\n";
    unsigned int i = 0;

    std::vector<cl::Platform> platforms = getPlatforms();
    if (platforms.empty())
        return;


    for (unsigned j = 0; j < platforms.size(); ++j) {
        i = 0;
        std::vector<cl::Device> devices = getDevices(platforms, j);
        for (auto const& device: devices) {
            outString += "[" + std::to_string(j) + "] [" + std::to_string(i) + "] " + device.getInfo<CL_DEVICE_NAME>() + "\n";
            outString += "\tCL_DEVICE_TYPE: ";
            switch (device.getInfo<CL_DEVICE_TYPE>())
            {
                case CL_DEVICE_TYPE_CPU:
                    outString += "CPU\n";
                    break;
                case CL_DEVICE_TYPE_GPU:
                    outString += "GPU\n";
                    break;
                case CL_DEVICE_TYPE_ACCELERATOR:
                    outString += "ACCELERATOR\n";
                    break;
                default:
                    outString += "DEFAULT\n";
                    break;
            }
            outString += "\tCL_DEVICE_GLOBAL_MEM_SIZE: " + std::to_string(device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()) + "\n";
            outString += "\tCL_DEVICE_MAX_MEM_ALLOC_SIZE: " + std::to_string(device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>()) + "\n";
            outString += "\tCL_DEVICE_MAX_WORK_GROUP_SIZE: " + std::to_string(device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>()) + "\n";
            ++i;
        }
    }

    cllog << outString;
}

bool OpenCLMiner::configureGPU(
        unsigned _localWorkSize,
        int _globalWorkSizeMultiplier,
        unsigned _platformId,
        uint64_t _currentBlock,
        unsigned _dagLoadMode,
        unsigned _dagCreateDevice,
        bool _noeval,
        bool _exit)
{
    std::lock_guard<std::mutex> lock(m_device_mutex);
    s_noeval = _noeval;
    s_dagLoadMode = _dagLoadMode;
    s_dagCreateDevice = _dagCreateDevice;
    s_exit = _exit;
    s_platformId = _platformId;
    _localWorkSize = ((_localWorkSize + 7) / 8) * 8;
    s_workgroupSize = _localWorkSize;
    if (_globalWorkSizeMultiplier < 0) {
        s_adjustWorkSize = true;
        _globalWorkSizeMultiplier = -_globalWorkSizeMultiplier;
    }
    s_initialGlobalWorkSize = _globalWorkSizeMultiplier * _localWorkSize;

    uint64_t dagSize = nrghash::dag_t::get_full_size(_currentBlock);

    std::vector<cl::Platform> platforms = getPlatforms();
    if (platforms.empty())
        return false;

    if (_platformId >= platforms.size())
        return false;

    bool devices_valid = true;
    std::vector<cl::Device> devices = getDevices(platforms, _platformId);
    for (const auto& device : devices) {
        cl_ulong gpu_mem_size = 0;
        device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &gpu_mem_size);
        if (gpu_mem_size < dagSize) {
            devices_valid = false;
            cwarn << "OpenCL device " << device.getInfo<CL_DEVICE_NAME>()
                  << " has insufficient GPU memory." << FormattedMemSize(gpu_mem_size)
                  << " of memory found < " << FormattedMemSize(dagSize) << " of memory required";
        }
    }
    return devices_valid;
}


void OpenCLMiner::trun()
{
    setThreadName("OpenCL");
    // Memory for zero-ing buffers. Cannot be static because crashes on macOS.
    uint32_t const c_zero = 0;
    uint64_t startNonce = 0;

    const uint8_t kIntervalPasses = 4;  // must be a power of 2 passes
    // this gives each miner a pretty big range of nonces, supporting up to 16 miners.
    // TODO: get smarter about how many miners we support.
    //uint64_t const nonceSegment = static_cast<uint64_t>(m_index) << (64 - 4);
    try {
        while (!shouldStop()) {
            if (is_mining_paused()) {
                std::this_thread::sleep_for(std::chrono::seconds(3));
                continue;
            }
            const Work& work = this->getWork(); // This work is a copy of last assigned work the worker was provided by plant
            if ( !work.isValid() ) {
                cnote << "No work received. Pause for 1 s.";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                if ( this->shouldStop() ) {
                    break;
                }
                continue;
            } else {
                //cllog << name() << "Valid work.";
            }
            if (m_current != work) {
                if (!m_dagLoaded || ((work.nHeight / nrghash::constants::EPOCH_LENGTH) != (m_lastHeight / nrghash::constants::EPOCH_LENGTH))) {
                    if (s_dagLoadMode == DAG_LOAD_MODE_SEQUENTIAL) {
                        while (s_dagLoadIndex < m_index)
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                        ++s_dagLoadIndex;
                    }
                    init_dag(work.nHeight);
                    m_dagLoaded = true;
                }
                m_lastHeight = work.nHeight;
                m_current = work;
                energi::CBlockHeaderTruncatedLE truncatedBlockHeader(m_current);
                nrghash::h256_t hash_header(&truncatedBlockHeader, sizeof(truncatedBlockHeader));

                // Upper 64 bits of the boundary.
                const uint64_t target = *reinterpret_cast<uint64_t const *>((m_current.hashTarget >> 192).data());
                assert(target > 0);

                // Update header constant buffer.
                m_queue.enqueueWriteBuffer(m_header, CL_FALSE, 0, hash_header.hash_size, &hash_header.b[0]);
                m_queue.enqueueWriteBuffer(m_searchBuffer, CL_FALSE, 0, sizeof(c_zero), &c_zero);

                m_searchKernel.setArg(0, m_searchBuffer);  // Supply output buffer to kernel.
                m_searchKernel.setArg(4, target);

                startNonce = m_plant.getStartNonce(m_current, m_index);
            }

            // Run the kernel.
            m_searchKernel.setArg(3, startNonce);
            m_queue.enqueueNDRangeKernel(m_searchKernel, cl::NullRange, globalWorkSize_, workgroupSize_);

            // Read results.
            // TODO: could use pinned host pointer instead.
            uint32_t results[c_maxSearchResults + 1] = { 0 };
            m_queue.enqueueReadBuffer(m_searchBuffer, CL_TRUE, 0, sizeof(results), &results);

            uint64_t nonce = 0;
            if (results[0] > 0) {
                // Ignore results except the first one.
                nonce = startNonce + results[1];
                // Reset search buffer if any solution found.
                m_queue.enqueueWriteBuffer(m_searchBuffer, CL_TRUE, 0, sizeof(c_zero), &c_zero);
            }

            // Report results while the kernel is running.
            // It takes some time because proof of work must be re-evaluated on CPU.
            if (nonce != 0) {
                m_current.nNonce = nonce;
                auto const powHash = GetPOWHash(m_current);
                if (UintToArith256(powHash) <= m_current.hashTarget) {
                    cllog << name() << " Submitting block blockhash: " << m_current.GetHash().ToString() << " height: " << m_current.nHeight << " nonce: " << nonce;
                    Solution solution(m_current, m_current.getSecondaryExtraNonce());
                    m_plant.submitProof(solution);
                } else {
                    cwarn << name() << " CL Miner proposed invalid solution: " << m_current.GetHash().ToString() << " nonce: " << nonce;
                }
            }
            // Increase start nonce for following kernel execution.
            startNonce += globalWorkSize_;
            m_hashCount += globalWorkSize_;
            if ((++m_searchPasses & (kIntervalPasses - 1)) == 0) {
                updateHashRate(m_hashCount);
                m_hashCount = 0;
            }

            // Make sure the last buffer write has finished --
            // it reads local variable.
            m_queue.finish();
        }
        m_queue.finish();
    } catch (cl::Error const& _e) {
        cwarn << name() << " OpenCL Error: " << CLErrorHelper(_e);
    }
}


std::tuple<bool, cl::Device, int, int, std::string> OpenCLMiner::getDeviceInfo(int index)
{
    auto failResult = std::make_tuple(false, cl::Device(), 0, 0, "");
    std::vector<cl::Platform> platforms = getPlatforms();
    if (platforms.empty()) {
        return failResult;
    }

    // use selected platform
    unsigned platformIdx = std::min<unsigned>(s_platformId, platforms.size() - 1);
    std::string platformName = platforms[platformIdx].getInfo<CL_PLATFORM_NAME>();
    ETHCL_LOG("Platform: " << platformName);

    int platformId = OPENCL_PLATFORM_UNKNOWN;
    if (platformName == "NVIDIA CUDA") {
        platformId = OPENCL_PLATFORM_NVIDIA;
        m_hwmoninfo.deviceType = HwMonitorInfoType::NVIDIA;
        m_hwmoninfo.indexSource = HwMonitorIndexSource::OPENCL;
    } else if (platformName == "AMD Accelerated Parallel Processing") {
        platformId = OPENCL_PLATFORM_AMD;
        m_hwmoninfo.deviceType = HwMonitorInfoType::AMD;
        m_hwmoninfo.indexSource = HwMonitorIndexSource::OPENCL;
    } else if (platformName == "Clover") {
        platformId = OPENCL_PLATFORM_CLOVER;
    }

    // get GPU device of the default platform
    std::vector<cl::Device> devices = getDevices(platforms, platformIdx);
    if (devices.empty()) {
        ETHCL_LOG("No OpenCL devices found.");
        return failResult;
    }

    // use selected device
    int idx = index % devices.size();
    unsigned deviceId = s_devices[idx] > -1 ? s_devices[idx] : index;
    m_hwmoninfo.deviceIndex = deviceId % devices.size();
    cl::Device& device = devices[deviceId % devices.size()];
    std::string device_version = device.getInfo<CL_DEVICE_VERSION>();
    ETHCL_LOG("Device:   " << device.getInfo<CL_DEVICE_NAME>() << " / " << device_version);

    std::string clVer = device_version.substr(7, 3);

    if (clVer == "1.0" || clVer == "1.1") {
        if (platformId == OPENCL_PLATFORM_CLOVER) {
            ETHCL_LOG("OpenCL " << clVer << " not supported, but platform Clover might work nevertheless. USE AT OWN RISK!");
        } else {
            ETHCL_LOG("OpenCL " << clVer << " not supported - minimum required version is 1.2");
            return failResult;
        }
    }

    char options[256];
    int computeCapability = 0;
    if (platformId == OPENCL_PLATFORM_NVIDIA) {
        cl_uint computeCapabilityMajor;
        cl_uint computeCapabilityMinor;
        clGetDeviceInfo(device(), CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV, sizeof(cl_uint), &computeCapabilityMajor, NULL);
        clGetDeviceInfo(device(), CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV, sizeof(cl_uint), &computeCapabilityMinor, NULL);

        computeCapability = computeCapabilityMajor * 10 + computeCapabilityMinor;
        int maxregs = computeCapability >= 35 ? 72 : 63;
        sprintf(options, "-cl-nv-maxrregcount=%d", maxregs);
    } else {
        sprintf(options, "%s", "");
    }

    return std::make_tuple(true, device, platformId, computeCapability, std::string(options));
}

bool OpenCLMiner::init_dag(uint32_t height)
{
    // get all platforms
    try {
        uint32_t const epoch = height / nrghash::constants::EPOCH_LENGTH;
        cllog << name() << " Generating DAG for epoch #" << epoch;
        auto deviceResult = getDeviceInfo(m_index);
        // create context
        auto device = std::get<1>(deviceResult);
        m_context  = cl::Context(std::vector<cl::Device>(&device, &device + 1));
        m_queue    = cl::CommandQueue(m_context, device);

        workgroupSize_        = s_workgroupSize;
        globalWorkSize_       = s_initialGlobalWorkSize;


        if (s_adjustWorkSize) {
            unsigned int computeUnits;
            clGetDeviceInfo(device(), CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, NULL);
            // Apparently some 36 CU devices return a bogus 14!!!
            auto platformId = std::get<2>(deviceResult);
            computeUnits = computeUnits == 14 ? 36 : computeUnits;
            if ((platformId == OPENCL_PLATFORM_AMD) && (computeUnits != 36)) {
                globalWorkSize_ = (globalWorkSize_ * computeUnits) / 36;
                // make sure that global work size is evenly divisible by the local workgroup size
                if (globalWorkSize_ % workgroupSize_ != 0)
                    globalWorkSize_ = ((globalWorkSize_ / workgroupSize_) + 1) * workgroupSize_;
                cnote << "Adjusting CL work multiplier for " << computeUnits << " CUs."
                    << " Adjusted work multiplier: " << globalWorkSize_ / workgroupSize_;
            }
        }
        nrghash::cache_t  cache = nrghash::cache_t(height);
        uint64_t dagSize = nrghash::dag_t::get_full_size(height);//dag->size();
        uint32_t dagSize128 = (unsigned)(dagSize / nrghash::constants::MIX_BYTES);
        uint32_t lightSize64 = (unsigned)(cache.data().size()); //dag->get_cache().data().size();
        // patch source code
        // note: CLMiner_kernel is simply ethash_cl_miner_kernel.cl compiled
        // into a byte array by bin2h.cmake. There is no need to load the file by hand in runtime
        // TODO: Just use C++ raw string literal.
        std::string code(CLMiner_kernel, CLMiner_kernel + sizeof(CLMiner_kernel));

        addDefinition(code, "GROUP_SIZE", workgroupSize_);
        addDefinition(code, "DAG_SIZE", dagSize128);
        addDefinition(code, "LIGHT_SIZE", lightSize64);
        addDefinition(code, "ACCESSES", nrghash::constants::ACCESSES);
        addDefinition(code, "MAX_OUTPUTS", c_maxSearchResults);
        addDefinition(code, "PLATFORM", std::get<2>(deviceResult));
        addDefinition(code, "COMPUTE", std::get<3>(deviceResult));
        addDefinition(code, "THREADS_PER_HASH", 8); // going to be set to 8 by the kernel either way , kernel only supports 8

        // create miner OpenCL program
        cl::Program::Sources sources{{code.data(), code.size()}};
        cl::Program program(m_context, sources);
        try {
            program.build({device}, std::get<4>(deviceResult).c_str());
        } catch (cl::Error const&) {
            cwarn << name() << " Build info: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
            cwarn << name() << " Failed" ;
            return false;
        }

        // create buffer for dag
        std::vector<uint32_t> vData;
        for (auto &d : cache.data()) {
            for ( auto &dv : d) {
                vData.push_back(dv.hword);
            }
        }

        cl_ulong result = 0;
        device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &result);
        if (result < dagSize) {
            cllog << name() << " OpenCL device " << device.getInfo<CL_DEVICE_NAME>()
                << " has insufficient GPU memory. " << FormattedMemSize(result)
                << " of memory found < " << FormattedMemSize(dagSize) << " of memory required";
            return false;
        }
        try {
            m_light      = cl::Buffer(m_context, CL_MEM_READ_ONLY, sizeof(uint32_t) * vData.size());
            m_dag        = cl::Buffer(m_context, CL_MEM_READ_ONLY, dagSize);

            m_searchKernel     = cl::Kernel(program, "ethash_search");
            m_dagKernel        = cl::Kernel(program, "ethash_calculate_dag_item");

            //ETHCL_LOG("Creating light buffer");

            m_queue.enqueueWriteBuffer(m_light, CL_TRUE, 0, sizeof(uint32_t) * vData.size(), vData.data());
        } catch (cl::Error const& err) {
            cwarn << name() << "Creating DAG buffer failed: " << err.what() << err.err();
            return false;
        }

        // create buffer for header
        //ETHCL_LOG("Creating buffer for header.");
        m_header = cl::Buffer(m_context, CL_MEM_READ_ONLY, 32);

        m_searchKernel.setArg(1, m_header);
        m_searchKernel.setArg(2, m_dag);
        m_searchKernel.setArg(5, ~0u);  // Pass this to stop the compiler unrolling the loops.

        // create mining buffers
        //ETHCL_LOG("Creating mining buffer");
        m_searchBuffer = cl::Buffer(m_context, CL_MEM_WRITE_ONLY, (c_maxSearchResults + 1) * sizeof(uint32_t));

        uint32_t const work = (uint32_t)(dagSize / sizeof(nrghash::node));
        uint32_t fullRuns = work / globalWorkSize_;
        uint32_t const restWork = work % globalWorkSize_;
        if (restWork > 0) {
            fullRuns++;
        }

        m_dagKernel.setArg(1, m_light);
        m_dagKernel.setArg(2, m_dag);
        m_dagKernel.setArg(3, ~0u);

        for (uint32_t i = 0; i < fullRuns; ++i) {
            m_dagKernel.setArg(0, i * globalWorkSize_);
            m_queue.enqueueNDRangeKernel(m_dagKernel, cl::NullRange, globalWorkSize_, workgroupSize_);
            m_queue.finish();
        }

        cllog << name() << " Generating DAG for epoch #" << epoch << " finished.";

    } catch (cl::Error const& err) {
        cwarn << name() << err.what() << " (" << err.err() << ")";
        return false;
    }
    return true;
}
