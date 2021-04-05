#include "captcha.h"

using namespace Napi;

Napi::String OpenCvVersionWrapped(const CallbackInfo& info) {
  Env env = info.Env();
  return Napi::String::New(env, Captcha::get_opencv_ver());
}

Value InitializeWrapped(const CallbackInfo& info) {
  Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  try {
    string characters;

    if (info.Length() == 0) {
      characters = Captcha::initialize();
    }
    else {
      characters = Captcha::initialize(info[0].As<Napi::String>());
    }

    deferred.Resolve(Napi::String::New(env, characters));
  }
  catch (const exception& e) {
    deferred.Reject(Napi::Error::New(env, e.what()).Value());
  }

  return deferred.Promise();
}

Value SolveWrapped(const CallbackInfo& info) {
  Env env = info.Env();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  string error, result;

  try {
    if (!Captcha::has_initialized()) {
      error = "initialize() must be called before solve().";
    }
    else if (info.Length() == 0) {
      error = "1 argument required: a file path or buffer.";
    }
    else {
      auto arg = info[0];

      if (arg.IsString()) {
        result = Captcha::solve_captcha_image(arg.As<Napi::String>());
      }
      else if (arg.IsBuffer()) {
        auto buffer = arg.As<Napi::Buffer<char>>();
        result = Captcha::solve_captcha_buffer(buffer.Data(), buffer.Length());
      }
      else {
        error = "Unknown argument type.";
      }
    }
  }
  catch (const exception& e) {
    error = e.what();
  }

  if (error.length() > 0) {
    deferred.Reject(Napi::Error::New(env, error).Value());
  }
  else {
    deferred.Resolve(Napi::String::New(env, result));
  }

  return deferred.Promise();
}

Object Init(Env env, Object exports) {
  exports.Set("openCvVersion", Function::New(env, OpenCvVersionWrapped));
  exports.Set("initialize", Function::New(env, InitializeWrapped));
  exports.Set("solve", Function::New(env, SolveWrapped));
  return exports;
}

NODE_API_MODULE(addon, Init)
