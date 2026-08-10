#include <Geode/Geode.hpp>
#include "utils/MacUtils.hpp"

#include <objc/runtime.h>
#include <mach-o/utils.h>
#include <sys/sysctl.h>

#define CommentType CommentTypeDummy
#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#undef CommentType

using namespace geode::prelude;

namespace tinker::utils {
    CCPoint getRealMousePos() {
        auto windowFrame = NSApp.mainWindow.frame;
        auto viewFrame = NSApp.mainWindow.contentView.frame;
        auto scaleFactor = CCPoint(CCDirector::get()->getWinSize()) / ccp(viewFrame.size.width, viewFrame.size.height);
        auto mouse = [NSEvent mouseLocation];
        return ccp(mouse.x - windowFrame.origin.x, mouse.y - windowFrame.origin.y) * scaleFactor;
    }
}