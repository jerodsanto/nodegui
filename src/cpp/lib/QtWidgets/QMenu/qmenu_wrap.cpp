#include "QtWidgets/QMenu/qmenu_wrap.h"
#include <nodegui/QtWidgets/QWidget/qwidget_wrap.h>
#include <nodegui/Extras/Utils/nutils.h>
#include <QWidget>

Napi::FunctionReference QMenuWrap::constructor;

Napi::Object QMenuWrap::init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);
  char CLASSNAME[] = "QMenu";
  Napi::Function func = DefineClass(env, CLASSNAME, {
    InstanceMethod("setTitle", &QMenuWrap::setTitle),
    QWIDGET_WRAPPED_METHODS_EXPORT_DEFINE(QMenuWrap)
  });
  constructor = Napi::Persistent(func);
  exports.Set(CLASSNAME, func);
  return exports;
}

NMenu* QMenuWrap::getInternalInstance() {
  return this->instance.get();
}

QMenuWrap::QMenuWrap(const Napi::CallbackInfo& info): Napi::ObjectWrap<QMenuWrap>(info)  {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if(info.Length() == 1) {
    Napi::Object parentObject = info[0].As<Napi::Object>();
    QWidgetWrap* parentWidgetWrap = Napi::ObjectWrap<QWidgetWrap>::Unwrap(parentObject);
    this->instance = std::make_unique<NMenu>(parentWidgetWrap->getInternalInstance()); //this sets the parent to current widget
  }else if (info.Length() == 0){
    this->instance = std::make_unique<NMenu>();
  }else {
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
  }
  // Adds measure function on yoga node so that widget size is calculated based on its text also.
  YGNodeSetMeasureFunc(this->instance->getFlexNode(), &extrautils::measureQtWidget);
}

QMenuWrap::~QMenuWrap() {
  this->instance.reset();
}

Napi::Value QMenuWrap::setTitle(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  
  Napi::String message = info[0].As<Napi::String>();
  this->instance->setTitle(QString::fromStdString(message.Utf8Value()));

  return env.Null();
}
