#include <chrono>
#include <thread>
#include "infra/include/Logger.h"
#include "infra/include/network/Network.h"
#include "infra/include/network/NetworkThreadPool.h"
#include "infra/include/thread/WorkThreadPool.h"
#include "Uluai.h"

int main(int argc, char* argv[]) {

    std::shared_ptr<infra::LogChannel> console_log = std::make_shared<infra::ConsoleLogChannel>();
    infra::Logger::instance().addLogChannel(console_log);

    infra::network_init();
    infra::NetworkThreadPool::instance()->init(4);
    infra::WorkThreadPool::instance()->init(4);

    auto alg = IUluai::create(E_ALG_CRK);
    alg->init();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}
