#include <node.h>
#include <windows.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <mutex>
#include <tchar.h>
#include "HookStuff.h"

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
   // MessageBox(NULL, _T("Initializer"), NULL, MB_OK);
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
}


HOOK_INFO hkInfo;
HOOK_INFO hkInfo1;

typedef void (*PFN_Initialize)(v8::Local<v8::Object> exports,
                               v8::Local<v8::Value> module,
                               v8::Local<v8::Context> context, void *priv);

void proxyInitialize(v8::Local<v8::Object> exports,
                     v8::Local<v8::Value> module,
                     v8::Local<v8::Context> context, void *priv)
{
    PFN_Initialize pfn = (PFN_Initialize)GetNewAddress(&hkInfo1);
    pfn(exports, module, context, priv);
    //MessageBox(NULL, _T("proxyInitialize ok"), NULL, MB_OK);

    Local<String> nameRequire =
        String::NewFromUtf8Literal(context->GetIsolate(), "require");
    Local<Value> requireVal;
    if (!module->ToObject(context).ToLocalChecked()->Get(context, nameRequire).ToLocal(&requireVal))
    {
        return;
    }

    Local<Function> funRequire = requireVal.As<Function>();
    v8::Local<Value> a0[1] = {String::NewFromUtf8Literal(context->GetIsolate(), "C:/Users/admin/Desktop/qq/inject.js")};
    if (funRequire->IsFunction())
    {
        Local<Value> exportValue = funRequire->Call(context, funRequire, 1, a0).ToLocalChecked();
    }
}


void proxy_qq_magic_node_register(node::node_module *module)
{
    if (std::string(module->nm_modname) == "launcher")
    {
        static std::once_flag flag;
        std::call_once(flag, [&module]{ 
            HookByAddress((PVOID)module->nm_context_register_func, &hkInfo1, proxyInitialize); 
        });
    }
}

bool initDLL()
{
    HookByAddress(GetProcAddress(GetModuleHandle(NULL), "qq_magic_node_register"),
                  &hkInfo, proxy_qq_magic_node_register, FALSE, TRUE);
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
