#pragma once

namespace DynamicLoader {
class DynamicScriptInterface {
  public:
    virtual ~DynamicScriptInterface() {}
    virtual void SetUp() = 0;
    virtual void Run() = 0;
    virtual void TearDown() = 0;
};
}
