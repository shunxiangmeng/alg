#include <chrono>
#include <thread>
#include "infra/include/Logger.h"
#include "infra/include/network/Network.h"
#include "infra/include/network/NetworkThreadPool.h"
#include "infra/include/thread/WorkThreadPool.h"
#include "Fmix.h"

int main(int argc, char* argv[]) {

    std::shared_ptr<infra::LogChannel> console_log = std::make_shared<infra::ConsoleLogChannel>();
    infra::Logger::instance().addLogChannel(console_log);

    infra::network_init();
    infra::NetworkThreadPool::instance()->init(4);
    infra::WorkThreadPool::instance()->init(4);

    std::shared_ptr<Fmix> fmix = std::make_shared<Fmix>();
    fmix->init();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}