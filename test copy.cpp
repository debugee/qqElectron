#include <node.h>
#include <windows.h>
#include <tchar.h>
#include <string>
#include <iostream>
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

using namespace v8;

class AddonData
{
public:
    explicit AddonData(Isolate *isolate) : call_count(0)
    {
        // Ensure this per-addon-instance data is deleted at environment cleanup.
        node::AddEnvironmentCleanupHook(isolate, DeleteInstance, this);
    }

    // Per-addon data.
    int call_count;

    static void DeleteInstance(void *data)
    {
        delete static_cast<AddonData *>(data);
    }
};

static void Method(const v8::FunctionCallbackInfo<v8::Value> &info)
{
    // Retrieve the per-addon-instance data.
    AddonData *data =
        reinterpret_cast<AddonData *>(info.Data().As<External>()->Value());
    data->call_count++;
    info.GetReturnValue().Set((double)data->call_count);
}

void Initializer(v8::Local<v8::Object> exports,
                 v8::Local<v8::Value> module,
                 v8::Local<v8::Context> context, void *priv)
{
    MessageBox(NULL, _T("Initializer"), NULL, MB_OK);
    Isolate *isolate = context->GetIsolate();
    Isolate::GetCurrent()->GetCurrentContext();
    // Create a new instance of `AddonData` for this instance of the addon and
    // tie its life cycle to that of the Node.js environment.
    AddonData *data = new AddonData(isolate);

    // Wrap the data in a `v8::External` so we can pass it to the method we
    // expose.
    Local<External> external = External::New(isolate, data);

    // Expose the method `Method` to JavaScript, and make sure it receives the
    // per-addon-instance data we created above by passing `external` as the
    // third parameter to the `FunctionTemplate` constructor.
    exports->Set(context,
                 String::NewFromUtf8(isolate, "method").ToLocalChecked(),
                 FunctionTemplate::New(isolate, Method, external)
                     ->GetFunction(context)
                     .ToLocalChecked())
        .FromJust();

    // Local<String> process_name =
    //     String::NewFromUtf8Literal(context->GetIsolate(), "process");
    // Local<Value> process_val;
    // if (!context->Global()->Get(context, process_name).ToLocal(&process_val) || !process_val->IsObject())
    // {
    //     return;
    // }

    // // Local<Object> asdf = process_val.As<Object>();

    // //     Local<Array> props =
    // //   asdf->GetPropertyNames(context).ToLocalChecked();
    // //     for(int i = 0; i < props->Length(); i++)
    // //     {
    // //         char *p = *String::Utf8Value(context->GetIsolate(), props->Get(context, i).ToLocalChecked()->ToString(context).ToLocalChecked());
    // //         std::cout << p << std::endl;
    // //     }

    // // If there is no Process function, or if it is not a function,
    // // bail out
    // //   if (!context->Global()->Get(context, process_name).ToLocal(&process_val) || !process_val->IsObject()) {
    // //         return ;
    // //   }

    // Local<Object> asdf = process_val.As<Object>();
    // Local<String> pid =
    //     String::NewFromUtf8Literal(context->GetIsolate(), "setSourceMapsEnabled");
    // Local<Value> zz = asdf->Get(context, pid).ToLocalChecked();
    // // char *p = *String::Utf8Value(context->GetIsolate(), zz);
    // // char *p1 =p ;
    // // int a =  zz.As<Number>()->Int32Value(context).ToChecked();

    // // asdf->GetPropertyAttributes(context, pid).ToChecked();

    // //   // It is a function; cast it to a Function
    // Local<Function> process_fun = zz.As<Function>();
    // // v8::Local<Value> a0[argc] = {v8_num(4), v8_num(2), v8_num(3), v8_num(2)};
    // Local<Value> a0[1] = {Boolean::New(context->GetIsolate(), true)};
    // // fun->Call(context.local(), fun, argc, a0).ToLocalChecked();
    // if (process_fun->IsFunction())
    //     Local<Value> zzz = process_fun->Call(context, process_val, 1, a0).ToLocalChecked();

              Local<String> process_name =
      String::NewFromUtf8Literal(context->GetIsolate(), "require");
  Local<Value> process_val;
      if (!context->Global()->Get(context, process_name).ToLocal(&process_val) ) {
        return ;
  }
//      Local<Object> asdf = process_val.As<Object>();

//     Local<Array> props =
//   asdf->GetPropertyNames(context).ToLocalChecked();
//     for(int i = 0; i < props->Length(); i++)
//     {
//         String::Utf8Value ppp(context->GetIsolate(), props->Get(context, i).ToLocalChecked()->ToString(context).ToLocalChecked());
//         char *p = *ppp;
//         MessageBoxA(NULL, p, NULL, MB_OK);
//         std::cout << p << std::endl;
//     }

 // It is a function; cast it to a Function
 Local<Function> process_fun = process_val.As<Function>();
v8::Local<Value> a0[1] = { String::NewFromUtf8Literal(context->GetIsolate(), "C:/Users/admin/Desktop/qq/aaaa.js")};
//   process_fun->Call(context, process_fun, 1, a0).ToLocalChecked();
if (process_fun->IsFunction()){
 Local<Value> zz = process_fun->Call(context, context->Global(), 1, a0).ToLocalChecked();
       Local<Object> asdf = zz.As<Object>();
     Local<Array> props =
  asdf->GetPropertyNames(context).ToLocalChecked();
    for(int i = 0; i < props->Length(); i++)
    {
        String::Utf8Value ppp(context->GetIsolate(), props->Get(context, i).ToLocalChecked()->ToString(context).ToLocalChecked());
        char *p = *ppp;
        MessageBoxA(NULL, p, NULL, MB_OK);
        std::cout << p << std::endl;
    }
}


}

#include <tchar.h>
#include "HookStuff.h"
const static intptr_t g_offset_BrowserWindow = 0x23B860;
HOOK_INFO hkInfo;
const static intptr_t g_offset_OnFinishLaunching = 0x2041A0;
HOOK_INFO hkInfo1;
HOOK_INFO hkInfo2;
HOOK_INFO hkInfo3;

typedef void (*PFN_Initialize)(v8::Local<v8::Object> exports,
                                            v8::Local<v8::Value> module,
                                            v8::Local<v8::Context> context, void *priv);

void proxyBrowserWindowInitialize(v8::Local<v8::Object> exports,
                 v8::Local<v8::Value> module,
                 v8::Local<v8::Context> context, void * priv)
{
    PFN_Initialize pfn = (PFN_Initialize)GetNewAddress(&hkInfo);
    pfn(exports, module, context, priv);
    MessageBox(NULL, _T("hook ok"), NULL, MB_OK);

          Local<String> process_name =
      String::NewFromUtf8Literal(context->GetIsolate(), "this");
  Local<Value> process_val;
      if (!context->Global()->Get(context, process_name).ToLocal(&process_val) ) {
        return ;
  }
    MessageBoxA(NULL, *String::Utf8Value(context->GetIsolate(), process_val->TypeOf(context->GetIsolate())), NULL, MB_OK);

//      Local<Object> asdf = process_val.As<Object>();

//     Local<Array> props =
//   asdf->GetPropertyNames(context).ToLocalChecked();
//     for(int i = 0; i < props->Length(); i++)
//     {
//         String::Utf8Value ppp(context->GetIsolate(), props->Get(context, i).ToLocalChecked()->ToString(context).ToLocalChecked());
//         char *p = *ppp;
//         MessageBoxA(NULL, p, NULL, MB_OK);
//         std::cout << p << std::endl;
//     }

 // It is a function; cast it to a Function
 Local<Function> process_fun = process_val.As<Function>();
v8::Local<Value> a0[1] = { String::NewFromUtf8Literal(context->GetIsolate(), "C:/Users/admin/Desktop/qq/aaaa.js")};
//   process_fun->Call(context, process_fun, 1, a0).ToLocalChecked();
if (process_fun->IsFunction()){
 Local<Value> zz = process_fun->Call(context, context->Global(), 1, a0).ToLocalChecked();
       Local<Object> asdf = zz.As<Object>();
     Local<Array> props =
  asdf->GetPropertyNames(context).ToLocalChecked();
    for(int i = 0; i < props->Length(); i++)
    {
        String::Utf8Value ppp(context->GetIsolate(), props->Get(context, i).ToLocalChecked()->ToString(context).ToLocalChecked());
        char *p = *ppp;
        MessageBoxA(NULL, p, NULL, MB_OK);
        std::cout << p << std::endl;
    }
}

}

void proxyOnFinishLaunching(void *p1, void *p2)
{
    MessageBox(NULL, _T("proxyOnFinishLaunching"), NULL, MB_OK);
    Isolate *isolate = Isolate::GetCurrent();
    Local<Context> context = isolate->GetCurrentContext();
    HandleScope scope(isolate);
    Local<String> process_name =
        String::NewFromUtf8Literal(context->GetIsolate(), "app");
    Local<Value> process_val;
    if (!context->Global()->Get(context, process_name).ToLocal(&process_val) || !process_val->IsObject())
    {
        return;
    }
    Local<Object> asdf = process_val.As<Object>();

    Local<Array> props =
        asdf->GetPropertyNames(context).ToLocalChecked();
    for (int i = 0; i < props->Length(); i++)
    {
        String::Utf8Value ppp(context->GetIsolate(), props->Get(context, i).ToLocalChecked()->ToString(context).ToLocalChecked());
        char *p = *ppp;
        MessageBoxA(NULL, p, NULL, MB_OK);
        std::cout << p << std::endl;
    }
}

void proxyInitialize(v8::Local<v8::Object> exports,
                 v8::Local<v8::Value> module,
                 v8::Local<v8::Context> context, void * priv)
{
    PFN_Initialize pfn = (PFN_Initialize)GetNewAddress(&hkInfo3);
    pfn(exports, module, context, priv);
    MessageBox(NULL, _T("proxyInitialize ok"), NULL, MB_OK);


             Local<String> process_name =
      String::NewFromUtf8Literal(context->GetIsolate(), "require");
  Local<Value> process_val;
      if (!module->ToObject(context).ToLocalChecked()->Get(context, process_name).ToLocal(&process_val) ) {
        return ;
  }
    MessageBoxA(NULL, *String::Utf8Value(context->GetIsolate(), process_val->TypeOf(context->GetIsolate())), NULL, MB_OK);


 Local<Function> process_fun = process_val.As<Function>();
v8::Local<Value> a0[1] = { String::NewFromUtf8Literal(context->GetIsolate(), "C:/Users/admin/Desktop/qq/aaaa.js")};
//   process_fun->Call(context, process_fun, 1, a0).ToLocalChecked();
if (process_fun->IsFunction()){
 Local<Value> zz = process_fun->Call(context, process_val, 1, a0).ToLocalChecked();
       Local<Object> asdf = zz.As<Object>();
     Local<Array> props =
  asdf->GetPropertyNames(context).ToLocalChecked();
    for(int i = 0; i < props->Length(); i++)
    {
        String::Utf8Value ppp(context->GetIsolate(), props->Get(context, i).ToLocalChecked()->ToString(context).ToLocalChecked());
        char *p = *ppp;
        MessageBoxA(NULL, p, NULL, MB_OK);
        std::cout << p << std::endl;
    }
}

}

#include <mutex>
void proxy_qq_magic_node_register(node::node_module *module)
{
    if (std::string(module->nm_modname) == "launcher")
    {
        //MessageBoxA(NULL, module->nm_filename, module->nm_modname, MB_OK);
        static std::once_flag flag;
        std::call_once(flag, [&module]{
            HookByAddress((PVOID)module->nm_context_register_func, &hkInfo3, proxyInitialize);
        });
    }
}

bool initDLL()
{
    // hook electron app v8 object, and call Initializer

    // PVOID pfnBrowserWindowInitialize = (PVOID)((intptr_t)GetModuleHandle(NULL) + g_offset_BrowserWindow);
    // HookByAddress(pfnBrowserWindowInitialize, &hkInfo, proxyBrowserWindowInitialize);

    // PVOID pfnOnFinishLaunching = (PVOID)((intptr_t)GetModuleHandle(NULL) + g_offset_OnFinishLaunching);
    // HookByAddress(pfnOnFinishLaunching, &hkInfo1, proxyOnFinishLaunching, FALSE, TRUE);

    HookByAddress(GetProcAddress(GetModuleHandle(NULL), "qq_magic_node_register"), &hkInfo2, proxy_qq_magic_node_register, FALSE, TRUE);
    return true;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
#ifdef NDEBUG
        MessageBox(NULL, _T("test.node"), NULL, MB_OK);
        initDLL();
#endif
        break;
    }
    return TRUE;
}

#ifndef NDEBUG
NODE_MODULE_CONTEXT_AWARE(NODE_GYP_MODULE_NAME, Initializer)
#endif
