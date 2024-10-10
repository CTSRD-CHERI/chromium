// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PPAPI_PROXY_PPP_CLASS_PROXY_H_
#define PPAPI_PROXY_PPP_CLASS_PROXY_H_

#include <stdint.h>

#include <vector>

#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/proxy/interface_proxy.h"

struct PPB_Var_Deprecated;

namespace ppapi {
namespace proxy {

class SerializedVar;
class SerializedVarReceiveInput;
class SerializedVarVectorReceiveInput;
class SerializedVarOutParam;
class SerializedVarReturnValue;

class PPP_Class_Proxy : public InterfaceProxy {
 public:
  // PPP_Class isn't a normal interface that you can query for, so this
  // constructor doesn't take an interface pointer.
  explicit PPP_Class_Proxy(Dispatcher* dispatcher);

  PPP_Class_Proxy(const PPP_Class_Proxy&) = delete;
  PPP_Class_Proxy& operator=(const PPP_Class_Proxy&) = delete;

  ~PPP_Class_Proxy() override;

  // Factory function used for registration (normal code can just use the
  // constructor).
  static InterfaceProxy* Create(Dispatcher* dispatcher);

  // Creates a proxied object in the browser process. This takes the browser's
  // PPB_Var_Deprecated interface to use to create the object. The class and
  static PP_Var CreateProxiedObject(const PPB_Var_Deprecated* var,
                                    Dispatcher* dispatcher,
                                    PP_Instance instance_id,
#if defined(__CHERI_PURE_CAPABILITY__)
                                    intptr_t ppp_class,
                                    intptr_t class_data);
#else   // !__CHERI_PURE_CAPABILITY__
                                    int64_t ppp_class,
                                    int64_t class_data);
#endif  // !__CHERI_PURE_CAPABILITY__

  static PP_Bool IsInstanceOf(const PPB_Var_Deprecated* ppb_var_impl,
                              const PP_Var& var,
#if defined(__CHERI_PURE_CAPABILITY__)
                              intptr_t ppp_class,
                              intptr_t* ppp_class_data);
#else   // !__CHERI_PURE_CAPABILITY__
                              int64_t ppp_class,
                              int64_t* ppp_class_data);
#endif  // !__CHERI_PURE_CAPABILITY__

  // InterfaceProxy implementation.
  bool OnMessageReceived(const IPC::Message& msg) override;

 private:
  // IPC message handlers.
#if defined(__CHERI_PURE_CAPABILITY__)
  void OnMsgHasProperty(intptr_t ppp_class,
                        intptr_t object,
#else   // !__CHERI_PURE_CAPABILITY__
  void OnMsgHasProperty(int64_t ppp_class,
                        int64_t object,
#endif  // !__CHERI_PURE_CAPABILITY__
                        SerializedVarReceiveInput property,
                        SerializedVarOutParam exception,
                        bool* result);
#if defined(__CHERI_PURE_CAPABILITY__)
  void OnMsgHasMethod(intptr_t ppp_class,
                      intptr_t object,
#else   // !__CHERI_PURE_CAPABILITY__
  void OnMsgHasMethod(int64_t ppp_class,
                      int64_t object,
#endif  // !__CHERI_PURE_CAPABILITY__
                      SerializedVarReceiveInput property,
                      SerializedVarOutParam exception,
                      bool* result);
#if defined(__CHERI_PURE_CAPABILITY__)
  void OnMsgGetProperty(intptr_t ppp_class,
                        intptr_t object,
#else   // !__CHERI_PURE_CAPABILITY__
  void OnMsgGetProperty(int64_t ppp_class,
                        int64_t object,
#endif  // !__CHERI_PURE_CAPABILITY__
                        SerializedVarReceiveInput property,
                        SerializedVarOutParam exception,
                        SerializedVarReturnValue result);
#if defined(__CHERI_PURE_CAPABILITY__)
  void OnMsgEnumerateProperties(intptr_t ppp_class,
                                intptr_t object,
#else   // !__CHERI_PURE_CAPABILITY__
  void OnMsgEnumerateProperties(int64_t ppp_class,
                                int64_t object,
#endif  // !__CHERI_PURE_CAPABILITY__
                                std::vector<SerializedVar>* props,
                                SerializedVarOutParam exception);
#if defined(__CHERI_PURE_CAPABILITY__)
  void OnMsgSetProperty(intptr_t ppp_class,
                        intptr_t object,
#else   // !__CHERI_PURE_CAPABILITY__
  void OnMsgSetProperty(int64_t ppp_class,
                        int64_t object,
#endif  // !__CHERI_PURE_CAPABILITY__
                        SerializedVarReceiveInput property,
                        SerializedVarReceiveInput value,
                        SerializedVarOutParam exception);
#if defined(__CHERI_PURE_CAPABILITY__)
  void OnMsgRemoveProperty(intptr_t ppp_class,
                           intptr_t object,
#else   // !__CHERI_PURE_CAPABILITY__
  void OnMsgRemoveProperty(int64_t ppp_class,
                           int64_t object,
#endif  // !__CHERI_PURE_CAPABILITY__
                           SerializedVarReceiveInput property,
                           SerializedVarOutParam exception);
#if defined(__CHERI_PURE_CAPABILITY__)
  void OnMsgCall(intptr_t ppp_class,
                 intptr_t object,
#else   // !__CHERI_PURE_CAPABILITY__
  void OnMsgCall(int64_t ppp_class,
                 int64_t object,
#endif  // !__CHERI_PURE_CAPABILITY__
                 SerializedVarReceiveInput method_name,
                 SerializedVarVectorReceiveInput arg_vector,
                 SerializedVarOutParam exception,
                 SerializedVarReturnValue result);
#if defined(__CHERI_PURE_CAPABILITY__)
  void OnMsgConstruct(intptr_t ppp_class,
                      intptr_t object,
#else   // !__CHERI_PURE_CAPABILITY__
  void OnMsgConstruct(int64_t ppp_class,
                      int64_t object,
#endif  // !__CHERI_PURE_CAPABILITY__
                      SerializedVarVectorReceiveInput arg_vector,
                      SerializedVarOutParam exception,
                      SerializedVarReturnValue result);
#if defined(__CHERI_PURE_CAPABILITY__)
  void OnMsgDeallocate(intptr_t ppp_class, intptr_t object);
#else   // !__CHERI_PURE_CAPABILITY__
  void OnMsgDeallocate(int64_t ppp_class, int64_t object);
#endif  // !__CHERI_PURE_CAPABILITY__

  // Returns true if the given class/data points to a plugin-implemented
  // object. On failure, the exception, if non-NULL, will also be set.
#if defined(__CHERI_PURE_CAPABILITY__)
  bool ValidateUserData(intptr_t ppp_class,
                        intptr_t class_data,
#else   // !__CHERI_PURE_CAPABILITY__
  bool ValidateUserData(int64_t ppp_class,
                        int64_t class_data,
#endif  // !__CHERI_PURE_CAPABILITY__
                        SerializedVarOutParam* exception);
};

}  // namespace proxy
}  // namespace ppapi

#endif  // PPAPI_PROXY_PPP_CLASS_PROXY_H_
