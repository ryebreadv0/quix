#ifndef _QUIX_PIPELINE_HPP
#define _QUIX_PIPELINE_HPP

namespace quix {

namespace graphics {

class pipeline {
public:

    pipeline();
    ~pipeline();

    pipeline(const pipeline&) = delete;
    pipeline& operator=(const pipeline&) = delete;
    pipeline(pipeline&&) = delete;
    pipeline& operator=(pipeline&&) = delete;
    
private:


};


} // namespace graphics


} // namespace quix


#endif // _QUIX_PIPELINE_HPP