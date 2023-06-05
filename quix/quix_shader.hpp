#ifndef _QUIX_SHADER_HPP
#define _QUIX_SHADER_HPP

namespace quix {

class shader {
public:
    shader(const char* path, EShLanguage stage);

    shader(const shader&) = delete;
    shader& operator=(const shader&) = delete;
    shader(shader&&) = delete;
    shader& operator=(shader&&) = delete;

    static void setShaderVersion(uint32_t apiVersion);

    std::vector<uint32_t> getSpirvCode();
    VkShaderModule createShaderModule(VkDevice device);

private:
    void compileShader(EShLanguage stage, const char* path, const char* cSpvPath);
    const std::string getSourceCode(const char* path);
    void loadSpvCode(const char* path);
    void saveSpvCode(const char* path);
    static bool ends_with(const char* str, const char* suffix);

    std::vector<uint32_t> code;
};

} // namespace quix

#endif // _QUIX_SHADER_HPP