#include "synacor_vm.h"

#include <string>
#include <napi.h>

class VMWrap : public Napi::ObjectWrap<VMWrap> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    VMWrap(const Napi::CallbackInfo& info);
  
  private:
    Napi::Value Execute(const Napi::CallbackInfo& info);
    Napi::Value Undo(const Napi::CallbackInfo& info);
    Napi::Value GetState(const Napi::CallbackInfo& info);
    Napi::Value Reset(const Napi::CallbackInfo& info);
    Napi::Value Revert(const Napi::CallbackInfo& info);

    SynacorVM vm_;
};

Napi::Object VMWrap::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
    DefineClass(env, "VMWrap", {InstanceMethod<&VMWrap::Execute>("execute"),
                                InstanceMethod<&VMWrap::Undo>("undo"),
                                InstanceMethod<&VMWrap::GetState>("getState"),
                                InstanceMethod<&VMWrap::Reset>("reset"),
                                InstanceMethod<&VMWrap::Revert>("revert")});
  
  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  exports.Set("VMWrap", func);
  env.SetInstanceData<Napi::FunctionReference>(constructor);

  return exports;
}

VMWrap::VMWrap(const Napi::CallbackInfo& info) : Napi::ObjectWrap<VMWrap>(info) {
  Napi::Env env = info.Env();

  if (info.Length() <= 0 || !info[0].IsString()) {
    Napi::TypeError::New(env, "Expected string argument for constructor").ThrowAsJavaScriptException();
    return;
  }

  Napi::String bin_path = info[0].As<Napi::String>();
  this->vm_.load((std::string) bin_path);
}

Napi::Value VMWrap::Execute(const Napi::CallbackInfo& info) {
  Napi::String command = info[0].As<Napi::String>();
  return Napi::String::New(info.Env(), this->vm_.execute((std::string) command));
}

Napi::Value VMWrap::Undo(const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(), this->vm_.undo());
}

Napi::Value VMWrap::GetState(const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(), this->vm_.getState());
}

Napi::Value VMWrap::Reset(const Napi::CallbackInfo& info) {
  return Napi::String::New(info.Env(), this->vm_.reset());
}

Napi::Value VMWrap::Revert(const Napi::CallbackInfo& info) {
  Napi::Number step = info[0].As<Napi::Number>();
  return Napi::String::New(info.Env(), this->vm_.revert(step.Int32Value()));
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return VMWrap::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll);