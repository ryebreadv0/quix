#ifndef _QUIX_SHADER_CPP
#define _QUIX_SHADER_CPP

#include "quix_shader.hpp"

#include <spirv-tools/libspirv.hpp>
#include <spirv-tools/optimizer.hpp>

#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>

namespace quix {

static glslang::EShTargetClientVersion eshTargetClientVersion = glslang::EShTargetVulkan_1_3;
static glslang::EShTargetLanguageVersion eshTargetLanguageVersion = glslang::EShTargetSpv_1_3;

// shader class
shader::shader(const char* path, EShLanguage stage)
{

    if (ends_with(path, ".spv")) {
        loadSpvCode(path);
        return;
    }

    // see if spv file exists and if it is newer than the shader file
    struct stat shaderStat;
    struct stat spvStat;

    std::string spvPath = std::string(path) + ".spv";
    const char* cSpvPath = spvPath.c_str();

    if (stat(path, &shaderStat) == -1) {
        spdlog::error("Error in {} shader file does not exist!", path);
        throw std::runtime_error("failed to parse shader!");
    }
    if (stat(cSpvPath, &spvStat) == -1) {
#ifdef _DEBUG
        spdlog::info("No spv found, generating spv file for {}", path);
#endif
    } else {
        // compare shader file date to spv file date
        if (shaderStat.st_mtime <= spvStat.st_mtime) {
            // then load in the spv file contents
            loadSpvCode(cSpvPath);
            return;
        } else {
#ifdef _DEBUG
            spdlog::info("Shader was updated, generating spv file for {}", path);
#endif
        }
    }
    compileShader(stage, path, cSpvPath);
}

shader::~shader() = default;

std::vector<uint32_t> shader::getSpirvCode()
{
    return code;
}

VkShaderModule shader::createShaderModule(VkDevice device)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size() * sizeof(uint32_t);
    createInfo.pCode = code.data();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void shader::loadSpvCode(const char* file)
{
    FILE* handle = fopen(file, "rb");
    if (handle == nullptr) {
        throw std::runtime_error("failed to open file!");
    }
    fseek(handle, 0, SEEK_END);
    size_t fileSize = ftell(handle);
    fseek(handle, 0, SEEK_SET);

    // this might cause errors if the file is not a multiple of 4 bytes
    code.resize(fileSize / sizeof(uint32_t));
    size_t readSize = fread(code.data(), sizeof(uint32_t), fileSize / sizeof(uint32_t), handle);

    fclose(handle);
}

void shader::saveSpvCode(const char* file)
{
    FILE* handle = fopen(file, "wb");
    if (handle == nullptr) {
        throw std::runtime_error("failed to open file!");
    }

    size_t writeSize = fwrite(code.data(), sizeof(uint32_t), code.size(), handle);

    fclose(handle);
}

void shader::setShaderVersion(uint32_t apiVersion)
{
    switch (apiVersion) {
    case VK_API_VERSION_1_0:
        eshTargetClientVersion = glslang::EShTargetVulkan_1_0;
        eshTargetLanguageVersion = glslang::EShTargetSpv_1_0;
        break;
    case VK_API_VERSION_1_1:
        eshTargetClientVersion = glslang::EShTargetVulkan_1_1;
        eshTargetLanguageVersion = glslang::EShTargetSpv_1_1;
        break;
    case VK_API_VERSION_1_2:
        eshTargetClientVersion = glslang::EShTargetVulkan_1_2;
        eshTargetLanguageVersion = glslang::EShTargetSpv_1_2;
        break;
    case VK_API_VERSION_1_3:
        eshTargetClientVersion = glslang::EShTargetVulkan_1_3;
        eshTargetLanguageVersion = glslang::EShTargetSpv_1_3;
        break;
    default:
        throw std::runtime_error("Invalid vulkan version");
    }
}

void shader::compileShader(EShLanguage stage, const char* path, const char* cSpvPath)
{

    const TBuiltInResource* resources = GetDefaultResources();

    glslang::TProgram program;

    glslang::TShader shader(stage);
    shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, eshTargetClientVersion);
    shader.setEnvTarget(glslang::EShTargetSpv, eshTargetLanguageVersion);

    std::string source = getSourceCode(path);
    const char* cSource = source.c_str();
    shader.setStrings(&cSource, 1);

    if (!shader.parse(resources, 100, false, EShMsgDefault)) {
        spdlog::error("Error in {} {}", path, shader.getInfoLog());
        throw std::runtime_error("failed to parse shader!");
    }

    program.addShader(&shader);

    if (!program.link(EShMsgDefault)) {
        spdlog::error("Error in {} {}", path, program.getInfoLog());
        throw std::runtime_error("failed to link shader!");
    }

    glslang::GlslangToSpv(*program.getIntermediate(stage), code);

    // optimize spirv

    spvtools::Optimizer optimizer(SPV_ENV_UNIVERSAL_1_3);
    optimizer.SetMessageConsumer([](spv_message_level_t level, const char* source, const spv_position_t& position, const char* message) {
        if (level == SPV_MSG_FATAL || level == SPV_MSG_INTERNAL_ERROR)
            throw std::runtime_error(message);
        else if (level == SPV_MSG_ERROR)
            spdlog::error("{}: {}", position.index, message);
        else if (level == SPV_MSG_WARNING)
            spdlog::warn("{}: {}", position.index, message);
        else if (level == SPV_MSG_INFO)
            spdlog::info("{}: {}", position.index, message);
        else if (level == SPV_MSG_DEBUG)
            spdlog::debug("{}: {}", position.index, message);
    });

    optimizer.RegisterLegalizationPasses();
    optimizer.RegisterPerformancePasses();
    optimizer.RegisterSizePasses();

    // I will be honest I don't know how this thing works
    optimizer.RegisterPass(spvtools::CreateSetSpecConstantDefaultValuePass({ { 1, "42" } }))
        .RegisterPass(spvtools::CreateFreezeSpecConstantValuePass())
        .RegisterPass(spvtools::CreateUnifyConstantPass())
        .RegisterPass(spvtools::CreateStripDebugInfoPass())
        .RegisterPass(spvtools::CreateEliminateDeadConstantPass())
        .RegisterPass(spvtools::CreateFoldSpecConstantOpAndCompositePass());

    if (!optimizer.Run(code.data(), code.size(), &code)) {
        throw std::runtime_error("failed to optimize shader!");
    }

    // save the spv code to a file
    saveSpvCode(cSpvPath);
}

const std::string shader::getSourceCode(const char* path)
{
    FILE* file = fopen(path, "r");
    if (file == nullptr) {
        throw std::runtime_error("failed to open file!");
    }
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::string source;
    source.resize(fileSize);
    size_t readSize = fread(source.data(), sizeof(char), fileSize, file);
    // printf("readSize: %zu\n", readSize);

    return source;
}

bool shader::ends_with(const char* str, const char* suffix)
{
    size_t len_str = strlen(str);
    size_t len_suffix = strlen(suffix);
    if (len_suffix > len_str) {
        return false;
    }
    return strncmp(str + len_str - len_suffix, suffix, len_suffix) == 0;
}

} // namespace quix

#endif // _QUIX_SHADER_HPP