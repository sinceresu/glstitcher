//
// Created by sujin on 2016/11/15.
//

#ifndef GLSTITCH_MEMTRANSFERFACTORY_H
#define GLSTITCH_MEMTRANSFERFACTORY_H


#include <memory>

class MemTransfer;

class MemTransferFactory {
public:
     static std::shared_ptr<MemTransfer> createInstance();
    /**
    * Try to enable platform optimizations. Returns true on success, else false.
    */
    static bool tryEnablePlatformOptimizations();
private:
    static bool usePlatformOptimizations;   // is true if tryEnablePlatformOptimizations() was called and succeeded


};


#endif //GLSTITCH_MEMTRANSFERFACTORY_H
