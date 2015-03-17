#include <helper/manager/dmanager.h>
#include <helper/logger/dlogger.h>
#include <core/file_types/elffile.h>
#include <core/file_types/pefile.h>

template <typename RegistersType>
bool DManager::__get_descriptions() {
  json_parser.setPath(settings.getDescriptionsPath<RegistersType>());

  // TODO: fill!!!!

  return true;
}
template bool DManager::__get_descriptions<Registers_x86>();
template bool DManager::__get_descriptions<Registers_x64>();

bool DManager::__secure_elf(const QByteArray &data, const DManager::secured_file_info &sfi) {
  ELF elf(data);

  if(elf.is_x64()) {
      /*
      ss = test_one_ex<Registers_x64>(&elf, type, method, handler, x, obfuscate);
      if (ss == SecuredState::NONCOMPATIBLE)
        return true;
      if (ss != SecuredState::SECURED)
        return false;
      */
      // TODO: add error codes
      return __secure_elf<Registers_x64>(&elf, sfi);
  }


  if(elf.is_x86()) {
      /*
      ss = test_one_ex<Registers_x86>(&elf, type, method, handler, x, obfuscate);
      if (ss == SecuredState::NONCOMPATIBLE)
        return true;
      if (ss != SecuredState::SECURED)
        return false;
      */
      return __secure_elf<Registers_x86>(&elf, sfi);
  }


  LOG_ERROR("Elf architecture is not superted");

  return false;
}

template <typename RegistersType>
bool DManager::__secure_elf(ELF *elf, const DManager::secured_file_info &sfi) {

  json_parser.setPath(settings.getDescriptionsPath<RegistersType>());

  ELFAddingMethods<RegistersType> adder(elf);

  Wrapper<RegistersType> *meth = json_parser.loadInjectDescription<RegistersType>(QString("%1.json").arg(sfi.get_dd_method()));
  Wrapper<RegistersType> *wrapper = nullptr; //TODO: cahnge // json_parser.loadInjectDescription<RegistersType>(); elf->is_x86() ? wrappers_x86[type] : wrappers_x64[type])

  if (!meth) {
    LOG_ERROR(QString("Specified debugger detection method %s is absent").arg(sfi.get_dd_method()));
    return false;
  }

  if (!wrapper) {
    LOG_ERROR(QString("Specified wrapper method %s is absent").arg(""));
    return false;
  }

  // set detection handler
  wrapper->detect_handler = json_parser.loadInjectDescription<RegistersType>(QString("%1.json").arg(sfi.get_dd_handler()));

  if (!wrapper->detect_handler) {
    LOG_ERROR(QString("Specified debugger detection handler %s is absent").arg(sfi.get_dd_handler()));
    return false;
  }

  // TODO: wtf???
  wrapper->ret = meth->ret;

  switch(sfi.get_adding_method()) {
    case AddingMethodType::OEP : {
      OEPWrapper<RegistersType> *oep_wrapper = dynamic_cast<OEPWrapper<RegistersType>*>(wrapper);
      if (!oep_wrapper) {
        LOG_ERROR("Error");
        return false;
      }
      oep_wrapper->oep_action = meth;
      break;
    }
    case AddingMethodType::Thread: {
      ThreadWrapper<RegistersType> *thread_wrapper = dynamic_cast<ThreadWrapper<RegistersType>*>(wrapper);
      if (!thread_wrapper) {
        LOG_ERROR("Error");
        return false;
      }
      thread_wrapper->thread_actions = { meth };
      break;
    }

    case AddingMethodType::Trampoline:
    case AddingMethodType::INIT_ARRAY:
    case AddingMethodType::CTORS:
    case AddingMethodType::INIT: {
      TrampolineWrapper<RegistersType> *tramp_wrapper = dynamic_cast<TrampolineWrapper<RegistersType>*>(wrapper);
      if (!tramp_wrapper) {
        LOG_ERROR("Error");
        return false;
      }
      break;
      tramp_wrapper->tramp_action = meth;
    }
    default:
      LOG_ERROR("Error");
      return false;
  }

  typename DAddingMethods<RegistersType>::InjectDescription id;
  id.adding_method = wrapper;
  id.cm = static_cast<typename DAddingMethods<RegistersType>::CallingMethod>(sfi.get_adding_method());
  id.change_x_only = sfi.get_change_x();

  if (!meth->allowed_methods.contains(static_cast<typename DAddingMethods<RegistersType>::CallingMethod>(id.cm)) ||
      !wrapper->detect_handler->allowed_methods.contains(static_cast<typename DAddingMethods<RegistersType>::CallingMethod>(sfi.get_adding_method())) ||
      (meth->only_rwx & id.change_x_only) || (!meth->obfuscation & sfi.get_obfuscate())) {
    LOG_ERROR("Invalid configuration of chosen methods");
    return false;
  }

  QList<typename DAddingMethods<RegistersType>::InjectDescription*> ids = { &id };

  LOG_MSG(QString("Secure using wrapper: %1 \n\tmethod: %2\n\thandler: %3").arg(elf->is_x86() ? "" : "", sfi.get_dd_method(), sfi.get_dd_handler()));

  bool s = adder.secure(ids);
  if (sfi.get_obfuscate())
    // TODO: specify params
    s &= adder.obfuscate(5, 10, 20);

  if (!meth->detect_handler) {
    delete meth->detect_handler;
    meth->detect_handler = nullptr;
  }

  delete meth;
  meth = nullptr;

  return s;
}
template bool DManager::__secure_elf<Registers_x86>(ELF *elf, const DManager::secured_file_info &sfi);
template bool DManager::__secure_elf<Registers_x64>(ELF *elf, const DManager::secured_file_info &sfi);

bool DManager::__secure_pe(const QByteArray &data, const DManager::secured_file_info &sfi) {
  // TODO: fill
  return true;
}

DManager::DManager():
  settings(DSettings::getSettings()) {

  if (!settings.loaded()) {
    LOG_ERROR("Setting file is not loaded");
    throw std::runtime_error("Setting file is not loaded");
  }
  // configure mapper
  mapper["Method"]  = &dd_methods;
  mapper["Handler"] = &dd_handlers;
  mapper["Wrapper"] = &adding_methods;

  if (!__get_descriptions<Registers_x86>()) {
    LOG_ERROR("Error while parsing x86 description files");
    throw std::runtime_error("Error while parsing x86 description files");
  }

  if (!__get_descriptions<Registers_x64>()) {
    LOG_ERROR("Error while parsing x64 description files");
    throw std::runtime_error("Error while parsing x64 description files");
  }



}

template <typename RegistersType>
bool DManager::__secure_pe(PEFile *pe, const DManager::secured_file_info &sfi) {
  return true;
}
template bool DManager::__secure_pe<Registers_x86>(PEFile *pe, const DManager::secured_file_info &sfi);
template bool DManager::__secure_pe<Registers_x64>(PEFile *pe, const DManager::secured_file_info &sfi);

bool DManager::secure(const DManager::secured_file_info &sfi) {
  // check file type
  QFile in(sfi.get_file_name());
  if(!in.open(QFile::ReadOnly)) {
      LOG_ERROR(QString("Could not open specified file: %s").arg(sfi.get_file_name()));
      return false;
  }

  // read whole file content
  QByteArray data = in.readAll(); // is passed as parameter to avoid race condition

  // TODO: change and save file
  bool s;
  // check if ELF / PE type
  if (ELF(data).is_valid()) {
    s = __secure_elf(data, sfi);
    if (!s) {
      LOG_WARN("Secure failed");
      return false;
    }

    return s;
  }
  if (PEFile(data).is_valid())
    return __secure_pe(data, sfi);

  LOG_ERROR(QString("Specified file %s is non-ELF neither non-PE").arg(sfi.get_file_name()));
  return false;
}

QString DManager::secured_file_info::get_file_name() const {
  return file_name;
}

void DManager::secured_file_info::set_file_name(const QString &value) {
  file_name = value;
}

DManager::AddingMethodType DManager::secured_file_info::get_adding_method() const {
  return adding_method;
}

void DManager::secured_file_info::set_adding_method(const AddingMethodType &value) {
  adding_method = value;
}

QString DManager::secured_file_info::get_dd_method() const {
  return dd_method;
}

void DManager::secured_file_info::set_dd_method(const QString &value) {
  dd_method = value;
}

QString DManager::secured_file_info::get_dd_handler() const {
  return dd_handler;
}

void DManager::secured_file_info::set_dd_handler(const QString &value) {
  dd_handler = value;
}

bool DManager::secured_file_info::get_change_x() const {
  return change_x;
}

void DManager::secured_file_info::set_change_x(bool value) {
  change_x = value;
}

bool DManager::secured_file_info::get_obfuscate() const {
  return obfuscate;
}

void DManager::secured_file_info::set_obfuscate(bool value) {
  obfuscate = value;
}

bool DManager::secured_file_info::get_pack() const {
  return pack;
}

void DManager::secured_file_info::set_pack(bool value) {
  pack = value;
}

