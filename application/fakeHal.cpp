#include "hal/Video.h"
#include "hal/Image.h"
#include "hal/Audio.h"

namespace hal
{
IVideo* IVideo::instance() {
    return nullptr;
}
IImage* IImage::instance() {
    return nullptr;
}
IAudio* IAudio::instance() {
    return nullptr;
}
} // namespace hal
