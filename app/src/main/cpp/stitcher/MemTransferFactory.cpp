//
// Created by sujin on 2016/11/15.
//

#include "MemTransferFactory.h"
#include "MemTransfer.h"

bool MemTransferFactory::usePlatformOptimizations = false;

std::shared_ptr<MemTransfer> MemTransferFactory::createInstance() {

    return std::make_shared<MemTransfer>();
   // std::shared_ptr<MemTransfer> instance = NULL;

/*    if (usePlatformOptimizations) {   // create specialized instance
#ifdef __APPLE__
        instance = (MemTransfer *)new MemTransferIOS();
#elif __ANDROID__
        instance = (MemTransfer *)new MemTransferAndroid();
#endif
    }*/

    /*if (!usePlatformOptimizations)*/ {    // create default instance
      //  instance = std::make_shared<MemTransfer>();
    }

  //  return instance;
}

bool MemTransferFactory::tryEnablePlatformOptimizations() {
/*#ifdef __APPLE__
    usePlatformOptimizations = MemTransferIOS::initPlatformOptimizations();
#elif __ANDROID__
    usePlatformOptimizations = MemTransferAndroid::initPlatformOptimizations();
#else
    usePlatformOptimizations = false;
#endif*/

    return usePlatformOptimizations;
}
